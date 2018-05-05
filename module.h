#ifndef	_MODULE_H
#define	_MODULE_H

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
  node_t* data; // ?, values?
  /*union {
    value_t val;
    function_t fn;
  } data;*/
} symbol_t;

struct symbol_table_t;
typedef struct symbol_table_t {
  struct symbol_table_t* parent; // scoping
  int num_symbols;
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
} module_t;

#endif
