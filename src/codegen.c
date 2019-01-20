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
#include <llvm-c/Target.h>

#include "codegen.h"
#include "debug.h"
#include "parser.h"
#include "types.h"
#include "utils.h"

typedef void* (*fn_zero_arg_t) ();
typedef void* (*fn_one_arg_t) (void*);
typedef void* (*fn_two_arg_t) (void*, void*);
typedef void* (*fn_three_arg_t) (void*, void*, void*);


char* triple;
LLVMTargetMachineRef machine;
LLVMTargetDataRef data_layout;

char type_name[80];
char fn_name_buffer[80];

int type_instance_name_iter (char* str, int rem, type_t* type, type_t* template) {
  int i = 0;
  for (; i < (int)strlen(type->name); i++) {
    assert(i < rem);
    str[i] = type->name[i];
  }
  assert(i < rem);
  for (int j = 0; j < type->num_fields; j++) {
    if (template != NULL && template->fields[j]->meta != TYPE_HOLE) {
      continue;
    }
    if (type->fields[j] != NULL) {
      str[i++] = '_';
      i += type_instance_name_iter(str + i, rem - i, type->fields[j], NULL);
    }
  }
  return i;
}

char* type_instance_name (type_t* type, type_t* template) {
  int i = type_instance_name_iter(type_name, 80, type, template);
  type_name[i] = '\0';
  return type_name;
}

char* type_compiled_name (type_t* type, type_t* template) {
  // this function may not be needed; remove if type names always have all field names
  return (template == NULL) ? type->name : type_instance_name(type, template);
}

char* fn_name (char* name, type_t* fn_type) {
  int rem = 80;
  int i = 0;
  for (; i < (int)strlen(name); i++) {
    fn_name_buffer[i] = name[i];
    assert(i < rem);
  }
  assert(i < rem);
  for (int j = 0; j < fn_type->num_fields - 1; j++) {
    fn_name_buffer[i++] = '_';
    for (int k = 0; k < (int)strlen(fn_type->fields[j]->name); k++) {
      fn_name_buffer[i++] = fn_type->fields[j]->name[k];
      assert(i < rem);
    }
  }
  fn_name_buffer[i] = '\0';
  return fn_name_buffer;
}

char* fn_name_expr (expr_t* expr) {
  int rem = 80;
  int i = 0;
  for (; i < (int)strlen(expr->fn_name); i++) {
    fn_name_buffer[i] = expr->fn_name[i];
    assert(i < rem);
  }
  assert(i < rem);
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
    types[i+1] = (type->fields[i] == NULL) ? LLVMInt1Type() : type->fields[i]->llvm;
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
    case TYPE_PARAM:
      if (strcmp(type->name, "Ptr") == 0) {
        if (type->num_fields != 1) {
          fprintf(stderr, "Ptr type must have 1 field, got %d\n", type->num_fields);
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

int compile_const_val (val_t* val, LLVMBuilderRef builder, LLVMValueRef* result) {
  if (val->type->meta == TYPE_PRIM && strcmp(val->type->name, "I32") == 0) {
    *result = LLVMConstInt(LLVMInt32Type(), *(int*)val->data, 0);
  } else if (val->type->meta == TYPE_PARAM &&
             strcmp(val->type->name, "Ptr") == 0 &&
             val->type->num_fields == 1 &&
             strcmp(val->type->fields[0]->name, "I8") == 0) {
    int len = strlen((char*)val->data);
    LLVMValueRef int_list = LLVMBuildArrayAlloca(builder,
        LLVMArrayType(LLVMInt8Type(), len + 1),
        LLVMConstInt(LLVMInt8Type(), 1, false),
        "");
    LLVMBuildStore(builder, LLVMConstString((char*)val->data, len, false), int_list);
    *result = LLVMBuildBitCast(builder, int_list, LLVMPointerType(LLVMInt8Type(), 0), "");
  } else {
    fprintf(stderr, "cannot compile constant of type ");
    type_print(val->type);
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
  if (LLVMGetTypeKind(LLVMTypeOf(condition)) == LLVMPointerTypeKind) {
    condition = LLVMBuildLoad(builder, condition, "load_if_cond");
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

int compile_let (module_t* mod, symbol_table_t* table, expr_t* expr, LLVMBuilderRef builder, LLVMValueRef* result) {
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
  //  if (compile_expr(mod, table, val->body, builder, &val_result) != 0) {
  //    return 1;
  //  }
  //  LLVMBuildStore(builder, val_result, val->llvm);
  //}
  for (val_t* val = expr->let_table->values; val < expr->let_table->values + expr->let_table->num_symbols; val++) {
    if (compile_expr(mod, table, val->body, builder, &val->llvm) != 0) {
      return 1;
    }
  }
  return compile_expr(mod, expr->let_table, expr->let_body, builder, result);
}

int compile_match (module_t* mod, symbol_table_t* table, expr_t *expr, LLVMBuilderRef builder, LLVMValueRef* result) {
  LLVMTypeRef condition_ty;
  LLVMValueRef condition, on_val;
  LLVMValueRef fn = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));

  if (compile_expr(mod, table, expr->match_cond, builder, &condition) != 0) {
    return 1;
  }
  condition_ty = LLVMTypeOf(condition);
  if (LLVMGetTypeKind(condition_ty) == LLVMPointerTypeKind) {
    condition = LLVMBuildLoad(builder, condition, "load_match_cond");
  }
  condition_ty = LLVMTypeOf(condition);
  LLVMValueRef data = condition;
  if (LLVMGetTypeKind(condition_ty) == LLVMStructTypeKind) {
    condition = LLVMBuildExtractValue(builder, data, 0, "sum_idx");
  } else if (LLVMGetTypeKind(condition_ty) != LLVMIntegerTypeKind) {
    fprintf(stderr, "unexpected type kind for match condition\n");
    return 1;
  }
  condition_ty = LLVMTypeOf(condition);

  LLVMBasicBlockRef else_bb = LLVMAppendBasicBlock(fn, "else");
  LLVMBasicBlockRef end_bb = LLVMAppendBasicBlock(fn, "match_cont");
  LLVMValueRef match_val = LLVMBuildSwitch(builder, condition, else_bb, expr->num_pats);

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

    LLVMPositionBuilderAtEnd(builder, bbs[i]);
    switch (expr->match_pats[i].form) {
      case EXPR_VAR:
        if (compile_expr(mod, table, expr->match_bodies + i, builder, results + i) != 0) {
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
        expr->match_bodies[i].let_table->values[0].llvm = LLVMBuildExtractValue(builder, data, index + 1, "");
        expr->match_bodies[i].let_table->values[0].type = expr->match_cond->type->fields[index];
        if (compile_expr(mod, expr->match_bodies[i].let_table, expr->match_bodies[i].let_body, builder, results + i) != 0) {
          return 1;
        }
        break;
      default:
        fprintf(stderr, "invalid expr type for match: %d\n", expr->match_pats[i].form);
        return 1;
    }
    LLVMBuildBr(builder, end_bb);
    bbs[i] = LLVMGetInsertBlock(builder);
  }

  LLVMPositionBuilderAtEnd(builder, else_bb);
  LLVMBuildUnreachable(builder);

  LLVMPositionBuilderAtEnd(builder, end_bb);
  *result = LLVMBuildPhi(builder, LLVMTypeOf(results[0]), "matchtmp");
  LLVMAddIncoming(*result, results, bbs, expr->num_pats);

  free(bbs);
  free(results);
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
  free(results);
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

  LLVMValueRef fn = LLVMGetNamedFunction(mod->llvm, expr->fn_name);
  if (fn == NULL) {
    fn = LLVMGetNamedFunction(mod->llvm, fn_name_expr(expr));
  }
  if (fn == NULL) {
    fprintf(stderr, "function %s not found\n", expr->fn_name);
    return 1;
  }
  *result = LLVMBuildCall(builder, fn, args, expr->num_params, expr->fn_name);
  return 0;
}

int compile_expr (module_t* mod, symbol_table_t* table, expr_t* expr, LLVMBuilderRef builder, LLVMValueRef* result) {
  switch (expr->form) {
    case EXPR_CONST:
      return compile_const_val(expr->cnst, builder, result);
    case EXPR_FUNCALL:
      return compile_funcall(mod, table, expr, builder, result);
    case EXPR_IF:
      return compile_if(mod, table, expr, builder, result);
    case EXPR_LET:
      return compile_let(mod, table, expr, builder, result);
    case EXPR_MATCH:
      return compile_match(mod, table, expr, builder, result);
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
      if (expr->var->llvm == NULL) {
        *result = LLVMGetNamedGlobal(mod->llvm, expr->var_name);
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

int compile_fn (module_t* mod, val_t* val, char* name) {
  // ffi declaration
  if (val->body == NULL) {
    return 0;
  }

  for (int i = 0; i < val->type->num_fields - 1; i++) {
    val->body->let_table->values[i].llvm = LLVMGetParam(val->llvm, i);
  }

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(val->llvm, "entry");

  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder, entry);

  LLVMValueRef result;

  if (compile_expr(mod, val->body->let_table, val->body->let_body, builder, &result) != 0) {
    return 1;
  }

  LLVMBuildRet(builder, result);
  return 0;
}

int compile_type_sum_constructor (module_t* mod, type_t* type, type_t* field, int index) {
  if (field == NULL) {
    LLVMValueRef var = LLVMAddGlobal(mod->llvm, type->llvm, type->field_names[index]);
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
  compile_type_fn(fn_type);
  char name[80];
  int j;
  for(j = 0; j < 80 && type->field_names[index][j] != '\0'; j++) {
    name[j] = type->field_names[index][j];
  }
  name[j++] = '_';
  strncpy(name + j, type_instance_name(field, NULL), 79 - j);
  LLVMValueRef fn = LLVMAddFunction(mod->llvm, name, fn_type->llvm);

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

int compile_type_product_constructor(module_t* mod, type_t* type) {
  type_t* fn_type = type_product_constructor(type);
  compile_type_fn(fn_type);
  LLVMValueRef fn = LLVMAddFunction(mod->llvm, type_instance_name(type, NULL), fn_type->llvm);

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

int compile_type_product_getter (module_t* mod, type_t* type, int index) {
  type_t* fn_type = type_product_getter(type->fields[index], type);
  compile_type_fn(fn_type);
  // type->field_names[index] + type_instance_name(type->fields[index], NULL)
  LLVMValueRef fn = LLVMAddFunction(mod->llvm, type->field_names[index], fn_type->llvm);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(fn, "entry");
  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder, entry);

  LLVMBuildRet(builder, LLVMBuildExtractValue(builder, LLVMGetParam(fn, 0), index, type->field_names[index]));
  return 0;
}

int compile_value_constructors (module_t* mod, type_t* type) {
  switch (type->meta) {
    case TYPE_PRODUCT:
      if (compile_type_product_constructor(mod, type) != 0) {
        fprintf(stderr, "error compile type constructor %s\n", type->name);
        return 1;
      }
      for (int j = 0; j < type->num_fields; j++) {
        if (compile_type_product_getter(mod, type, j) != 0) {
          fprintf(stderr, "error compile type getter %s/%s\n", type->name, type->field_names[j]);
          return 1;
        }
      }
      break;
    case TYPE_SUM:
      for (int j = 0; j < type->num_fields; j++) {
        if (compile_type_sum_constructor(mod, type, type->fields[j], j) != 0) {
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

int module_compile (module_t* mod) {
  if (mod->llvm != NULL) {
    return 0;
  }

  module_t* dep;
  LLVMValueRef import;
  mod->llvm = LLVMModuleCreateWithName(mod->name);
  for (int i = 0; i < mod->num_deps; i++) {
    dep = mod->deps[i];
    if (module_compile(dep) != 0) {
      return 1;
    }

    import = LLVMGetFirstFunction(dep->llvm);
    while (import != NULL) {
      // only import function definitions (skip declarations)
      if (LLVMCountBasicBlocks(import) > 0 || str_includes(dep->name, "/usr/local/lib/sith/c")) {
        LLVMAddFunction(mod->llvm, LLVMGetValueName(import), LLVMGetElementType(LLVMTypeOf(import)));
      }
      import = LLVMGetNextFunction(import);
    }

    import = LLVMGetFirstGlobal(dep->llvm);
    while (import != NULL) {
      LLVMAddGlobal(mod->llvm, LLVMGetElementType(LLVMTypeOf(import)), LLVMGetValueName(import));
      import = LLVMGetNextGlobal(import);
    }
  }

  // declare types
  type_t* type;
  for (int i = 0; i < mod->num_types; i++) {
    type = mod->types + i;
    if (type->meta != TYPE_HOLE && type->num_args == 0) {
      if (compile_type(type) != 0) {
        fprintf(stderr, "error compile type %s\n", type->name);
        return 1;
      }
      if (compile_value_constructors(mod, type) != 0) {
        fprintf(stderr, "error compile value constructors for type %s\n", type->name);
        return 1;
      }
    }
  }
  for (int i = 0; i < mod->num_type_instances; i++) {
    type = mod->type_instances + i;
    if (type->num_args == 0) {
      if (compile_type(type) != 0) {
        fprintf(stderr, "error compile type instance %s\n", type->name);
        return 1;
      }
      if (compile_value_constructors(mod, type) != 0) {
        fprintf(stderr, "error compile value constructors for type instance %s\n", type->name);
        return 1;
      }
    }
  }

  // declare values
  val_t* val;
  for (int i = 0; i < mod->table.num_symbols; i++) {
    val = mod->table.values + i;
    // skip value constructors, which are compiled with their types
    // TODO: find a way to skip product type getters
    if (val->type->num_args > 0 || isupper(mod->table.names[i][0])) {
      continue;
    }
    if (val->type->meta == TYPE_FUNC) {
      if (compile_type_fn(val->type) != 0) {
        val_print(mod->table.names[i], val, 0);
        fprintf(stderr, "\nerror compile function type %s\n", mod->table.names[i]);
        return 1;
      }
      if (str_includes(mod->name, "/usr/local/lib/sith/c")) {
        // functions linked from c libs must retain their original names
        val->llvm = LLVMAddFunction(mod->llvm, mod->table.names[i], val->type->llvm);
      } else {
        val->llvm = LLVMAddFunction(mod->llvm, fn_name(mod->table.names[i], mod->table.values[i].type), val->type->llvm);
      }
    } else {
      val->llvm = LLVMAddGlobal(mod->llvm, val->type->llvm, mod->table.names[i]);
    }
  }

  // compile functions
  for (int i = 0; i < mod->table.num_symbols; i++) {
    val = mod->table.values + i;
    if (val->type->meta == TYPE_FUNC && compile_fn(mod, val, mod->table.names[i]) != 0) {
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

int codegen_init () {
  LLVMInitializeNativeTarget();

  triple = LLVMGetDefaultTargetTriple();

  char* error = NULL;
  LLVMTargetRef target;
  if (LLVMGetTargetFromTriple(triple, &target, &error)) {
    fprintf(stderr, "LLVMGetTargetFromTriple: %s\n", error);
    return 1;
  }
  LLVMDisposeMessage(error);

  machine = LLVMCreateTargetMachine(target, triple, "generic", "", LLVMCodeGenLevelDefault, LLVMRelocDefault, LLVMCodeModelDefault);
  data_layout = LLVMCreateTargetDataLayout(machine);
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
  LLVMInitializeNativeAsmPrinter();
  LLVMSetTarget(mod->llvm, triple);
  LLVMSetModuleDataLayout(mod->llvm, data_layout);

  char* error = NULL;
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

  val_t* res = symbol_table_get(&mod->table, (char*)"main", NULL);
  if (res == NULL) {
    fprintf(stderr, "no main function\n");
    return 1;
  }
  int (*fn)(void) = LLVMGetPointerToGlobal(engine, res->llvm);
  return (*fn)();
}
