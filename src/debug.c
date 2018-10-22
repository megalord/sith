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
  int i;
  switch (type->meta) {
    case TYPE_PRIM:
      printf("%s", type->name);
      break;
    case TYPE_FUNC:
      printf("_%d ::", type->num_fields);
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
      if (type->fields == NULL) {
        for (i = 0; i < type->num_fields; i++) {
          printf(" %c", 'a' + i);
        }
      } else {
        for (i = 0; i < type->num_fields; i++) {
          printf(" ");
          type_print(type->fields[i]);
        }
      }
      printf(")");
      break;
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
        if (type->fields[i] != NULL) {
          type_print(type->fields[i]);
        } else {
          printf("%s", type->field_names[i]);
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
  printf("%*s %s", depth, "", name);
  type_print(val->type);
}

void symbol_table_print (symbol_table_t* table, int depth) {
  printf("TABLE %p (%d/%d):\n", (void*)table, table->num_symbols, table->max_symbols);
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
  printf("defines types:\n");
  for (int i = 0; i < module->num_types; i = i + 1) {
    printf(" - ");
    type_print(module->types + i);
    printf("\n");
  }
  printf("defines symbols:\n");
  for (int i = 0; i < module->table.num_symbols; i = i + 1) {
    printf(" - ");
    val_print(module->table.names[i], module->table.values + i, 0);
    printf("\n");
  }
  for (int i = 0; i < module->num_deps; i = i + 1) {
    module_print(module->deps[i]);
  }
}
