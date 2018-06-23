#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "module.h"
#include "utils.h"

symbol_t* symbol_table_get (symbol_table_t* table, char* name) {
  for (int i = 0; i < table->num_symbols; i++) {
    if (strcmp(table->symbols[i].name, name) == 0) {
      return &table->symbols[i];
    }
  }
  if (table->parent != NULL) {
    return symbol_table_get(table->parent, name);
  }
  return NULL;
}

symbol_t* symbol_table_add (symbol_table_t* table, symbol_t* symbol) {
  if (table->num_symbols == table->max_symbols) {
    table->max_symbols = table->max_symbols * 2;
    symbol_t* symbols = malloc(sizeof(symbol_t) * table->max_symbols);
    memcpy(symbols, table->symbols, sizeof(symbol_t) * table->num_symbols);
    free(table->symbols);
    table->symbols = symbols;
  }
  symbol_t* copy = table->symbols + table->num_symbols;
  table->symbols[table->num_symbols] = *symbol;
  table->num_symbols++;
  return copy;
}

symbol_t* module_deps_symbol_find (module_t *mod, char* name) {
  symbol_t* sym;
  for (int i = 0; i < mod->num_deps; i++) {
    sym = symbol_table_get(&mod->deps[i].table, name);
    if (sym != NULL) {
      return sym;
    }
  }
  return NULL;
}

int module_parse_func_type (list_t* list, type_t* type) {
  type->num_fields = list->len - 1;
  node_t* node = list->fst;
  if (node->type != NODE_ATOM || strcmp(node->atom->name, "->") != 0) {
    fprintf(stderr, "invalid signature");
    return 1;
  }

  type->fields = malloc(type->num_fields * sizeof(type_t));
  type_t* field;
  for (field = type->fields; field < type->fields + type->num_fields - 1; field++) {
    node = node->next;
    if (node->type != NODE_ATOM) {
      fprintf(stderr, "only primitive arguments are allowed");
      return 1;
    }
    field->name = node->atom->name;
    field->meta = TYPE_PRIM;
    field->num_fields = 0;
    field->fields = NULL;
    field->field_names = NULL;
  }

  node = node->next;
  if (node->type != NODE_ATOM) {
    fprintf(stderr, "only primitive return values are allowed\n");
    return 1;
  }
  field->name = node->atom->name,
  field->meta = TYPE_PRIM,
  field->num_fields = 0,
  field->fields = NULL;
  field->field_names = NULL;
  return 0;
}

int module_setup (module_t* module, node_t* root) {
  module->num_deps = 0;
  module->table.num_symbols = 0;
  module->table.max_symbols = 0;
  module->table.parent = NULL;
  if (root->type != NODE_LIST) {
    fprintf(stderr, "root node must be a list\n");
    return 1;
  }
  node_t* node = root->list->fst;
  for (int i = 0; i < root->list->len; i = i + 1) {
    if (node->type == NODE_LIST && node->list->fst->type == NODE_ATOM) {
      if (strcmp(node->list->fst->atom->name, "include") == 0) {
        module->num_deps = module->num_deps + 1;
      } else if (strcmp(node->list->fst->atom->name, ":") == 0) {
        module->table.num_symbols = module->table.num_symbols + 1;
      }
    }
    node = node->next;
  }
  module->deps = malloc(module->num_deps * sizeof(module_t));
  module->table.max_symbols = module->table.num_symbols;
  module->table.symbols = malloc(module->table.max_symbols * sizeof(symbol_t));
  return 0;
}

int module_parse (node_t* root, module_t* module) {
  if (module_setup(module, root) != 0) {
    return 1;
  }

  node_t* node = root->list->fst;
  int i_dep = 0;
  int i_sym = 0;
  for (int i = 0; i < root->list->len; i++) {
    if (node->type != NODE_LIST) {
      fprintf(stderr, "sub-root node must be a list\n");
      continue;
    }
    if (node->list->fst->type == NODE_ATOM) {
      char* name = node->list->fst->atom->name;
      if (strcmp(name, "include") == 0) {
        module->deps[i_dep].name = node->list->fst->next->atom->name;
        if (str_includes(module->deps[i_dep].name, ".sith")) {
          node_t dep_root;
          if (node_from_file(module->deps[i_dep].name, &dep_root) != 0) {
            return 1;
          }
          if (module_parse(&dep_root, &module->deps[i_dep]) != 0) {
            return 1;
          }
        }
        i_dep++;
      } else if (strcmp(name, ":") == 0) {
        module->table.symbols[i_sym].name = node->list->fst->next->atom->name;
        if (node->list->fst->next->next->type == NODE_LIST) {
          //module->table.symbols[i_sym].type.name = "Function";
          module->table.symbols[i_sym].type.meta = TYPE_FUNC;
          module_parse_func_type(node->list->fst->next->next->list, &module->table.symbols[i_sym].type);
        }
        i_sym++;
      } else if (strcmp(name, "defun") == 0) {
        node_t* sig_node = node->list->fst->next;
        if (sig_node->type != NODE_LIST || sig_node->list->fst->type != NODE_ATOM) {
          fprintf(stderr, "invalid arg list\n");
          continue;
        }
        char* fn_name = sig_node->list->fst->atom->name;
        symbol_t* sym = symbol_table_get(&module->table, fn_name);
        if (sym == NULL) {
          fprintf(stderr, "no type signature found for %s\n", sym->name);
          continue;
        }
        if (sym->type.meta != TYPE_FUNC) {
          fprintf(stderr, "%s is not a function\n", sym->name);
          continue;
        }
        if (sym->type.num_fields != sig_node->list->len) {
          fprintf(stderr, "%s definition does not match type signature\n", sym->name);
          continue;
        }
        sym->type.field_names = malloc((sym->type.num_fields - 1) * sizeof(char*));
        node_t* param_node = sig_node->list->fst->next; // skip function name
        for (int j = 0; j < sym->type.num_fields - 1; j++) {
          if (param_node->type != NODE_ATOM || param_node->atom->type != ATOM_IDENTIFIER) {
            fprintf(stderr, "Function parameter arguments must be atoms");
            return 1;
          }
          sym->type.field_names[j] = param_node->atom->name;
          param_node = param_node->next;
        }
        sym->data = sig_node->next;
      }
    }
    node = node->next;
  }
  return 0;
}
