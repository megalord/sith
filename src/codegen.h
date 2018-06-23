#ifndef	_CODEGEN_H
#define	_CODEGEN_H

#include "module.h"

int module_compile (module_t* mod);
int emit_ir (module_t* mod);
int emit_object_code (module_t* mod);
int exec_main (module_t* mod);

#endif
