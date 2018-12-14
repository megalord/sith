#include "lexer.h"
#include "parser.h"

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
  if (type == NULL) {
    return;
  }

  int i;

  if (type->is_template) {
    printf("*");
  }

  switch (type->meta) {
    case TYPE_PRIM:
      printf("%s", type->name);
      break;
    case TYPE_FUNC:
      for (i = 0; i < type->num_fields - 1; i++) {
        printf(" ");
        type_print(type->fields[i]);
        printf(" ->");
      }
      printf(" ");
      type_print(type->fields[i]);
      break;
    case TYPE_PARAM:
      printf("(%s", type->name);
      for (i = 0; i < type->num_fields; i++) {
        printf(" ");
        type_print(type->fields[i]);
      }
      printf(")");
    case TYPE_PRODUCT:
      for (i = 0; i < type->num_fields; i++) {
        if (i != 0) {
          printf(", ");
        }
        printf("%s: ", type->field_names[i]);
        type_print(type->fields[i]);
      }
      break;
    case TYPE_SUM:
      printf("%s { ", type->name);
      for (i = 0; i < type->num_fields; i++) {
        if (i != 0) {
          printf(" | ");
        }
        printf("%s", type->field_names[i]);
        if (type->fields[i] != NULL) {
          printf(" :: ");
          type_print(type->fields[i]);
        }
      }
      printf(" }");
      break;
    default:
      printf("?%d?", type->meta);
      break;
  }
}

void val_print (char* name, val_t* val, int depth) {
  printf("%*s %s :: ", depth, "", name);
  type_print(val->type);
}

void symbol_table_print (symbol_table_t* table, int depth) {
  printf("TABLE (%d/%d):\n", table->num_symbols, table->max_symbols);
  for (int i = 0; i < table->num_symbols; i++) {
    val_print(table->names[i], table->values + i, depth);
    printf("\n");
  }
}

void module_print (module_t* module) {
  printf("--------------------------\n");
  printf("MODULE %s\n", module->name);
  printf("requires:\n");
  for (int i = 0; i < module->num_deps; i = i + 1) {
    printf(" - %s\n", module->deps[i]->name);
  }
  printf("defines types (%d/%d):\n", module->num_types, module->max_types);
  for (int i = 0; i < module->num_types; i = i + 1) {
    printf(" - ");
    type_print(module->types + i);
    printf("\n");
  }
  symbol_table_print(&module->table, 0);
}

void module_print_follow (module_t* module) {
  module_print(module);
  for (int i = 0; i < module->num_deps; i = i + 1) {
    module_print_follow(module->deps[i]);
  }
}
