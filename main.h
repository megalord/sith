#include "lexer.h"

struct symbol_t;
typedef struct {
  int arity;
  struct symbol_t* args;
  struct symbol_t* ret;
  list_t* body;
} func_t;

typedef struct symbol_t {
  char* name;
  int is_fn;
  union {
    atom_type_t type;
    func_t* fn;
  } data;
} symbol_t;

typedef struct {
  int len;
  symbol_t* symbols;
} symbol_table_t;

struct module_t;
typedef struct module_t {
  char* name;
  int num_deps;
  struct module_t* deps;
  symbol_table_t table;
} module_t;

int INDENTATION = 2;

// recursively used functions
int parse_filename (char*, char**);
int parse (char*, module_t*);
int compile_front (module_t*, char*, bool);

symbol_table_t global_table;
