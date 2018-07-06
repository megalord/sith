#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Target.h>

#include "codegen.h"
#include "module.h"
#include "utils.h"

typedef void* (*fn_zero_arg_t) ();
typedef void* (*fn_one_arg_t) (void*);
typedef void* (*fn_two_arg_t) (void*, void*);
typedef void* (*fn_three_arg_t) (void*, void*, void*);

int eval_if(module_t* mod, symbol_table_t* table, list_t* list, LLVMBuilderRef builder, LLVMValueRef* result) {
  LLVMValueRef condition, results[2];
  LLVMValueRef fn = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));
  LLVMBasicBlockRef bbs[2] = {
    LLVMAppendBasicBlock(fn, "then"),
    LLVMAppendBasicBlock(fn, "else")
  };
  LLVMBasicBlockRef end_bb = LLVMAppendBasicBlock(fn, "if_cont");

  node_t* node = list->fst->next;
  if (eval_statement(mod, table, node, builder, &condition) != 0) {
    return 1;
  }
  LLVMBuildCondBr(builder, condition, bbs[0], bbs[1]);

  for (int i = 0; i < 2; i++) {
    LLVMPositionBuilderAtEnd(builder, bbs[i]);
    node = node->next;
    if (eval_statement(mod, table, node, builder, results + i) != 0) {
      return 1;
    }
    LLVMBuildBr(builder, end_bb);
    bbs[i] = LLVMGetInsertBlock(builder);
  }

  LLVMPositionBuilderAtEnd(builder, end_bb);
  *result = LLVMBuildPhi(builder, LLVMTypeOf(results[0]), "iftmp");
  LLVMAddIncoming(*result, results, bbs, 2);
  return 0;
}

int add_case(module_t* mod, symbol_table_t* table, node_t* node, LLVMBuilderRef builder, LLVMValueRef switch_val, LLVMBasicBlockRef bb) {
  LLVMValueRef on_val;
  if (eval_statement(mod, table, node, builder, &on_val) != 0) {
    return 1;
  }
  LLVMAddCase(switch_val, on_val, bb);
  return 0;
}

int eval_switch(module_t* mod, symbol_table_t* table, list_t* list, LLVMBuilderRef builder, LLVMValueRef* result) {
  LLVMValueRef condition;
  LLVMValueRef fn = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));

  unsigned num_cases = list->len - 2;
  node_t* node = list->fst->next;
  if (eval_statement(mod, table, node, builder, &condition) != 0) {
    return 1;
  }
  LLVMBasicBlockRef else_bb = LLVMAppendBasicBlock(fn, "else");
  LLVMBasicBlockRef end_bb = LLVMAppendBasicBlock(fn, "switch_cont");
  LLVMValueRef switch_val = LLVMBuildSwitch(builder, condition, else_bb, num_cases);

  LLVMBasicBlockRef* bbs = malloc(sizeof(LLVMBasicBlockRef) * num_cases);
  LLVMValueRef* results = malloc(sizeof(LLVMValueRef) * num_cases);
  char case_name[8];
  node_t* sub_node;
  int num_exprs;
  for (int i = 0; i < num_cases; i++) {
    node = node->next;
    if (node->type != NODE_LIST || node->list->len != 2) {
      fprintf(stderr, "expected list with length 2\n");
      return 1;
    }
    snprintf(case_name, 8, "case_%01d", i);
    bbs[i] = LLVMAppendBasicBlock(fn, case_name);

    sub_node = node->list->fst;
    switch (sub_node->type) {
      case NODE_ATOM:
        if (add_case(mod, table, sub_node, builder, switch_val, bbs[i]) != 0) {
          return 1;
        }
        break;
      case NODE_LIST:
        num_exprs = sub_node->list->len;
        sub_node = sub_node->list->fst;
        for (int j = 0; j < num_exprs; j++) {
          if (add_case(mod, table, sub_node, builder, switch_val, bbs[i]) != 0) {
            return 1;
          }
          sub_node = sub_node->next;
        }
        break;
      default:
        fprintf(stderr, "unknown node type\n");
        return 1;
    }

    LLVMPositionBuilderAtEnd(builder, bbs[i]);
    if (eval_statement(mod, table, node->list->fst->next, builder, &results[i]) != 0) {
      return 1;
    }
    LLVMBuildBr(builder, end_bb);
    bbs[i] = LLVMGetInsertBlock(builder);
  }

  // TODO: handle default
  LLVMPositionBuilderAtEnd(builder, else_bb);
  LLVMBuildUnreachable(builder);

  LLVMPositionBuilderAtEnd(builder, end_bb);
  *result = LLVMBuildPhi(builder, LLVMTypeOf(results[0]), "switchtmp");
  LLVMAddIncoming(*result, results, bbs, num_cases);

  free(bbs);
  return 0;
}

int eval_statement(module_t* mod, symbol_table_t* table, node_t* node, LLVMBuilderRef builder, LLVMValueRef* result) {
  if (node->type == NODE_LIST) {
    int arity = node->list->len - 1;
    if (node->list->fst->type != NODE_ATOM) {
      fprintf(stderr, "atom expected\n");
      return 1;
    }

    char* sub_fn_name = node->list->fst->atom->name;

    // handle special statements first
    if (strcmp(sub_fn_name, "if") == 0) {
      return eval_if(mod, table, node->list, builder, result);
    } else if (strcmp(sub_fn_name, "cond") == 0) {
      return eval_switch(mod, table, node->list, builder, result);
    }

    node = node->list->fst;
    LLVMValueRef* args = malloc(arity * sizeof(LLVMValueRef));
    symbol_t* sym_local;
    int eval_result;
    for (int i = 0; i < arity; i++) {
      node = node->next;
      if (node->type == NODE_LIST) {
        eval_result = eval_statement(mod, table, node, builder, &args[i]);
        if (eval_result != 0) {
          return eval_result;
        }
      } else {
        switch (node->atom->type) {
          case ATOM_CHAR:
            fprintf(stderr, "ATOM_CHAR constants not supported\n");
            return 1;
          case ATOM_IDENTIFIER:
            sym_local = symbol_table_get(table, node->atom->name);
            if (sym_local == NULL) {
              fprintf(stderr, "symbol \"%s\" not found (%d:%d)\n", node->atom->name, node->atom->line, node->atom->pos);
              return 1;
            }
            args[i] = sym_local->value;
            break;
          case ATOM_INT:
            args[i] = LLVMConstInt(LLVMInt32Type(), atoi(node->atom->name), 0);
            break;
          case ATOM_STRING:
            //args[i] = LLVMConstString(node->atom->name, strlen(node->atom->name), false);
            args[i] = LLVMBuildGlobalStringPtr(builder, node->atom->name, "str");
            break;
        }
      }
    }


    if (strcmp(sub_fn_name, "eq") == 0) {
      if (arity != 2) {
        fprintf(stderr, "eq called with %d args, expected 2\n", arity);
        return 1;
      }
      *result = LLVMBuildICmp(builder, LLVMIntEQ, args[0], args[1], "eq");
      return 0;
    }

    symbol_t* sym = symbol_table_get(table, sub_fn_name);
    if (sym == NULL) {
      sym = module_deps_symbol_find(mod, sub_fn_name);
      if (sym != NULL) {
        sym = symbol_table_add(&mod->table, sym);
        sym->value = LLVMAddFunction(mod->llvm, sym->name, LLVMGetElementType(LLVMTypeOf(sym->value)));
      }
    }
    if (sym == NULL) {
      fprintf(stderr, "symbol \"%s\" not found (%d:%d)\n", sub_fn_name, node->atom->line, node->atom->pos);
      return 1;
    }
    if (sym->type.meta != TYPE_FUNC) {
      fprintf(stderr, "symbol \"%s\" is not a function (%d:%d)\n", sub_fn_name, node->atom->line, node->atom->pos);
      return 1;
    }

    *result = LLVMBuildCall(builder, sym->value, args, arity, sym->name);
    return 0;
  } else {
    switch (node->atom->type) {
      case ATOM_INT:
        *result = LLVMConstInt(LLVMInt32Type(), atoi(node->atom->name), 0);
        return 0;
      default:
        fprintf(stderr, "cannot eval atom type %d\n", node->atom->type);
        return 1;
    }
  }
  return 1;
}

LLVMTypeRef type_to_llvm (type_t* type) {
  if (strcmp(type->name, "I8") == 0) {
    return LLVMInt8Type();
  } else if (strcmp(type->name, "I32") == 0) {
    return LLVMInt32Type();
  } else if (strcmp(type->name, "Ptr") == 0) {
    if (type->num_fields != 1) {
      fprintf(stderr, "Ptr type must have 1 field, got %d\n", type->num_fields);
      return NULL;
    }
    return LLVMPointerType(type_to_llvm(&type->fields[0]), 0);
  } else {
    fprintf(stderr, "cannot convert type %s\n", type->name);
    return NULL;
  }
}

int compile_fn (module_t* mod, symbol_t* sym) {
  type_t type = sym->type;
  int i;
  LLVMTypeRef* param_types = malloc((type.num_fields - 1) * sizeof(LLVMTypeRef));
  for (i = 0; i < type.num_fields - 1; i++) {
    param_types[i] = type_to_llvm(&type.fields[i]);
    if (param_types[i] == NULL) {
      return 1;
    }
  }
  LLVMTypeRef ret_type = type_to_llvm(&type.fields[i]);
  if (ret_type == NULL) {
    return 1;
  }
  LLVMTypeRef fn_type = LLVMFunctionType(ret_type, param_types, type.num_fields - 1, 0);
  sym->value = LLVMAddFunction(mod->llvm, sym->name, fn_type);

  symbol_table_t* table = malloc(sizeof(symbol_table_t));
  table->num_symbols = type.num_fields - 1;
  table->max_symbols = table->num_symbols;
  table->parent = &mod->table;

  //LLVMValueRef* params = malloc((type.num_fields - 1) * sizeof(LLVMValueRef));
  symbol_t* params = malloc(table->num_symbols * sizeof(symbol_t));
  for (i = 0; i < type.num_fields - 1; i++) {
    //params[i] = LLVMGetParam(sym->value, i);
    params[i].name = type.field_names[i];
    params[i].type = type.fields[i];
    params[i].value = LLVMGetParam(sym->value, i);
  }
  table->symbols = params;

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(sym->value, "entry");

  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder, entry);
  //LLVMValueRef tmp = LLVMBuildAdd(builder, , , "tmp");
  //LLVMValueRef tmp = LLVMBuildCall(builder, get_puts(ll_mod), args, num_args, name);

  LLVMValueRef result;
  node_t* body = sym->data;
  while (body != NULL) {
    if (eval_statement(mod, table, body, builder, &result) != 0) {
      return 1;
    }
    body = body->next;
  }
  LLVMBuildRet(builder, result);
  return 0;
}

int parse_ir (module_t* mod) {
  LLVMMemoryBufferRef buf;
  char* msg = NULL;

  if (LLVMCreateMemoryBufferWithContentsOfFile(mod->name, &buf, &msg)) {
    fprintf(stderr, "Error reading file: %s\n", msg);
    return 1;
  }

  mod->llvm = LLVMModuleCreateWithName(mod->name);
  if (LLVMParseIRInContext(LLVMGetGlobalContext(), buf, &mod->llvm, &msg) != 0) {
    fprintf(stderr, "error parsing IR: %s\n", msg);
    return 1;
  }

  mod->table.parent = NULL;
  mod->table.num_symbols = 1;
  mod->table.symbols = malloc(sizeof(symbol_t));
  mod->table.symbols[0].name = malloc(5);
  strncpy(mod->table.symbols[0].name, "puts", 5);
  mod->table.symbols[0].data = NULL;
  mod->table.symbols[0].type = (type_t) { .meta = TYPE_FUNC, .num_fields = 0, .fields = NULL };
  mod->table.symbols[0].type.name = malloc(6);
  strncpy(mod->table.symbols[0].type.name, "char*", 6);
  mod->table.symbols[0].value = LLVMGetNamedFunction(mod->llvm, "puts");

  //LLVMDisposeMemoryBuffer(buf);
  //LLVMDumpModule(mod->llvm);
  return 0;
}

int module_compile (module_t* mod) {
  for (int i = 0; i < mod->num_deps; i++) {
    if (str_includes(mod->deps[i].name, ".ll")) {
      if (parse_ir(&mod->deps[i]) != 0) {
        return 1;
      }
    } else {
      if (module_compile(&mod->deps[i]) != 0) {
        return 1;
      }
    }
  }
  mod->llvm = LLVMModuleCreateWithName(mod->name);
  symbol_t* sym;
  int len = mod->table.num_symbols;
  for (int i = 0; i < len; i++) {
    sym = &mod->table.symbols[i];
    if (compile_fn(mod, sym) != 0) {
      fprintf(stderr, "error compile function %s\n", sym->name);
      return 1;
    }
  }

  char* error = NULL;
  if (LLVMVerifyModule(mod->llvm, LLVMReturnStatusAction, &error) != 0) {
    fprintf(stderr, "error: %s\n", error);
    LLVMDisposeMessage(error);
    return 1;
  }
  LLVMDisposeMessage(error);

  return 0;
}

int emit_ir (module_t* mod) {
  char* error = NULL;
  char ir_file[100];
  snprintf(ir_file, 100, "%s.ll", mod->name);
  if (LLVMPrintModuleToFile(mod->llvm, ir_file, &error)) {
    fprintf(stderr, "LLVMPrintModuleToFile: %s\n", error);
    return 1;
  }
  LLVMDisposeMessage(error);
  return 0;
}

int emit_object_code (module_t* mod) {
  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  char* triple = LLVMGetDefaultTargetTriple();
  LLVMSetTarget(mod->llvm, triple);

  char* error = NULL;
  LLVMTargetRef target;
  if (LLVMGetTargetFromTriple(triple, &target, &error)) {
    fprintf(stderr, "LLVMGetTargetFromTriple: %s\n", error);
    return 1;
  }
  LLVMDisposeMessage(error);
  error = NULL;

  LLVMTargetMachineRef machine = LLVMCreateTargetMachine(target, triple, "generic", "", LLVMCodeGenLevelDefault, LLVMRelocDefault, LLVMCodeModelDefault);
  LLVMSetModuleDataLayout(mod->llvm, LLVMCreateTargetDataLayout(machine));

  char object_file[100];
  snprintf(object_file, 100, "%s.o", mod->name);
  if (LLVMTargetMachineEmitToFile(machine, mod->llvm, object_file, LLVMObjectFile, &error)) {
    fprintf(stderr, "LLVMTargetMachineEmitToFile: %s\n", error);
    return 1;
  }
  LLVMDisposeMessage(error);

  char cmd[100];
  snprintf(cmd, 100, "clang -g %s.o -o %s", mod->name, mod->name);
  return system(cmd);
}

int exec_main (module_t* mod) {
  LLVMLinkInMCJIT();
  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  char* error = NULL;
  LLVMExecutionEngineRef engine;
  if (LLVMCreateExecutionEngineForModule(&engine, mod->llvm, &error) != 0) {
    fprintf(stderr, "failed to create execution engine\n");
    abort();
  }
  if (error != NULL) {
    fprintf(stderr, "error: %s\n", error);
    LLVMDisposeMessage(error);
    exit(EXIT_FAILURE);
  }

  symbol_t* main = symbol_table_get(&mod->table, "main");
  if (main != NULL) {
    int (*fn)(void) = LLVMGetPointerToGlobal(engine, main->value);
    return (*fn)();
  } else {
    fprintf(stderr, "no main function\n");
    return 1;
  }
}
