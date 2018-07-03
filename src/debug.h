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

void type_print (type_t* type) {
  int i;
  switch (type->meta) {
    case TYPE_PRIM:
      printf("%s", type->name);
      break;
    case TYPE_FUNC:
      printf("_%d ::", type->num_fields);
      for (i = 0; i < type->num_fields - 1; i++) {
        printf(" ");
        type_print(&type->fields[i]);
        printf(" ->");
      }
      printf(" ");
      type_print(&type->fields[i]);
      break;
    case TYPE_PARAM:
      printf("(%s", type->name);
      for (i = 0; i < type->num_fields; i++) {
        printf(" ");
        type_print(&type->fields[i]);
      }
      printf(")");
      break;
    default:
      printf("???");
      break;
  }
}

void symbol_print (symbol_t* sym, int depth) {
  printf("%*s %s", depth, "", sym->name);
  type_print(&sym->type);
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
