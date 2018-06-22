#ifndef	_MODULE_H
#define	_MODULE_H

#include <llvm-c/Core.h>

typedef enum {
  TYPE_PRIM,
  TYPE_FUNC,
  TYPE_PRODUCT,
  TYPE_SUM
} meta_type_t;

struct type_t;
typedef struct type_t {
  char* name;
  meta_type_t meta;
  int num_fields;
  struct type_t* fields;
  char** field_names; // for structs
} type_t;

//typedef struct {
//  int num_args;
//  symbol_t* args;
//} func_call_t;

typedef struct {
  char* name;
  type_t type;
  node_t* data;
  LLVMValueRef value;
} symbol_t;

struct symbol_table_t;
typedef struct symbol_table_t {
  struct symbol_table_t* parent; // scoping
  int num_symbols;
  int max_symbols;
  symbol_t* symbols;
} symbol_table_t;

struct module_t;
typedef struct module_t {
  char* name;
  int num_deps;
  struct module_t* deps;
  int num_types;
  type_t* types;
  symbol_table_t table;
  LLVMModuleRef llvm;
} module_t;

symbol_t* symbol_table_get (symbol_table_t* table, char* name);
symbol_t* symbol_table_add (symbol_table_t* table, symbol_t* symbol);
symbol_t* module_deps_symbol_find (module_t *mod, char* name);

#endif
