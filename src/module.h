#ifndef	_MODULE_H
#define	_MODULE_H

#include <llvm-c/Core.h>

#include "lexer.h"
#include "parser.h"

struct module_t;
typedef struct module_t {
  char* name;
  int num_deps;
  struct module_t** deps;
  int num_types;
  type_t* types;
  symbol_table_t table;
  LLVMModuleRef llvm;
} module_t;

typedef struct {
  int len, max;
  module_t* modules;
} module_cache_t;

val_t* module_deps_symbol_find (module_t *mod, char* name);
int module_cache_init ();
int module_parse_file (char* filename, module_t* module);
module_t* module_load (char* filename);

#endif
