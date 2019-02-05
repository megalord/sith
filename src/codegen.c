#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/IRReader.h>

#include "codegen.h"
#include "debug.h"
#include "parser.h"
#include "types.h"
#include "utils.h"

typedef void* (*fn_zero_arg_t) ();
typedef void* (*fn_one_arg_t) (void*);
typedef void* (*fn_two_arg_t) (void*, void*);
typedef void* (*fn_three_arg_t) (void*, void*, void*);


char type_name[80];
char fn_name_buffer[80];

int type_instance_name_iter (char* str, int rem, type_t* type, type_t* template, int depth) {
  int i = 0;
  for (; i < (int)strlen(type->name); i++) {
    assert(i < rem);
    str[i] = type->name[i];
  }
  if (depth > 0) {
    return i;
  }
  assert(i < rem);
  for (int j = 0; j < type->num_fields; j++) {
    if (template != NULL && template->fields[j]->meta != TYPE_HOLE) {
      continue;
    }
    if (type->fields[j] != NULL) {
      str[i++] = '_';
      i += type_instance_name_iter(str + i, rem - i, type->fields[j], NULL, 1);
    }
  }
  return i;
}

char* type_instance_name (type_t* type, type_t* template) {
  int i = type_instance_name_iter(type_name, 80, type, template, 0);
  type_name[i] = '\0';
  return type_name;
}

char* val_name (char* mod, char* name, type_t* type) {
  int rem = 80;
  int i = 0;
  for (; i < (int)strlen(mod); i++) {
    fn_name_buffer[i] = mod[i];
    assert(i < rem);
  }

  if (name != NULL) {
    // Product type constructors will use the type's name below.
    fn_name_buffer[i++] = '_';
    assert(i < rem);
    for (int j = 0; j < (int)strlen(name); j++) {
      fn_name_buffer[i] = name[j];
      i++;
      assert(i < rem);
    }
  }

  if (type->meta != TYPE_FUNC) {
    // A function's type name is meaningless, but everything else is important
    fn_name_buffer[i++] = '_';
    assert(i < rem);
    for (int j = 0; j < (int)strlen(type->name); j++) {
      fn_name_buffer[i] = type->name[j];
      i++;
      assert(i < rem);
    }
  }

  for (int j = 0; j < type->num_fields - 1; j++) {
    fn_name_buffer[i++] = '_';
    char* field_name = (type->fields[j] == NULL) ? type->field_names[j] : type->fields[j]->name;
    for (int k = 0; k < (int)strlen(field_name); k++) {
      fn_name_buffer[i++] = field_name[k];
      assert(i < rem);
    }
  }
  fn_name_buffer[i] = '\0';
  return fn_name_buffer;
}

char* fn_name_expr (expr_t* expr) {
  // repeat the same logic as val_name(), but iterating through funcall expression args
  int rem = 80;
  int i = 0;
  for (; i < (int)strlen(expr->fn_mod); i++) {
    fn_name_buffer[i] = expr->fn_mod[i];
    assert(i < rem);
  }

  fn_name_buffer[i++] = '_';
  assert(i < rem);
  for (int j = 0; j < (int)strlen(expr->fn_name); j++) {
    fn_name_buffer[i] = expr->fn_name[j];
    i++;
    assert(i < rem);
  }

  for (int j = 0; j < expr->num_params; j++) {
    fn_name_buffer[i++] = '_';
    for (int k = 0; k < (int)strlen(expr->params[j].type->name); k++) {
      fn_name_buffer[i++] = expr->params[j].type->name[k];
      assert(i < rem);
    }
  }
  fn_name_buffer[i] = '\0';
  return fn_name_buffer;
}

int compile_type_product (type_t* type) {
  type->llvm = LLVMStructCreateNamed(LLVMGetGlobalContext(), type_instance_name(type, NULL));
  LLVMTypeRef* types = malloc(type->num_fields * sizeof(LLVMTypeRef));
  for (int i = 0; i < type->num_fields; i++) {
    if (type->fields[i]->llvm == NULL && compile_type(type->fields[i]) != 0) {
      return 1;
    }
    types[i] = type->fields[i]->llvm;
  }
  LLVMStructSetBody(type->llvm, types, type->num_fields, 0);
  return 0;
}

int compile_type_sum (type_t* type) {
  int has_data = 0;
  for (int i = 0; i < type->num_fields; i++) {
    if (type->fields[0] != NULL) {
      has_data = 1;
      break;
    }
  }

  if (!has_data) {
    type->llvm = (type->num_fields <= 2) ? LLVMInt1Type() : LLVMInt8Type();
    return 0;
  }

  type->llvm = LLVMStructCreateNamed(LLVMGetGlobalContext(), type_instance_name(type, NULL));
  LLVMTypeRef* types = malloc((type->num_fields + 1) * sizeof(LLVMTypeRef));
  types[0] = (type->num_fields <= 2) ? LLVMInt1Type() : LLVMInt8Type();
  for (int i = 0; i < type->num_fields; i++) {
    if (type->fields[i] == NULL) {
      types[i+1] = LLVMInt1Type();
    } else {
      if (type->fields[i]->llvm == NULL && compile_type(type->fields[i]) != 0) {
        return 1;
      }
      types[i+1] = type->fields[i]->llvm;
    }
  }
  LLVMStructSetBody(type->llvm, types, type->num_fields + 1, 0);
  return 0;
}

int compile_type_fn (type_t* type) {
  int i;
  LLVMTypeRef* param_types = malloc((type->num_fields - 1) * sizeof(LLVMTypeRef));
  for (i = 0; i < type->num_fields - 1; i++) {
    param_types[i] = type->fields[i]->llvm;
    if (param_types[i] == NULL) {
      fprintf(stderr, "param %d type is null\n", i);
      return 1;
    }
  }
  LLVMTypeRef ret_type = type->fields[i]->llvm;
  if (ret_type == NULL) {
    fprintf(stderr, "return type is null\n");
    return 1;
  }
  type->llvm = LLVMFunctionType(ret_type, param_types, type->num_fields - 1, 0);
  return 0;
}

int compile_type (type_t* type) {
  if (type->llvm != NULL) {
    return 0;
  }

  switch (type->meta) {
    case TYPE_OPAQUE:
      type->llvm = LLVMStructCreateNamed(LLVMGetGlobalContext(), type->name);
      return 0;
    case TYPE_PARAM:
      if (strcmp(type->name, "Ptr") == 0) {
        if (type->num_fields != 1) {
          fprintf(stderr, "Ptr type must have 1 field, got %d\n", type->num_fields);
          return 1;
        }
        // TODO: This is needed because the builtin module can have type instances for Ptr
        // for types that are defined elsewhere, but unlike other modules, builtin cannot have dependencies.
        if (type->fields[0]->llvm == NULL && compile_type(type->fields[0]) != 0) {
          fprintf(stderr, "Ptr type compile field failed\n");
          return 1;
        }
        type->llvm = LLVMPointerType(type->fields[0]->llvm, 0);
        return 0;
      } else {
        fprintf(stderr, "cannot compile type %s\n", type->name);
        return 1;
      }
    case TYPE_PRIM:
      if (strcmp(type->name, "I8") == 0) {
        type->llvm = LLVMInt8Type();
        return 0;
      } else if (strcmp(type->name, "I32") == 0) {
        type->llvm = LLVMInt32Type();
        return 0;
      } else {
        fprintf(stderr, "cannot compile type %s\n", type->name);
        return 1;
      }
    case TYPE_PRODUCT:
      //if (strcmp(type->name, "List") == 0) {
      //  return compile_type_list(type);
      //} else {
      return compile_type_product(type);
    case TYPE_SUM:
      return compile_type_sum(type);
    case TYPE_FUNC:
      return compile_type_fn(type);
    default:
      fprintf(stderr, "cannot compile type %s\n", type->name);
      return 1;
  }
}

int compile_const_val (context_t* ctx, val_t* val, LLVMValueRef* result) {
  if (val->type->meta == TYPE_PRIM && strcmp(val->type->name, "I32") == 0) {
    *result = LLVMConstInt(LLVMInt32Type(), *(int*)val->data, 0);
  } else if (val->type->meta == TYPE_PARAM &&
             strcmp(val->type->name, "Ptr") == 0 &&
             val->type->num_fields == 1 &&
             strcmp(val->type->fields[0]->name, "I8") == 0) {
    int len = strlen((char*)val->data);
    LLVMValueRef int_list = LLVMBuildArrayAlloca(ctx->builder,
        LLVMArrayType(LLVMInt8Type(), len + 1),
        LLVMConstInt(LLVMInt8Type(), 1, false),
        "");
    LLVMBuildStore(ctx->builder, LLVMConstString((char*)val->data, len, false), int_list);
    *result = LLVMBuildBitCast(ctx->builder, int_list, LLVMPointerType(LLVMInt8Type(), 0), "");
  } else {
    fprintf(stderr, "cannot compile constant of type ");
    type_print(val->type);
    return 1;
  }
  return 0;
}

int compile_if (context_t* ctx, symbol_table_t* table, expr_t *expr, LLVMValueRef* result) {
  LLVMValueRef condition, results[2];
  LLVMValueRef fn = LLVMGetBasicBlockParent(LLVMGetInsertBlock(ctx->builder));
  LLVMBasicBlockRef bbs[2] = {
    LLVMAppendBasicBlock(fn, "then"),
    LLVMAppendBasicBlock(fn, "else")
  };
  LLVMBasicBlockRef end_bb = LLVMAppendBasicBlock(fn, "if_cont");

  if (compile_expr(ctx, table, expr->if_cond, &condition) != 0) {
    return 1;
  }
  if (LLVMGetTypeKind(LLVMTypeOf(condition)) == LLVMPointerTypeKind) {
    condition = LLVMBuildLoad(ctx->builder, condition, "load_if_cond");
  }
  LLVMBuildCondBr(ctx->builder, condition, bbs[0], bbs[1]);

  LLVMPositionBuilderAtEnd(ctx->builder, bbs[0]);
  if (compile_expr(ctx, table, expr->if_, results) != 0) {
    return 1;
  }
  LLVMBuildBr(ctx->builder, end_bb);
  bbs[0] = LLVMGetInsertBlock(ctx->builder);

  LLVMPositionBuilderAtEnd(ctx->builder, bbs[1]);
  if (compile_expr(ctx, table, expr->else_, results + 1) != 0) {
    return 1;
  }
  LLVMBuildBr(ctx->builder, end_bb);
  bbs[1] = LLVMGetInsertBlock(ctx->builder);

  LLVMPositionBuilderAtEnd(ctx->builder, end_bb);
  *result = LLVMBuildPhi(ctx->builder, LLVMTypeOf(results[0]), "iftmp");
  LLVMAddIncoming(*result, results, bbs, 2);
  return 0;
}

int compile_let (context_t* ctx, symbol_table_t* table, expr_t* expr, LLVMValueRef* result) {
  //LLVMValueRef LLVMBuildAlloca(LLVMBuilderRef, LLVMTypeRef Ty, const char *Name);
  //LLVMValueRef LLVMBuildArrayAlloca(LLVMBuilderRef, LLVMTypeRef Ty,
  //                                  LLVMValueRef Val, const char *Name);
  //LLVMValueRef LLVMBuildStore(LLVMBuilderRef, LLVMValueRef Val, LLVMValueRef Ptr);

  //val_t* val;
  ////LLVMValueRef* ptrs = malloc(sizeof(LLVMValueRef) * expr->let_table->num_symbols);
  //LLVMValueRef val_result;
  //for (int i = 0; i < expr->let_table->num_symbols; i++) {
  //  val = expr->let_table->values + i;
  //  val->llvm = LLVMBuildAlloca(builder, type_to_llvm(val->type), expr->let_table->names[i]);
  //  if (compile_expr(ctx, table, val->body, builder, &val_result) != 0) {
  //    return 1;
  //  }
  //  LLVMBuildStore(builder, val_result, val->llvm);
  //}
  for (val_t* val = expr->let_table->values; val < expr->let_table->values + expr->let_table->num_symbols; val++) {
    if (compile_expr(ctx, table, val->body, &val->llvm) != 0) {
      return 1;
    }
  }
  return compile_expr(ctx, expr->let_table, expr->let_body, result);
}

int compile_match (context_t* ctx, symbol_table_t* table, expr_t *expr, LLVMValueRef* result) {
  LLVMTypeRef condition_ty;
  LLVMValueRef condition, on_val;
  LLVMValueRef fn = LLVMGetBasicBlockParent(LLVMGetInsertBlock(ctx->builder));

  if (compile_expr(ctx, table, expr->match_cond, &condition) != 0) {
    return 1;
  }
  condition_ty = LLVMTypeOf(condition);
  if (LLVMGetTypeKind(condition_ty) == LLVMPointerTypeKind) {
    condition = LLVMBuildLoad(ctx->builder, condition, "load_match_cond");
  }
  condition_ty = LLVMTypeOf(condition);
  LLVMValueRef data = condition;
  if (LLVMGetTypeKind(condition_ty) == LLVMStructTypeKind) {
    condition = LLVMBuildExtractValue(ctx->builder, data, 0, "sum_idx");
  } else if (LLVMGetTypeKind(condition_ty) != LLVMIntegerTypeKind) {
    fprintf(stderr, "unexpected type kind for match condition\n");
    return 1;
  }
  condition_ty = LLVMTypeOf(condition);

  LLVMBasicBlockRef else_bb = LLVMAppendBasicBlock(fn, "else");
  LLVMBasicBlockRef end_bb = LLVMAppendBasicBlock(fn, "match_cont");
  LLVMValueRef match_val = LLVMBuildSwitch(ctx->builder, condition, else_bb, expr->num_pats);

  LLVMBasicBlockRef* bbs = malloc(sizeof(LLVMBasicBlockRef) * expr->num_pats);
  LLVMValueRef* results = malloc(sizeof(LLVMValueRef) * expr->num_pats);
  char pat_name[7];
  for (int i = 0; i < expr->num_pats; i++) {
    snprintf(pat_name, 7, "pat_%01d", i);
    bbs[i] = LLVMAppendBasicBlock(fn, pat_name);
    char* name;
    switch (expr->match_pats[i].form) {
      case EXPR_VAR:
        name = expr->match_pats[i].var_name;
        break;
      case EXPR_FUNCALL:
        name = expr->match_pats[i].fn_name;
        break;
      default:
        fprintf(stderr, "invalid expr type for match: %d\n", expr->match_pats[i].form);
        return 1;
    }
    int index = type_sum_index(expr->match_cond->type, name);
    on_val = LLVMConstInt(condition_ty, index, false);
    LLVMAddCase(match_val, on_val, bbs[i]);

    LLVMPositionBuilderAtEnd(ctx->builder, bbs[i]);
    switch (expr->match_pats[i].form) {
      case EXPR_VAR:
        if (compile_expr(ctx, table, expr->match_bodies + i, results + i) != 0) {
          return 1;
        }
        break;
      case EXPR_FUNCALL:
        if (expr->match_pats[i].num_params > 1) {
          fprintf(stderr, "only one param allowed for fn match\n");
          return 1;
        }
        assert(expr->match_bodies[i].form == EXPR_LET);
        // assumes sum type field has only one value
        expr->match_bodies[i].let_table->values[0].llvm = LLVMBuildExtractValue(ctx->builder, data, index + 1, "");
        expr->match_bodies[i].let_table->values[0].type = expr->match_cond->type->fields[index];
        if (compile_expr(ctx, expr->match_bodies[i].let_table, expr->match_bodies[i].let_body, results + i) != 0) {
          return 1;
        }
        break;
      default:
        fprintf(stderr, "invalid expr type for match: %d\n", expr->match_pats[i].form);
        return 1;
    }
    LLVMBuildBr(ctx->builder, end_bb);
    bbs[i] = LLVMGetInsertBlock(ctx->builder);
  }

  LLVMPositionBuilderAtEnd(ctx->builder, else_bb);
  LLVMBuildUnreachable(ctx->builder);

  LLVMPositionBuilderAtEnd(ctx->builder, end_bb);
  *result = LLVMBuildPhi(ctx->builder, LLVMTypeOf(results[0]), "matchtmp");
  LLVMAddIncoming(*result, results, bbs, expr->num_pats);

  free(bbs);
  free(results);
  return 0;
}

int compile_switch (context_t* ctx, symbol_table_t* table, expr_t* expr, LLVMValueRef* result) {
  LLVMValueRef condition;
  LLVMValueRef fn = LLVMGetBasicBlockParent(LLVMGetInsertBlock(ctx->builder));

  if (compile_expr(ctx, table, expr->case_cond, &condition) != 0) {
    return 1;
  }
  LLVMBasicBlockRef else_bb = LLVMAppendBasicBlock(fn, "else");
  LLVMBasicBlockRef end_bb = LLVMAppendBasicBlock(fn, "switch_cont");
  LLVMValueRef switch_val = LLVMBuildSwitch(ctx->builder, condition, else_bb, expr->num_cases);

  LLVMBasicBlockRef* bbs = malloc(sizeof(LLVMBasicBlockRef) * expr->num_cases);
  LLVMValueRef* results = malloc(sizeof(LLVMValueRef) * expr->num_cases);
  LLVMValueRef on_val;
  char case_name[8];
  for (int i = 0; i < expr->num_cases; i++) {
    snprintf(case_name, 8, "case_%01d", i);
    bbs[i] = LLVMAppendBasicBlock(fn, case_name);

    for (int j = 0; j < expr->case_vals[i].len; j++) {
      if (compile_const_val(ctx, expr->case_vals[i].vals + j, &on_val) != 0) {
        return 1;
      }
      LLVMAddCase(switch_val, on_val, bbs[i]);
    }

    LLVMPositionBuilderAtEnd(ctx->builder, bbs[i]);
    if (compile_expr(ctx, table, expr->case_bodies + i, results + i) != 0) {
      return 1;
    }
    LLVMBuildBr(ctx->builder, end_bb);
    bbs[i] = LLVMGetInsertBlock(ctx->builder);
  }

  // TODO: handle default
  LLVMPositionBuilderAtEnd(ctx->builder, else_bb);
  LLVMBuildUnreachable(ctx->builder);

  LLVMPositionBuilderAtEnd(ctx->builder, end_bb);
  *result = LLVMBuildPhi(ctx->builder, LLVMTypeOf(results[0]), "switchtmp");
  LLVMAddIncoming(*result, results, bbs, expr->num_cases);

  free(bbs);
  free(results);
  return 0;
}

int compile_setf (context_t* ctx, symbol_table_t* table, expr_t* expr, LLVMValueRef* result) {
  fprintf(stderr, "compile_setf not implemented\n");
  return 1;
}

int compile_funcall (context_t* ctx, symbol_table_t* table, expr_t* expr, LLVMValueRef* result) {
  LLVMValueRef* args = malloc(expr->num_params * sizeof(LLVMValueRef));
  for (int i = 0; i < expr->num_params; i++) {
    if (compile_expr(ctx, table, expr->params + i, args + i) != 0) {
      return 1;
    }
  }

  if (strcmp(expr->fn_name, "eq") == 0) {
    if (expr->num_params != 2) {
      fprintf(stderr, "eq called with %d args, expected 2\n", expr->num_params);
      return 1;
    }
    *result = LLVMBuildICmp(ctx->builder, LLVMIntEQ, args[0], args[1], "eq");
    return 0;
  } else if (strcmp(expr->fn_name, "setf") == 0) {
    if (compile_setf(ctx, table, expr, result) != 0) {
      fprintf(stderr, "error compile_funcall\n");
      return 1;
    }
    return 0;
  }

  LLVMValueRef fn = expr->fn->llvm;
  if (fn == NULL) {
    fn = LLVMGetNamedFunction(ctx->mod_llvm, expr->fn_name); // only works for imported c functions
  }
  if (fn == NULL) {
    // should these two be the same?
    //fn = LLVMGetNamedFunction(ctx->mod_llvm, val_name(ctx->mod_src->name, expr->fn_name, expr->type));
    fn = LLVMGetNamedFunction(ctx->mod_llvm, fn_name_expr(expr));
  }
  if (fn == NULL) {
    LLVMDumpModule(ctx->mod_llvm);
    fprintf(stderr, "function %s not found\n", fn_name_expr(expr));
    return 1;
  }
  *result = LLVMBuildCall(ctx->builder, fn, args, expr->num_params, expr->fn_name);
  return 0;
}

int compile_expr (context_t* ctx, symbol_table_t* table, expr_t* expr, LLVMValueRef* result) {
  switch (expr->form) {
    case EXPR_CONST:
      return compile_const_val(ctx, expr->cnst, result);
    case EXPR_FUNCALL:
      return compile_funcall(ctx, table, expr, result);
    case EXPR_IF:
      return compile_if(ctx, table, expr, result);
    case EXPR_LET:
      return compile_let(ctx, table, expr, result);
    case EXPR_MATCH:
      return compile_match(ctx, table, expr, result);
    case EXPR_PROGN:
      for (int i = 0; i < expr->num_exprs; i++) {
        if (compile_expr(ctx, table, expr->exprs + i, result) != 0) {
          return 1;
        }
      }
      return 0;
    case EXPR_SWITCH:
      return compile_switch(ctx, table, expr, result);
    case EXPR_VAR:
      if (expr->var->llvm == NULL) {
        *result = LLVMGetNamedGlobal(ctx->mod_llvm, val_name(expr->var_mod, expr->var_name, expr->var->type));
        // variables set to global values will not have their own global
        if (*result == NULL) {
          fprintf(stderr, "global value %s not found\n", expr->var_name);
          return 1;
        }
      } else {
        *result = expr->var->llvm;
      }
      return 0;
  }
}

int compile_fn (context_t* ctx, val_t* val, char* name) {
  // ffi declaration
  if (val->body == NULL) {
    return 0;
  }

  for (int i = 0; i < val->type->num_fields - 1; i++) {
    val->body->let_table->values[i].llvm = LLVMGetParam(val->llvm, i);
  }

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(val->llvm, "entry");

  ctx->builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(ctx->builder, entry);

  LLVMValueRef result;

  if (compile_expr(ctx, val->body->let_table, val->body->let_body, &result) != 0) {
    return 1;
  }

  LLVMBuildRet(ctx->builder, result);
  ctx->builder = NULL;
  return 0;
}

int compile_type_sum_constructor (context_t* ctx, type_t* type, type_t* field, int index) {
  if (field == NULL) {
    LLVMValueRef var = LLVMAddGlobal(ctx->mod_llvm, type->llvm, val_name(ctx->mod_src->name, type->field_names[index], type));
    if (LLVMGetTypeKind(type->llvm) == LLVMIntegerTypeKind) {
      LLVMSetInitializer(var, LLVMConstInt(type->llvm, index, false));
    } else {
      LLVMValueRef* vals = malloc((type->num_fields + 1) * sizeof(LLVMValueRef));
      vals[0] = LLVMConstInt(LLVMStructGetTypeAtIndex(type->llvm, 0), index, false);
      for (int i = 0; i < type->num_fields; i++) {
        vals[i + 1] = (type->fields[i] == NULL) ? LLVMConstInt(LLVMInt1Type(), 0, false) : LLVMConstNull(type->fields[i]->llvm);
      }
      LLVMSetInitializer(var, LLVMConstNamedStruct(type->llvm, vals, type->num_fields + 1));
    }
    return 0;
  }

  type_t* fn_type = type_sum_constructor(field, type);
  if (compile_type_fn(fn_type) != 0) {
    fprintf(stderr, "error compiling type sum constructor for %s; fault of type_sum_constructor()\n", type->name);
    type_print(fn_type);
    fprintf(stderr, "\n");
    return 1;
  }
  LLVMValueRef fn = LLVMAddFunction(ctx->mod_llvm, val_name(ctx->mod_src->name, type->field_names[index], fn_type), fn_type->llvm);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(fn, "entry");
  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder, entry);

  LLVMValueRef gep_indices[2], ptr;
  LLVMValueRef result = LLVMBuildAlloca(builder, type->llvm, type->name);

  // The first operand indexes through the pointer
  gep_indices[0] = LLVMConstInt(LLVMInt32Type(), 0, false);

  gep_indices[1] = LLVMConstInt(LLVMInt32Type(), 0, false);
  ptr = LLVMBuildGEP(builder, result, gep_indices, 2, "sum");
  LLVMBuildStore(builder, LLVMConstInt(LLVMStructGetTypeAtIndex(type->llvm, 0), index, false), ptr);

  for (int i = 0; i < type->num_fields; i++) {
    gep_indices[1] = LLVMConstInt(LLVMInt32Type(), i + 1, false);
    ptr = LLVMBuildGEP(builder, result, gep_indices, 2, type->field_names[i]);
    if (i == index) {
      LLVMBuildStore(builder, LLVMGetParam(fn, 0), ptr);
    } else {
      LLVMBuildStore(builder, (type->fields[i] == NULL) ? LLVMConstInt(LLVMInt1Type(), 0, false) : LLVMConstNull(type->fields[i]->llvm), ptr);
    }
  }

  LLVMBuildRet(builder, LLVMBuildLoad(builder, result, "load_sum_constructor_return"));
  return 0;
}

int compile_type_product_constructor(context_t* ctx, type_t* type) {
  type_t* fn_type = type_product_constructor(type);
  if (compile_type_fn(fn_type) != 0) {
    fprintf(stderr, "error compiling type product constructor for %s; fault of type_product_constructor()\n", type->name);
    type_print(fn_type);
    fprintf(stderr, "\n");
    return 1;
  }
  LLVMValueRef fn = LLVMAddFunction(ctx->mod_llvm, val_name(ctx->mod_src->name, type->name, fn_type), fn_type->llvm);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(fn, "entry");
  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder, entry);

  LLVMValueRef gep_indices[2], ptr;
  LLVMValueRef result = LLVMBuildAlloca(builder, type->llvm, type->name);

  // The first operand indexes through the pointer
  gep_indices[0] = LLVMConstInt(LLVMInt32Type(), 0, false);
  for (int i = 0; i < type->num_fields; i++) {
    gep_indices[1] = LLVMConstInt(LLVMInt32Type(), i, false);
    ptr = LLVMBuildGEP(builder, result, gep_indices, 2, type->field_names[i]);
    LLVMBuildStore(builder, LLVMGetParam(fn, i), ptr);
  }

  LLVMBuildRet(builder, LLVMBuildLoad(builder, result, "load_product_constructor_return"));
  return 0;
}

int compile_type_product_getter (context_t* ctx, type_t* type, int index) {
  type_t* fn_type = type_product_getter(type->fields[index], type);
  if (compile_type_fn(fn_type) != 0) {
    fprintf(stderr, "error compiling type product getter; fault of type_product_getter()\n");
    type_print(fn_type);
    fprintf(stderr, "\n");
    return 1;
  }
  // type->field_names[index] + type_instance_name(type->fields[index], NULL)
  LLVMValueRef fn = LLVMAddFunction(ctx->mod_llvm, type->field_names[index], fn_type->llvm);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(fn, "entry");
  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder, entry);

  LLVMBuildRet(builder, LLVMBuildExtractValue(builder, LLVMGetParam(fn, 0), index, type->field_names[index]));
  return 0;
}

int compile_value_constructors (context_t* ctx, type_t* type) {
  switch (type->meta) {
    case TYPE_PRODUCT:
      if (compile_type_product_constructor(ctx, type) != 0) {
        fprintf(stderr, "error compile type constructor %s\n", type->name);
        return 1;
      }
      for (int j = 0; j < type->num_fields; j++) {
        if (compile_type_product_getter(ctx, type, j) != 0) {
          fprintf(stderr, "error compile type getter %s/%s\n", type->name, type->field_names[j]);
          return 1;
        }
      }
      break;
    case TYPE_SUM:
      for (int j = 0; j < type->num_fields; j++) {
        if (compile_type_sum_constructor(ctx, type, type->fields[j], j) != 0) {
          fprintf(stderr, "error compile type constructor %s\n", type->name);
          return 1;
        }
      }
      break;
    default:
      break;
  }
  return 0;
}

int module_compile (context_t* ctx) {
  if (ctx->mod_src->compiled == 1) {
    return 0;
  }

  module_t* mod_orig = ctx->mod_src;
  for (int i = 0; i < mod_orig->num_deps; i++) {
    // TODO: when this becomes concurrent, make a copy of ctx
    ctx->mod_src = mod_orig->deps[i];
    if (module_compile(ctx) != 0) {
      return 1;
    }
  }
  ctx->mod_src = mod_orig;

  // declare types
  type_t* type;
  for (int i = 0; i < ctx->mod_src->num_types; i++) {
    type = ctx->mod_src->types + i;
    if (type->meta != TYPE_HOLE && type->num_args == 0) {
      if (compile_type(type) != 0) {
        fprintf(stderr, "error compile type %s\n", type->name);
        return 1;
      }
      if (compile_value_constructors(ctx, type) != 0) {
        fprintf(stderr, "error compile value constructors for type %s\n", type->name);
        return 1;
      }
    }
  }
  for (int i = 0; i < ctx->mod_src->num_type_instances; i++) {
    type = ctx->mod_src->type_instances + i;
    if (type->num_args == 0) {
      if (compile_type(type) != 0) {
        fprintf(stderr, "error compile type instance %s\n", type->name);
        return 1;
      }
      if (compile_value_constructors(ctx, type) != 0) {
        fprintf(stderr, "error compile value constructors for type instance %s\n", type->name);
        return 1;
      }
    }
  }

  // declare values
  val_t* val;
  for (int i = 0; i < ctx->mod_src->table.num_symbols; i++) {
    val = ctx->mod_src->table.values + i;
    // skip value constructors, which are compiled with their types
    if (val->type->num_args > 0 || isupper(ctx->mod_src->table.names[i][0])) {
      continue;
    }

    if (val->type->meta == TYPE_FUNC) {
      // skip product getters, which are compiled with their type
      int is_product_getter = 0;
      type_t* product = val->type->fields[0];
      if (product->meta == TYPE_PRODUCT) {
        for (int j = 0; j < product->num_fields; j++) {
          if (strcmp(ctx->mod_src->table.names[i], product->field_names[j]) == 0) {
            is_product_getter = 1;
            break;
          }
        }
      }
      if (is_product_getter) {
        continue;
      }

      if (compile_type_fn(val->type) != 0) {
        val_print(ctx->mod_src->table.names[i], val, 0);
        fprintf(stderr, "\nerror compile function type %s in %s\n", ctx->mod_src->table.names[i], ctx->mod_src->name);
        return 1;
      }
      if (str_includes(ctx->mod_src->name, "/usr/local/lib/sith/c")) {
        // functions linked from c libs must retain their original names
        val->llvm = LLVMAddFunction(ctx->mod_llvm, ctx->mod_src->table.names[i], val->type->llvm);
      } else {
        val->llvm = LLVMAddFunction(ctx->mod_llvm, val_name(ctx->mod_src->name, ctx->mod_src->table.names[i], val->type), val->type->llvm);
      }
    } else {
      val->llvm = LLVMAddGlobal(ctx->mod_llvm, val->type->llvm, val_name(ctx->mod_src->name, ctx->mod_src->table.names[i], val->type));
    }
  }

  // compile functions
  for (int i = 0; i < ctx->mod_src->table.num_symbols; i++) {
    val = ctx->mod_src->table.values + i;
    if (val->type->meta == TYPE_FUNC && compile_fn(ctx, val, ctx->mod_src->table.names[i]) != 0) {
      fprintf(stderr, "error compile function %s\n", ctx->mod_src->table.names[i]);
      return 1;
    }
  }

  char* error = NULL;
  if (LLVMVerifyModule(ctx->mod_llvm, LLVMReturnStatusAction, &error) != 0) {
    fprintf(stderr, "error: %s\n", error);
    LLVMDisposeMessage(error);
    return 1;
  }
  LLVMDisposeMessage(error);

  ctx->mod_src->compiled = 1;
  return 0;
}

int codegen_init (context_t* ctx, module_t* mod) {
  LLVMInitializeNativeTarget();

  ctx->triple = LLVMGetDefaultTargetTriple();

  char* error = NULL;
  LLVMTargetRef target;
  if (LLVMGetTargetFromTriple(ctx->triple, &target, &error)) {
    fprintf(stderr, "LLVMGetTargetFromTriple: %s\n", error);
    return 1;
  }
  LLVMDisposeMessage(error);

  ctx->machine = LLVMCreateTargetMachine(target, ctx->triple, "generic", "", LLVMCodeGenLevelDefault, LLVMRelocDefault, LLVMCodeModelDefault);
  ctx->data_layout = LLVMCreateTargetDataLayout(ctx->machine);
  ctx->mod_llvm = LLVMModuleCreateWithName("main");
  ctx->mod_src = mod;
  return 0;
}

int emit_ir (context_t* ctx) {
  char* error = NULL;
  char ir_file[100];
  snprintf(ir_file, 100, "%s.ll", ctx->mod_src->name);
  if (LLVMPrintModuleToFile(ctx->mod_llvm, ir_file, &error)) {
    fprintf(stderr, "LLVMPrintModuleToFile: %s\n", error);
    return 1;
  }
  LLVMDisposeMessage(error);
  return 0;
}

int emit_object_code (context_t* ctx) {
  // Create a global main, since any module can define its own main
  val_t* local_main = symbol_table_get(&ctx->mod_src->table, (char*)"main", NULL);
  if (local_main == NULL) {
    fprintf(stderr, "no main function\n");
    return 1;
  }
  LLVMValueRef main = LLVMAddFunction(ctx->mod_llvm, "main", local_main->type->llvm);
  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main, "entry");
  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder, entry);
  LLVMBuildRet(builder, LLVMBuildCall(builder, local_main->llvm, NULL, 0, "local_main"));


  LLVMInitializeNativeAsmPrinter();
  LLVMSetTarget(ctx->mod_llvm, ctx->triple);
  LLVMSetModuleDataLayout(ctx->mod_llvm, ctx->data_layout);

  char* error = NULL;
  char object_file[100];
  snprintf(object_file, 100, "%s.o", ctx->mod_src->name);
  if (LLVMTargetMachineEmitToFile(ctx->machine, ctx->mod_llvm, object_file, LLVMObjectFile, &error)) {
    fprintf(stderr, "LLVMTargetMachineEmitToFile: %s\n", error);
    return 1;
  }
  LLVMDisposeMessage(error);

  char cmd[100];
  snprintf(cmd, 100, "clang -g %s.o -o %s", ctx->mod_src->name, ctx->mod_src->name);
  return system(cmd);
}

int exec_main (context_t* ctx) {
  LLVMLinkInMCJIT();
  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  char* error = NULL;
  LLVMExecutionEngineRef engine;
  if (LLVMCreateExecutionEngineForModule(&engine, ctx->mod_llvm, &error) != 0) {
    fprintf(stderr, "failed to create execution engine\n");
    abort();
  }
  if (error != NULL) {
    fprintf(stderr, "error: %s\n", error);
    LLVMDisposeMessage(error);
    exit(EXIT_FAILURE);
  }

  val_t* res = symbol_table_get(&ctx->mod_src->table, (char*)"main", NULL);
  if (res == NULL) {
    fprintf(stderr, "no main function\n");
    return 1;
  }
  int (*fn)(void) = LLVMGetPointerToGlobal(engine, res->llvm);
  return (*fn)();
}
