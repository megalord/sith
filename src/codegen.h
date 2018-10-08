#ifndef	_CODEGEN_H
#define	_CODEGEN_H

#include <llvm-c/Core.h>

#include "parser.h"

int compile_type (type_t* type);
int compile_const_val (val_t* val, LLVMBuilderRef builder, LLVMValueRef* result);
int compile_if (module_t* mod, symbol_table_t* table, expr_t *expr, LLVMBuilderRef builder, LLVMValueRef* result);
int compile_switch (module_t* mod, symbol_table_t* table, expr_t* expr, LLVMBuilderRef builder, LLVMValueRef* result);
int compile_funcall (module_t* mod, symbol_table_t* table, expr_t* expr, LLVMBuilderRef builder, LLVMValueRef* result);
int compile_expr (module_t* mod, symbol_table_t* table, expr_t* expr, LLVMBuilderRef builder, LLVMValueRef* result);
int compile_fn (module_t* mod, val_t* val, char* name);
int module_compile (module_t* mod);
int codegen_init ();
int emit_ir (module_t* mod);
int emit_object_code (module_t* mod);
int exec_main (module_t* mod);

#endif
