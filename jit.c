#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

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

// declare i32 @puts(i8* nocapture) nounwind
LLVMValueRef def_puts (LLVMModuleRef ll_mod) {
  LLVMTypeRef param_types[] = { LLVMPointerType(LLVMInt8Type(), 0) };
  LLVMTypeRef fn_type = LLVMFunctionType(LLVMInt32Type(), param_types, 1, 0);
  LLVMValueRef fn = LLVMAddFunction(ll_mod, "puts", fn_type);
  LLVMAddAttributeAtIndex(fn, 1, nocapture_attr);
  LLVMAddAttributeAtIndex(fn, LLVMAttributeFunctionIndex, nounwind_attr);
  return fn;
}

int eval_statement(LLVMBuilderRef builder, symbol_table_t* table, node_t* node, LLVMValueRef* result) {
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
        eval_result = eval_statement(builder, table, node, &args[i]);
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

int compile_fn (LLVMModuleRef ll_mod, module_t* mod, symbol_t* sym) {
  type_t type = sym->type;
  int i;
  LLVMTypeRef* param_types = malloc((type.num_fields - 1) * sizeof(LLVMTypeRef));
  for (i = 0; i < type.num_fields - 1; i++) {
    param_types[i] = type_to_llvm(&type.fields[i]);
  }
  LLVMTypeRef ret_type = type_to_llvm(&type.fields[i]);
  LLVMTypeRef fn_type = LLVMFunctionType(ret_type, param_types, type.num_fields - 1, 0);
  sym->value = LLVMAddFunction(ll_mod, sym->name, fn_type);

  symbol_table_t* table = malloc(sizeof(symbol_table_t));
  table->num_symbols = type.num_fields - 1;
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
    if (eval_statement(builder, table, body, &result) != 0) {
      return 1;
    }
    body = body->next;
  }
  LLVMBuildRet(builder, result);
  return 0;
}

int module_compile (LLVMModuleRef ll_mod, module_t* mod) {
  symbol_t* sym;
  for (int i = 0; i < mod->table.num_symbols; i++) {
    sym = &mod->table.symbols[i];
    if (compile_fn(ll_mod, mod, sym) != 0) {
      fprintf(stderr, "error compile funtion %s\n", sym->name);
      return 1;
    }
  }
  return 0;
}

int exec_main (module_t* mod) {
  LLVMLinkInMCJIT();
  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  LLVMModuleRef ll_mod = LLVMModuleCreateWithName(mod->name);

  unsigned nounwind_attr_id = LLVMGetEnumAttributeKindForName("nounwind", sizeof("nounwind") - 1);
  nounwind_attr = LLVMCreateEnumAttribute(LLVMGetGlobalContext(), nounwind_attr_id, 0);

  unsigned nocapture_attr_id = LLVMGetEnumAttributeKindForName("nocapture", sizeof("nocapture") - 1);
  nocapture_attr = LLVMCreateEnumAttribute(LLVMGetGlobalContext(), nocapture_attr_id, 0);

  //LLVMModuleRef ll_std_mod = LLVMModuleCreateWithName("std");
  symbol_table_t* deps_table = malloc(sizeof(symbol_table_t));
  deps_table->parent = NULL;
  deps_table->num_symbols = 1;
  deps_table->symbols = malloc(sizeof(symbol_t));
  deps_table->symbols[0].name = malloc(5);
  strncpy(deps_table->symbols[0].name, "puts", 5);
  deps_table->symbols[0].data = NULL;
  deps_table->symbols[0].type = (type_t) { .meta = TYPE_FUNC, .num_fields = 0, .fields = NULL };
  deps_table->symbols[0].type.name = malloc(6);
  strncpy(deps_table->symbols[0].type.name, "char*", 6);
  deps_table->symbols[0].value = def_puts(ll_mod);
  mod->table.parent = deps_table;

  //Assertion failed: (isa<X>(Val) && "cast<Ty>() argument of incompatible type!"), function cast, file /Users/jord7580/dev/llvm/include/llvm/Support/Casting.h, line 255.
  //for (int i = 0; i < deps_table.num_symbols; i++) {
  //  LLVMDumpValue(deps_table.symbols[0].value);
  //  LLVMDumpType(LLVMTypeOf(deps_table.symbols[0].value));
  //  LLVMAddFunction(ll_mod, deps_table.symbols[0].name, LLVMTypeOf(deps_table.symbols[0].value));
  //}

  if (module_compile(ll_mod, mod) != 0) {
    return 1;
  }

  char* error = NULL;
  LLVMVerifyModule(ll_mod, LLVMAbortProcessAction, &error);
  LLVMDisposeMessage(error);

  puts("LLVMCreateExecutionEngineForModule");
  error = NULL;
  LLVMExecutionEngineRef engine;
  if (LLVMCreateExecutionEngineForModule(&engine, ll_mod, &error) != 0) {
    fprintf(stderr, "failed to create execution engine\n");
    abort();
  }
  if (error != NULL) {
    fprintf(stderr, "error: %s\n", error);
    LLVMDisposeMessage(error);
    exit(EXIT_FAILURE);
  }
  LLVMDumpModule(ll_mod);

  symbol_t* main = symbol_table_get(&mod->table, "main");
  if (main != NULL) {
    fn_zero_arg_t fn = (fn_zero_arg_t)LLVMGetPointerToGlobal(engine, main->value);
    return (int)fn();
  } else {
    fprintf(stderr, "no main function\n");
    return 1;
  }
}
