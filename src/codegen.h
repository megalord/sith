#ifndef	_CODEGEN_H
#define	_CODEGEN_H

#include <llvm-c/Core.h>

#include "module.h"

int eval_statement(module_t* mod, symbol_table_t* table, node_t* node, LLVMBuilderRef builder, LLVMValueRef* result);
int module_compile (module_t* mod);
int emit_ir (module_t* mod);
int emit_object_code (module_t* mod);
int exec_main (module_t* mod);

#endif
