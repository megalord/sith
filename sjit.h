#ifndef	_SJIT_H
#define	_SJIT_H

typedef struct {
  char* name;
  int is_fn;
  jit_type_t type;
  union {
    jit_value_t val;
    jit_function_t fn;
  } data;
} sj_symbol_t;

struct sj_symbol_table_t;
typedef struct sj_symbol_table_t {
  struct sj_symbol_table_t* parent;
  int len;
  sj_symbol_t* symbols;
} sj_symbol_table_t;

struct sj_module_t;
typedef struct sj_module_t {
  char* name;
  int len;
  struct sj_module_t* deps;
  sj_symbol_table_t table;
} sj_module_t;

#endif
