#include <jit/jit.h>
#include "lexer.h"

/*void print_type (type_t* type) {
  if (type == jit_type_int) {
    printf("int");
  } else if (type == jit_type_void) {
    printf("void");
  } else if (type == type_cstring) {
    printf("char*");
  } else {
    printf("unknown");
  }
}*/

void symbol_print (symbol_t* sym, int depth) {
  if (sym->type.meta == TYPE_FUNC) {
    printf("%*s", depth, "");
    int arity = sym->type.num_fields;
    printf("%s_%d ::", sym->name, arity);
    int i;
    for (i = 0; i < arity - 1; i++) {
      printf(" %s", sym->type.fields[i].name);
      printf(" ->");
    }
    printf(" %s", sym->type.fields[i].name);
  } else {
    printf("%*s ", depth, "");
    printf("%s", sym->type.name);
  }
}

void symbol_table_print (symbol_table_t* table, int depth) {
  for (int i = 0; i < table->num_symbols; i++) {
    symbol_print(&table->symbols[i], depth);
    printf("\n");
  }
}

void module_print (module_t* module) {
  printf("--------------------------\n");
  printf("MODULE %s\n", module->name);
  printf("requires:\n");
  for (int i = 0; i < module->num_deps; i = i + 1) {
    printf(" - %s\n", module->deps[i].name);
  }
  printf("defines symbols:\n");
  for (int i = 0; i < module->table.num_symbols; i = i + 1) {
    printf(" - ");
    symbol_print(&module->table.symbols[i], 0);
    printf("\n");
  }
  for (int i = 0; i < module->num_deps; i = i + 1) {
    module_print(&module->deps[i]);
  }
}
