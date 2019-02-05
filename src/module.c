#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
#include "lexer.h"
#include "parser.h"
#include "types.h"

char cwd[1024];
const char* stdlib = "/usr/local/lib/sith/";
module_cache_t cache = { .len = 0, .max = 0, .modules = NULL };

module_t MODULE_BUILTIN = {
  .name = (char*)"builtin",
  .num_deps = 0,
  .deps = NULL,
  .num_types = 0,
  .types = NULL,
  .num_type_instances = 0,
  .max_type_instances = 10,
  .type_instances = NULL,
  .table = {
    .num_symbols = 0,
    .max_symbols = 0,
    .names = NULL,
    .values = NULL
  },
  .compiled = 0,
};

int module_cache_init () {
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    fprintf(stderr, "could not get cwd\n");
    return 1;
  }
  cache.len = 0;
  cache.max = 10;
  cache.modules = malloc(cache.max * sizeof(module_t));

  MODULE_BUILTIN.num_types = type_builtins(&MODULE_BUILTIN.types);
  MODULE_BUILTIN.type_instances = type_new_i(MODULE_BUILTIN.max_type_instances);
  MODULE_BUILTIN.table.num_symbols = 0;
  MODULE_BUILTIN.table.max_symbols = 2;
  MODULE_BUILTIN.table.names = malloc(sizeof(char*) * 2);
  MODULE_BUILTIN.table.values = malloc(sizeof(val_t) * 2);

  val_t* setf = malloc(sizeof(val_t));
  setf->type = type_new();
  type_t setf_ty = (type_t) {
    .name = NULL,
    .meta = TYPE_FUNC,
    .num_fields = 4,
    .field_names = NULL,
    .num_args = 4,
    .args = (char*)"abcd"
  };
  setf_ty.fields = malloc(4 * sizeof(type_t*));
  for (int i = 0; i < 4; i++) {
    setf_ty.fields[i] = TYPE_HOLES + i;
  }
  memcpy(setf->type, &setf_ty, sizeof(type_t));
  symbol_table_add(&MODULE_BUILTIN.table, (char*)"setf", setf);

  TYPE_CSTR = type_instance(TYPE_PTR, &TYPE_I8, type_instance_new(&MODULE_BUILTIN));
  return 0;
}

module_t* module_cache_next () {
  if (cache.len == cache.max - 1) {
    cache.max = cache.max * 2;
    module_t* modules = malloc(cache.max * sizeof(module_t));
    memcpy(modules, cache.modules, cache.len * sizeof(module_t));
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
  module->num_deps = 1;
  module->num_types = 0;
  module->table.num_symbols = 0;
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
  module->deps[0] = &MODULE_BUILTIN;
  module->types = type_new_i(module->num_types);
  module->table.max_symbols = (module->table.num_symbols == 0) ? 5 : module->table.num_symbols;
  module->num_type_instances = 0;
  module->max_type_instances = (module->num_types == 0) ? 0 : 2;
  module->type_instances = type_new_i(module->max_type_instances);
  module->table.names = malloc(module->table.max_symbols * sizeof(char*));
  module->table.values = malloc(module->table.max_symbols * sizeof(val_t));
  return 0;
}

int module_parse_node (node_t* root, module_t* module) {
  if (module_setup(module, root) != 0) {
    return 1;
  }

  node_t* node = root->list->fst;
  int i_sym = 0;
  int i_type = 0;
  char* name;

  // Must set all names before parsing types and values
  // so that the parsing phase can link a type with later types.
  for (int i = 0; i < root->list->len; i++) {
    if (node->type != NODE_LIST) {
      fprintf(stderr, "sub-root node must be a list\n");
      return 1;
    }
    if (node->list->fst->type == NODE_ATOM) {
      name = node->list->fst->atom->name;
      if (strcmp(name, "deftype") == 0) {
        node_t* name_node = node->list->fst->next;
        if (name_node->type == NODE_ATOM) {
          module->types[i_type].name = name_node->atom->name;
        } else if (name_node->list->fst->type == NODE_ATOM) {
          module->types[i_type].name = name_node->list->fst->atom->name;
        } else {
          fprintf(stderr, "invalid deftype form\n");
          node_print(name_node, 0);
          fprintf(stderr, "\n");
          node_print(name_node->next, 0);
          fprintf(stderr, "\n");
          return 1;
        }
        i_type++;
      } else if (strcmp(name, ":") == 0) {
        module->table.names[i_sym] = node->list->fst->next->atom->name;
        i_sym++;
      }
    }
    node = node->next;
  }

  module_t** dep = module->deps + 1;
  val_t* sym = module->table.values;
  node = root->list->fst;
  i_sym = 0;
  i_type = 0;
  for (int i = 0; i < root->list->len; i++) {
    if (node->type != NODE_LIST) {
      fprintf(stderr, "sub-root node must be a list\n");
      return 1;
    }
    if (node->list->fst->type == NODE_ATOM) {
      name = node->list->fst->atom->name;
      if (strcmp(name, "include") == 0) {
        *dep = module_load(node->list->fst->next->atom->name);
        if (*dep == NULL) {
          return 1;
        }
        dep++;
      } else if (strcmp(name, "deftype") == 0) {
        if (parse_deftype(module, module->types + i_type, node) != 0) {
          return 1;
        }
        i_type++;
      } else if (strcmp(name, ":") == 0) {
        sym->type = type_new();
        sym->type->name = NULL;
        if (parse_type(module, node->list->fst->next->next, sym->type) != 0) {
          return 1;
        }
        if (sym->type->meta == TYPE_FUNC) {
          sym->body = NULL; // in case function is ffi declaration
        }
        sym++;
        i_sym++;
      }
    }
    node = node->next;
  }

  // This is done after type parsing because type constructors would invalidate i_sym
  for (int i = 0; i < module->num_types; i++) {
    type_add_constructors(module, module->types + i);
  }

  node = root->list->fst;
  for (int i = 0; i < root->list->len; i++) {
    if (node->list->fst->type == NODE_ATOM) {
      name = node->list->fst->atom->name;
      if (strcmp(name, "defun") == 0) {
        if (parse_defun(module, &module->table, node) != 0) {
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
    fprintf(stderr, "failed in lexing %s\n", module->name);
    return 1;
  }
  if (module_parse_node(&root, module) != 0) {
    fprintf(stderr, "failed in parsing %s\n", module->name);
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
  mod->compiled = 0;
  strcpy(mod->name, path);

  strncat(path, ".sith", 5);
  if (module_parse_file(path, mod) != 0) {
    return NULL;
  }
  return mod;
}

symbol_table_t* symbol_table_new (symbol_table_t* parent, int len) {
  symbol_table_t* table = malloc(sizeof(symbol_table_t));
  table->parent = parent;
  table->num_symbols = len;
  table->max_symbols = len == 0 ? 2 : len;
  table->names = malloc(sizeof(char*) * len);
  table->values = malloc(sizeof(val_t) * len);
  return table;
}

val_t* symbol_table_get (symbol_table_t* table, char* name, type_t* type) {
  for (int i = 0; i < table->num_symbols; i++) {
    if (strcmp(table->names[i], name) != 0) {
      continue;
    }

    if (type == NULL) {
      return table->values + i;
    }

    int type_eq_res = type_eq(table->values[i].type, type);
    if (type_eq_res > 0) {
      return table->values + i;
    }
  }

  if (table->parent != NULL) {
    return symbol_table_get(table->parent, name, type);
  }
  return NULL;
}

val_t* symbol_table_add (symbol_table_t* table, char* name, val_t* val) {
  if (table->num_symbols == table->max_symbols) {
    table->max_symbols = table->max_symbols * 2;
    char** names = malloc(sizeof(char*) * table->max_symbols);
    val_t* vals = malloc(sizeof(val_t) * table->max_symbols);
    memcpy(names, table->names, sizeof(char*) * table->num_symbols);
    memcpy(vals, table->values, sizeof(val_t) * table->num_symbols);
    free(table->names);
    free(table->values);
    table->names = names;
    table->values = vals;
  }
  val_t* copy = table->values + table->num_symbols;
  table->names[table->num_symbols] = name;
  table->values[table->num_symbols] = *val;
  table->num_symbols++;
  return copy;
}

int type_add_constructors (module_t* mod, type_t* type) {
  val_t val = {};
  switch (type->meta) {
    case TYPE_SUM:
      for (int i = 0; i < type->num_fields; i++) {
        val.type = type_sum_constructor(type->fields[i], type);
        symbol_table_add(&mod->table, type->field_names[i], &val);
      }
      return 0;
    case TYPE_PRODUCT:
      val.type = type_product_constructor(type);
      symbol_table_add(&mod->table, type->name, &val);
      for (int i = 0; i < type->num_fields; i++) {
        val.type = type_product_getter(type->fields[i], type);
        val.type->name = type->field_names[i];
        symbol_table_add(&mod->table, type->field_names[i], &val);
      }
      return 0;
    case TYPE_ALIAS:
    case TYPE_FUNC:
    case TYPE_HOLE:
    case TYPE_OPAQUE:
    case TYPE_PARAM:
    case TYPE_PRIM:
      return 0;
  }
}

found_val_t module_deps_symbol_find (module_t *mod, char* name, type_t* type) {
  val_t* res;
  for (int i = 0; i < mod->num_deps; i++) {
    res = symbol_table_get(&(mod->deps[i]->table), name, type);
    if (res != NULL) {
      return (found_val_t){ .mod = mod->deps[i]->name, .val = res };
    }
  }
  return (found_val_t){ .mod = NULL, .val = NULL };
}

found_val_t find_var (module_t* module, symbol_table_t* table, char* name) {
  val_t* var = symbol_table_get(table, name, NULL);
  if (var != NULL) {
    return (found_val_t){ .mod = module->name, .val = var };
  }

  found_val_t res = module_deps_symbol_find(module, name, NULL);
  if (res.val != NULL) {
    return res;
  }

  fprintf(stderr, "var %s not found\n", name);
  return (found_val_t){ .mod = NULL, .val = NULL };
}

found_val_t find_fn (module_t* module, symbol_table_t* table, char* name, type_t** types, int num_types) {
  // should type setup be handled by caller?
  type_t type = { .name = NULL, .meta = TYPE_FUNC, .num_fields = num_types };
  type.fields = malloc(num_types * sizeof(type_t*));
  for (int i = 0; i < num_types; i++) {
    type.fields[i] = types[i];
  }

  val_t* fn = symbol_table_get(table, name, &type);
  if (fn != NULL) {
    return (found_val_t){ .mod = module->name, .val = fn };
  }
  found_val_t res = module_deps_symbol_find(module, name, &type);
  if (res.val != NULL) {
    return res;
  }

  fprintf(stderr, "fn %s not found\n", name);
  type_print(&type);
  puts("");
  module_print(module, 0);
  module_print(module->deps[0], 0);
  return (found_val_t){ .mod = NULL, .val = NULL };
}

type_t* type_instance_new (module_t* mod) {
  if (mod->num_type_instances == mod->max_type_instances) {
    mod->max_type_instances = mod->max_type_instances * 2;
    type_t* types = type_new_i(mod->max_type_instances);
    memcpy(types, mod->type_instances, mod->num_type_instances * sizeof(type_t));
    // free(mod->type_instances); program memory
    mod->type_instances = types;
  }
  return mod->type_instances + mod->num_type_instances++;
}

type_t* type_find_or_create_instance (module_t* mod, type_t* template, type_t** args) {
  for (int i = 0; i < mod->num_type_instances; i++) {
    if (strcmp(mod->type_instances[i].name, template->name) == 0 &&
        type_fields_match(mod->type_instances[i].fields, args, mod->type_instances[i].num_fields) == 1) {
      return mod->type_instances + i;
    }
  }

  return type_instance(template, args, type_instance_new(mod));
}

int module_defines_type (module_t* mod, type_t* type) {
  for (int i = 0; i < mod->num_types; i++) {
    if (mod->types + i == type) {
      return 1;
    }
  }
  return 0;
}

type_t* type_fn_get_return (module_t* mod, type_t* fn, type_t** args) {
  type_t* ret = fn->fields[fn->num_fields - 1];
  if (type_has_holes(ret) == 0) {
    return ret;
  }

  // find the module that the type is defined in
  module_t* type_mod = NULL;
  if (module_defines_type(mod, ret)) {
    type_mod = mod;
  } else {
    for (int i = 0; i < mod->num_deps; i++) {
      if (module_defines_type(mod->deps[i], ret)) {
        type_mod = mod->deps[i];
        break;
      }
    }
  }
  if (type_mod == NULL) {
    fprintf(stderr, "warning: module defining type %s not found, adding instance to current module (%s)\n", ret->name, mod->name);
    type_mod = mod;
  }

  return type_find_or_create_instance(type_mod, ret, type_fn_get_return_args(fn, args));
}

type_t* module_type_find (module_t* mod, char* name, int* i0, int* j0, int* k0) {
  for (int i = *i0; i < mod->num_types; i++) {
    if (strcmp(mod->types[i].name, name) == 0) {
      *i0 = i + 1;
      return mod->types + i;
    }
  }
  *i0 = mod->num_types;

  if (*j0 < 0) {
    return NULL;
  }

  type_t* type;

  int no_deps = -1;
  for (int j = *j0; j < mod->num_deps; j++) {
    type = module_type_find(mod->deps[j], name, k0, &no_deps, &no_deps);
    if (type != NULL) {
      *j0 = j;
      return type;
    }
    *k0 = 0;
  }
  *j0 = mod->num_deps;

  return NULL;
}

type_t* type_find (module_t* mod, node_t* node) {
  int i = 0, j = 0, k = 0;
  node_t* sub_node;
  switch (node->type) {
    case NODE_ATOM:
      if (strlen(node->atom->name) == 1 && islower(node->atom->name[0])) {
        return type_get_hole_builtin(node->atom->name[0]);
      }
      return module_type_find(mod, node->atom->name, &i, &j, &k);
    case NODE_LIST:
      sub_node = node->list->fst;
      if (sub_node->type != NODE_ATOM) {
        fprintf(stderr, "could not determine type name\n");
        return NULL;
      }
      char* name = sub_node->atom->name;
      type_t* template_type = NULL;
      module_t* template_mod = NULL;
      type_t* type = module_type_find(mod, name, &i, &j, &k);
      while (type != NULL) {
        // if the type is a template, save it for later to create an instance
        if (type_has_holes(type)) {
          template_type = type;
          template_mod = (k == 0) ? mod : mod->deps[j];
        } else if (type_matches_node(type, node)) {
          return type;
        }
        type = module_type_find(mod, name, &i, &j, &k);
      }

      if (template_type != NULL) {
        int num_args = node->list->len - 1;
        type_t** args = malloc(num_args * sizeof(type_t*));
        for (int l = 0; l < num_args; l++) {
          sub_node = sub_node->next;
          args[l] = type_find(mod, sub_node);
        }
        return type_find_or_create_instance(template_mod, template_type, args);
      }

      return NULL;
  }
}
