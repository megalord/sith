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
#include "parser.h"
#include "utils.h"

typedef void* (*fn_zero_arg_t) ();
typedef void* (*fn_one_arg_t) (void*);
typedef void* (*fn_two_arg_t) (void*, void*);
typedef void* (*fn_three_arg_t) (void*, void*, void*);

int compile_const_val (val_t* val, LLVMBuilderRef builder, LLVMValueRef* result) {
  if (val->type->meta != TYPE_PRIM) {
    fprintf(stderr, "cannot compile non-primitive constants\n");
    return 1;
  }

  if (strcmp(val->type->name, "I32") == 0) {
    *result = LLVMConstInt(LLVMInt32Type(), *(int*)val->data, 0);
  } else if (strcmp(val->type->name, "Str") == 0) {
    *result = LLVMBuildGlobalStringPtr(builder, (char*)val->data, "str");
  } else {
    fprintf(stderr, "cannot compile constant of type %s\n", val->type->name);
    return 1;
  }
  return 0;
}

int compile_if (module_t* mod, symbol_table_t* table, expr_t *expr, LLVMBuilderRef builder, LLVMValueRef* result) {
  LLVMValueRef condition, results[2];
  LLVMValueRef fn = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));
  LLVMBasicBlockRef bbs[2] = {
    LLVMAppendBasicBlock(fn, "then"),
    LLVMAppendBasicBlock(fn, "else")
  };
  LLVMBasicBlockRef end_bb = LLVMAppendBasicBlock(fn, "if_cont");

  if (compile_expr(mod, table, expr->if_cond, builder, &condition) != 0) {
    return 1;
  }
  LLVMBuildCondBr(builder, condition, bbs[0], bbs[1]);

  LLVMPositionBuilderAtEnd(builder, bbs[0]);
  if (compile_expr(mod, table, expr->if_, builder, results) != 0) {
    return 1;
  }
  LLVMBuildBr(builder, end_bb);
  bbs[0] = LLVMGetInsertBlock(builder);

  LLVMPositionBuilderAtEnd(builder, bbs[1]);
  if (compile_expr(mod, table, expr->else_, builder, results + 1) != 0) {
    return 1;
  }
  LLVMBuildBr(builder, end_bb);
  bbs[1] = LLVMGetInsertBlock(builder);

  LLVMPositionBuilderAtEnd(builder, end_bb);
  *result = LLVMBuildPhi(builder, LLVMTypeOf(results[0]), "iftmp");
  LLVMAddIncoming(*result, results, bbs, 2);
  return 0;
}

int compile_switch (module_t* mod, symbol_table_t* table, expr_t* expr, LLVMBuilderRef builder, LLVMValueRef* result) {
  LLVMValueRef condition;
  LLVMValueRef fn = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));

  if (compile_expr(mod, table, expr->case_cond, builder, &condition) != 0) {
    return 1;
  }
  LLVMBasicBlockRef else_bb = LLVMAppendBasicBlock(fn, "else");
  LLVMBasicBlockRef end_bb = LLVMAppendBasicBlock(fn, "switch_cont");
  LLVMValueRef switch_val = LLVMBuildSwitch(builder, condition, else_bb, expr->num_cases);

  LLVMBasicBlockRef* bbs = malloc(sizeof(LLVMBasicBlockRef) * expr->num_cases);
  LLVMValueRef* results = malloc(sizeof(LLVMValueRef) * expr->num_cases);
  LLVMValueRef on_val;
  char case_name[8];
  for (int i = 0; i < expr->num_cases; i++) {
    snprintf(case_name, 8, "case_%01d", i);
    bbs[i] = LLVMAppendBasicBlock(fn, case_name);

    for (int j = 0; j < expr->case_vals[i].len; j++) {
      if (compile_const_val(expr->case_vals[i].vals + j, builder, &on_val) != 0) {
        return 1;
      }
      LLVMAddCase(switch_val, on_val, bbs[i]);
    }

    LLVMPositionBuilderAtEnd(builder, bbs[i]);
    if (compile_expr(mod, table, expr->case_bodies + i, builder, results + i) != 0) {
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
  LLVMAddIncoming(*result, results, bbs, expr->num_cases);

  free(bbs);
  return 0;
}

int compile_funcall (module_t* mod, symbol_table_t* table, expr_t* expr, LLVMBuilderRef builder, LLVMValueRef* result) {
  LLVMValueRef* args = malloc(expr->num_params * sizeof(LLVMValueRef));
  for (int i = 0; i < expr->num_params; i++) {
    if (compile_expr(mod, table, expr->params + i, builder, args + i) != 0) {
      return 1;
    }
  }

  if (strcmp(expr->fn_name, "eq") == 0) {
    if (expr->num_params != 2) {
      fprintf(stderr, "eq called with %d args, expected 2\n", expr->num_params);
      return 1;
    }
    *result = LLVMBuildICmp(builder, LLVMIntEQ, args[0], args[1], "eq");
    return 0;
  }

  val_t* val = symbol_table_get(table, expr->fn_name);
  if (val == NULL) {
    val = module_deps_symbol_find(mod, expr->fn_name);
    if (val != NULL) {
      val = symbol_table_add(&mod->table, expr->fn_name, val);
      val->llvm = LLVMAddFunction(mod->llvm, expr->fn_name, LLVMGetElementType(LLVMTypeOf(val->llvm)));
    }
  }
  if (val == NULL) {
    fprintf(stderr, "symbol \"%s\" not found\n", expr->fn_name);
    return 1;
  }
  if (val->type->meta != TYPE_FUNC) {
    fprintf(stderr, "symbol \"%s\" is not a function\n", expr->fn_name);
    return 1;
  }

  *result = LLVMBuildCall(builder, val->llvm, args, expr->num_params, expr->fn_name);
  return 0;
}

int compile_expr (module_t* mod, symbol_table_t* table, expr_t* expr, LLVMBuilderRef builder, LLVMValueRef* result) {
  val_t* val;
  switch (expr->type) {
    case EXPR_CONST:
      return compile_const_val(expr->cnst, builder, result);
    case EXPR_FUNCALL:
      return compile_funcall(mod, table, expr, builder, result);
    case EXPR_IF:
      return compile_if(mod, table, expr, builder, result);
    case EXPR_PROGN:
      for (int i = 0; i < expr->num_exprs; i++) {
        if (compile_expr(mod, table, expr->exprs + i, builder, result) != 0) {
          return 1;
        }
      }
      return 0;
    case EXPR_SWITCH:
      return compile_switch(mod, table, expr, builder, result);
    case EXPR_VAR:
      val = symbol_table_get(table, expr->var_name);
      if (val == NULL) {
        fprintf(stderr, "symbol \"%s\" not found\n", expr->var_name);
        return 1;
      }
      *result = val->llvm;
      return 0;
  }
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
    return LLVMPointerType(type_to_llvm(type->fields), 0);
  } else {
    fprintf(stderr, "cannot convert type %s\n", type->name);
    return NULL;
  }
}

int compile_fn (module_t* mod, val_t* val, char* name) {
  type_t type = *val->type;
  int i;
  LLVMTypeRef* param_types = malloc((type.num_fields - 1) * sizeof(LLVMTypeRef));
  for (i = 0; i < type.num_fields - 1; i++) {
    param_types[i] = type_to_llvm(type.fields + i);
    if (param_types[i] == NULL) {
      return 1;
    }
  }
  LLVMTypeRef ret_type = type_to_llvm(type.fields + i);
  if (ret_type == NULL) {
    return 1;
  }
  LLVMTypeRef fn_type = LLVMFunctionType(ret_type, param_types, type.num_fields - 1, 0);
  val->llvm = LLVMAddFunction(mod->llvm, name, fn_type);

  // ffi declaration
  if (val->body == NULL) {
    return 0;
  }

  symbol_table_t* table = malloc(sizeof(symbol_table_t));
  table->num_symbols = type.num_fields - 1;
  table->max_symbols = table->num_symbols;
  table->parent = &mod->table;

  table->names = malloc(table->num_symbols * sizeof(char*));
  table->values = malloc(table->num_symbols * sizeof(val_t));
  for (i = 0; i < type.num_fields - 1; i++) {
    table->names[i] = type.field_names[i];
    table->values[i].type = type.fields + i;
    table->values[i].llvm = LLVMGetParam(val->llvm, i);
  }

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(val->llvm, "entry");

  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder, entry);

  LLVMValueRef result;

  if (compile_expr(mod, table, val->body, builder, &result) != 0) {
    return 1;
  }

  LLVMBuildRet(builder, result);
  return 0;
}

int module_compile (module_t* mod) {
  for (int i = 0; i < mod->num_deps; i++) {
    if (module_compile(mod->deps[i]) != 0) {
      return 1;
    }
  }
  mod->llvm = LLVMModuleCreateWithName(mod->name);
  val_t* val;
  int len = mod->table.num_symbols;
  for (int i = 0; i < len; i++) {
    val = mod->table.values + i;
    if (val->type->meta != TYPE_FUNC) {
      fprintf(stderr, "error compile top level expressions %s\n", mod->table.names[i]);
      return 1;
    }
    if (compile_fn(mod, val, mod->table.names[i]) != 0) {
      fprintf(stderr, "error compile function %s\n", mod->table.names[i]);
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

  val_t* main = symbol_table_get(&mod->table, (char*)"main");
  if (main != NULL) {
    int (*fn)(void) = LLVMGetPointerToGlobal(engine, main->llvm);
    return (*fn)();
  } else {
    fprintf(stderr, "no main function\n");
    return 1;
  }
}
