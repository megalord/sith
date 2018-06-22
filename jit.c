#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/IRReader.h>

#include "module.h"

typedef void* (*fn_zero_arg_t) ();
typedef void* (*fn_one_arg_t) (void*);
typedef void* (*fn_two_arg_t) (void*, void*);
typedef void* (*fn_three_arg_t) (void*, void*, void*);

LLVMAttributeRef nocapture_attr;
LLVMAttributeRef nounwind_attr;

/*int eval_if(jit_function_t* fn, sj_symbol_table_t* table, node_t* node, jit_value_t* result) {
  node = node->next;
  jit_value_t condition;
  eval_statement(fn, table, node, &condition);
  jit_label_t label = jit_label_undefined;
  jit_label_t end = jit_label_undefined;
  jit_insn_branch_if_not(*fn, condition, &label);

  node = node->next;
  jit_value_t if_result;
  eval_statement(fn, table, node, &if_result);
  *result = jit_value_create(*fn, jit_value_get_type(if_result));
  jit_insn_store(*fn, *result, if_result);
  jit_insn_branch(*fn, &end);

  jit_insn_label(*fn, &label);
  node = node->next;
  jit_value_t else_result;
  eval_statement(fn, table, node, &else_result);
  jit_insn_store(*fn, *result, else_result);
  jit_insn_label(*fn, &end);
  return 0;
}*/

int eval_statement(LLVMBuilderRef builder, module_t* mod, symbol_table_t* table, node_t* node, LLVMValueRef* result) {
  if (node->type == NODE_LIST) {
    int arity = node->list->len - 1;
    node = node->list->fst;
    char* sub_fn_name = node->atom->name;

    // handle special statements first
    /*if (strcmp(sub_fn_name, "if") == 0) {
      return eval_if(fn, table, node, result);
    }*/

    LLVMValueRef* args = malloc(arity * sizeof(LLVMValueRef));
    symbol_t* sym_local;
    int eval_result;
    for (int i = 0; i < arity; i++) {
      node = node->next;
      if (node->type == NODE_LIST) {
        eval_result = eval_statement(builder, mod, table, node, &args[i]);
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
            fprintf(stderr, "ATOM_CHAR constants not supported\n");
            return 1;
          case ATOM_STRING:
            //args[i] = LLVMConstString(node->atom->name, strlen(node->atom->name), false);
            args[i] = LLVMBuildGlobalStringPtr(builder, node->atom->name, "str");
        }
      }
    }


    /*if (strcmp(sub_fn_name, "puts") == 0) {
      *result = LLVMBuildCall(builder, get_puts(ll_mod), args, arity, "puts");
      return 0;
    } else if (strcmp(sub_fn_name, "eq") == 0) {
      *result = jit_insn_eq(*fn, args[0], args[1]);
      return 0;
    } else if (strcmp(sub_fn_name, "printf") == 0) {
      *result = c_printf(fn, node->next->atom->name, arity);
      return 0;
    }*/

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
  }
  fprintf(stderr, "cannot eval atoms\n");
  return 1;
}

LLVMTypeRef type_to_llvm (type_t* type) {
  if (strcmp(type->name, "int") == 0) {
    return LLVMInt32Type();
  } else if (strcmp(type->name, "char*") == 0) {
    return LLVMPointerType(LLVMInt8Type(), 0);
  } else {
    return NULL;
  }
}

int compile_fn (module_t* mod, symbol_t* sym) {
  type_t type = sym->type;
  int i;
  LLVMTypeRef* param_types = malloc((type.num_fields - 1) * sizeof(LLVMTypeRef));
  for (i = 0; i < type.num_fields - 1; i++) {
    param_types[i] = type_to_llvm(&type.fields[i]);
  }
  LLVMTypeRef ret_type = type_to_llvm(&type.fields[i]);
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
    if (eval_statement(builder, mod, table, body, &result) != 0) {
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
      fprintf(stderr, "error compile funtion %s\n", sym->name);
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
