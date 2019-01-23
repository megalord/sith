#ifndef	_CODEGEN_H
#define	_CODEGEN_H

#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>

#include "parser.h"

typedef struct {
  LLVMModuleRef mod_llvm;
  char* triple;
  LLVMTargetMachineRef machine;
  LLVMTargetDataRef data_layout;
  module_t* mod_src;
  LLVMBuilderRef builder;
} context_t;

int compile_type (type_t* type);
int compile_const_val (context_t* ctx, val_t* val, LLVMValueRef* result);
int compile_if (context_t* ctx, symbol_table_t* table, expr_t *expr, LLVMValueRef* result);
int compile_switch (context_t* ctx, symbol_table_t* table, expr_t* expr, LLVMValueRef* result);
int compile_funcall (context_t* ctx, symbol_table_t* table, expr_t* expr, LLVMValueRef* result);
int compile_expr (context_t* ctx, symbol_table_t* table, expr_t* expr, LLVMValueRef* result);
int compile_fn (context_t* ctx, val_t* val, char* name);
int module_compile (context_t* ctx);
int codegen_init (context_t* ctx, module_t* mod);
int emit_ir (context_t* ctx);
int emit_object_code (context_t* ctx);
int exec_main (context_t* ctx);

#endif
