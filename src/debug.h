#ifndef	_DEBUG_H
#define	_DEBUG_H

#include "lexer.h"

const char* atom_print_type (atom_type_t type) {
  switch (type) {
    case ATOM_CHAR:
      return "char";
    case ATOM_IDENTIFIER:
      return "symbol";
    case ATOM_INT:
      return "int";
    case ATOM_STRING:
      return "str";
  }
}

void node_print (node_t* node, int depth) {
  if (node->type == NODE_ATOM) {
    printf("%*s %s: %s\n", depth, "", atom_print_type(node->atom->type), node->atom->name);
  } else {
    if (node->type == NODE_LIST) {
      printf("%*s list: %d\n", depth, "", node->list->len);
      node_print(node->list->fst, depth + INDENTATION);
    }
  }
  if (node->next != NULL) {
    node_print(node->next, depth);
  }
}

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
    printf("%s :: %s", sym->name, sym->type.name);
  }
}

void symbol_table_print (symbol_table_t* table, int depth) {
  printf("TABLE (%d):\n", table->num_symbols);
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

#endif
