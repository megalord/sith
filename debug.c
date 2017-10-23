#include <jit/jit.h>
#include "lexer.h"

void print_type (jit_type_t type) {
  if (type == jit_type_int) {
    printf("int");
  } else if (type == jit_type_void) {
    printf("void");
  } else if (type == type_cstring) {
    printf("char*");
  } else {
    printf("unknown");
  }
}

void symbol_print (sj_symbol_t* sym, int depth) {
  if (sym->is_fn) {
    printf("%*s", depth, "");
    int arity = jit_type_num_params(sym->type);
    printf("%s_%d :: ", sym->name, arity);
    for (int i = 0; i < arity; i++) {
      print_type(jit_type_get_param(sym->type, i));
      printf(" ->");
    }
    print_type(jit_type_get_return(sym->type));
  } else {
    printf("%*s ", depth, "");
    print_type(sym->type);
  }
}


void module_print (sj_module_t* module) {
  printf("--------------------------\n");
  printf("MODULE %s\n", module->name);
  printf("requires:\n");
  for (int i = 0; i < module->len; i = i + 1) {
    printf(" - %s\n", module->deps[i].name);
  }
  printf("defines symbols:\n");
  for (int i = 0; i < module->table.len; i = i + 1) {
    printf(" - ");
    symbol_print(&module->table.symbols[i], 0);
    printf("\n");
  }
  printf("function definitions:\n");
  for (int i = 0; i < module->table.len; i = i + 1) {
    if (module->table.symbols[i].is_fn) {
      printf(" - %s\n", module->table.symbols[i].name);
      //node_print(module->table.symbols[i].data.fn->body->fst, 0);
      printf("\n");
    }
  }
  for (int i = 0; i < module->len; i = i + 1) {
    module_print(&module->deps[i]);
  }
}
