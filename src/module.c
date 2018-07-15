#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lexer.h"
#include "module.h"
#include "parser.h"
#include "utils.h"

char cwd[1024];
const char* stdlib = "/usr/local/lib/sith/";
module_cache_t cache = { .len = 0, .max = 0, .modules = NULL };

val_t* module_deps_symbol_find (module_t *mod, char* name) {
  val_t* val;
  for (int i = 0; i < mod->num_deps; i++) {
    val = symbol_table_get(&(*mod->deps[i]).table, name);
    if (val != NULL) {
      return val;
    }
  }
  return NULL;
}

int module_cache_init () {
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    fprintf(stderr, "could not get cwd\n");
    return 1;
  }
  cache.len = 0;
  cache.max = 10;
  cache.modules = malloc(cache.max * sizeof(module_t));
  return 0;
}

module_t* module_cache_next () {
  if (cache.len == cache.max - 1) {
    cache.max = cache.max * 2;
    module_t* modules = malloc(cache.max * sizeof(module_t));
    memcpy(modules, cache.modules, cache.len);
    free(cache.modules);
    cache.modules = modules;
  }
  return cache.modules + cache.len++;
}

module_t* module_cache_find (char* name) {
  for (module_t* module = cache.modules; module < cache.modules + cache.len; module++) {
    if (strcmp(module->name, name) == 0) {
      return module;
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
  module->deps = malloc(module->num_deps * sizeof(module_t*));
  module->types = malloc(module->num_types * sizeof(type_t));
  module->table.max_symbols = module->table.num_symbols;
  module->table.names = malloc(module->table.max_symbols * sizeof(char*));
  module->table.values = malloc(module->table.max_symbols * sizeof(val_t));
  return 0;
}

int module_parse_node (node_t* root, module_t* module) {
  if (module_setup(module, root) != 0) {
    return 1;
  }

  node_t* node = root->list->fst;
  module_t** dep = module->deps;
  val_t* sym = module->table.values;
  int i_sym = 0;
  int i_type = 0;
  for (int i = 0; i < root->list->len; i++) {
    if (node->type != NODE_LIST) {
      fprintf(stderr, "sub-root node must be a list\n");
      continue;
    }
    if (node->list->fst->type == NODE_ATOM) {
      char* name = node->list->fst->atom->name;
      if (strcmp(name, "include") == 0) {
        *dep = module_load(node->list->fst->next->atom->name);
        dep++;
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
        sym->type = malloc(sizeof(type_t));
        sym->type->name = NULL;
        if (parse_type(node->list->fst->next->next, sym->type) != 0) {
          return 1;
        }
        if (sym->type->meta == TYPE_FUNC) {
          sym->body = NULL; // in case function is ffi declaration
        }
        sym++;
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

int module_parse_file (char* filename, module_t* module) {
  node_t root;
  if (node_from_file(filename, &root) != 0) {
    return 1;
  }
  if (module_parse_node(&root, module) != 0) {
    return 1;
  }
  return 0;
}

module_t* module_load (char* partial_path) {
  char path[2048] = "";
  switch (partial_path[0]) {
    case '/':
      strncpy(path, partial_path, strlen(partial_path));
      break;
    case '.':
      strncpy(path, cwd, strlen(cwd));
      strncat(path, partial_path + 1, strlen(partial_path) - 1);
      break;
    default:
      strncat(path, stdlib, strlen(stdlib));
      strncat(path, partial_path, strlen(partial_path));
      break;
  }

  module_t* mod = module_cache_find(path);
  if (mod != NULL) {
    return mod;
  }

  mod = module_cache_next();
  mod->name = malloc(strlen(path) + 1);
  strcpy(mod->name, path);

  strncat(path, ".sith", 5);
  if (module_parse_file(path, mod) != 0) {
    return NULL;
  }
  return mod;
}
