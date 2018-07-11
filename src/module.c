#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "module.h"
#include "parser.h"
#include "utils.h"

val_t* module_deps_symbol_find (module_t *mod, char* name) {
  val_t* val;
  for (int i = 0; i < mod->num_deps; i++) {
    val = symbol_table_get(&mod->deps[i].table, name);
    if (val != NULL) {
      return val;
    }
  }
  return NULL;
}

int module_setup (module_t* module, node_t* root) {
  module->num_deps = 0;
  module->num_types = 0;
  module->table.num_symbols = 0;
  module->table.max_symbols = 0;
  module->table.parent = NULL;
  if (root->type != NODE_LIST) {
    fprintf(stderr, "root node must be a list\n");
    return 1;
  }
  node_t* node = root->list->fst;
  char* name;
  for (int i = 0; i < root->list->len; i = i + 1) {
    if (node->type == NODE_LIST && node->list->fst->type == NODE_ATOM) {
      name = node->list->fst->atom->name;
      if (strcmp(name, "include") == 0) {
        module->num_deps = module->num_deps + 1;
      } else if (strcmp(name, "deftype") == 0) {
        module->num_types = module->num_types + 1;
      } else if (strcmp(name, ":") == 0) {
        module->table.num_symbols = module->table.num_symbols + 1;
      }
    }
    node = node->next;
  }
  module->deps = malloc(module->num_deps * sizeof(module_t));
  module->types = malloc(module->num_types * sizeof(type_t));
  module->table.max_symbols = module->table.num_symbols;
  module->table.names = malloc(module->table.max_symbols * sizeof(char*));
  module->table.values = malloc(module->table.max_symbols * sizeof(val_t));
  return 0;
}

int module_parse (node_t* root, module_t* module) {
  if (module_setup(module, root) != 0) {
    return 1;
  }

  node_t* node = root->list->fst;
  int i_dep = 0;
  int i_type = 0;
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
          if (module_parse(&dep_root, module->deps + i_dep) != 0) {
            return 1;
          }
        }
        i_dep++;
      } else if (strcmp(name, "deftype") == 0) {
        node_t* name_node = node->list->fst->next;
        if (name_node->type != NODE_ATOM) {
          fprintf(stderr, "invalid type name\n");
          continue;
        }
        module->types[i_type].name = name_node->atom->name;
        if (parse_type(name_node->next, module->types + i_type) != 0) {
          return 1;
        }
        i_type++;
      } else if (strcmp(name, ":") == 0) {
        module->table.names[i_sym] = node->list->fst->next->atom->name;
        module->table.values[i_sym].type = malloc(sizeof(type_t));
        module->table.values[i_sym].type->name = NULL;
        if (parse_type(node->list->fst->next->next, module->table.values[i_sym].type) != 0) {
          return 1;
        }
        i_sym++;
      } else if (strcmp(name, "defun") == 0) {
        if (parse_defun(&module->table, node) != 0) {
          return 1;
        }
      }
    }
    node = node->next;
  }
  return 0;
}
