#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lexer.h"
#include "parser.h"

#define NUM_EXPRS 9192
static expr_t exprs[NUM_EXPRS];
static int i_expr = 0;

char cwd[1024];
const char* stdlib = "/usr/local/lib/sith/";
module_cache_t cache = { .len = 0, .max = 0, .modules = NULL };

module_t MODULE_BUILTIN = {
  .name = (char*)"builtin",
  .num_deps = 0,
  .deps = NULL,
  .num_types = 0,
  .types = NULL,
  .table = {
    .num_symbols = 0,
    .max_symbols = 0,
    .names = NULL,
    .values = NULL
  },
  .llvm = NULL
};

expr_t* expr_new () {
  assert(i_expr < NUM_EXPRS);
  return exprs + i_expr++;
}

expr_t* expr_new_i (int i) {
  assert(i_expr + i - 1 < NUM_EXPRS);
  expr_t* start = exprs + i_expr;
  i_expr += i;
  return start;
}

int parse_atom (module_t* module, symbol_table_t* table, atom_t* atom, val_t* val) {
  switch (atom->type) {
    case ATOM_CHAR:
      fprintf(stderr, "ATOM_CHAR not supported\n");
      return 1;
    case ATOM_IDENTIFIER:
      // value resolved in next phase
      fprintf(stderr, "ATOM_IDENTIFIER not supported\n");
      return 1;
    case ATOM_INT:
      val->type = TYPE_I32;
      val->data = malloc(sizeof(int));
      *(int*)val->data = (int)atoi(atom->name);
      if (val->data == 0 && strcmp(atom->name, "0") != 0) {
        fprintf(stderr, "unable to parse atom into int\n");
        return 1;
      }
      break;
    case ATOM_STRING:
      val->type = TYPE_CSTR;
      val->data = atom->name;
      break;
  }
  return 0;
}

int parse_if (module_t* module, symbol_table_t* table, list_t* list, expr_t* expr) {
  node_t* node = list->fst->next;
  if (parse_expr(module, table, node, expr->if_cond) != 0) {
    return 1;
  }
  if (strcmp(expr->if_cond->type->name, "Bool") != 0) {
    fprintf(stderr, "if condition must be bool, got %s\n", expr->if_cond->type->name);
    return 1;
  }
  node = node->next;
  if (parse_expr(module, table, node, expr->if_) != 0) {
    return 1;
  }
  node = node->next;
  if (parse_expr(module, table, node, expr->else_) != 0) {
    return 1;
  }
  return 0;
}

int parse_let (module_t* module, symbol_table_t* parent, list_t* list, expr_t* expr) {
  if (list->len < 3) {
    fprintf(stderr, "invalid let syntax: not enough forms\n");
    return 1;
  }
  node_t* node = list->fst->next;
  if (node->type != NODE_LIST) {
    fprintf(stderr, "invalid let syntax: var bindings must be a list\n");
    return 1;
  }

  symbol_table_t* table = symbol_table_new(parent, node->list->len);
  expr->let_table = table;

  node_t* var_node = node->list->fst;
  node_t* sub_node;
  expr_t* var_expr;
  for (int i = 0; i < table->num_symbols; i++) {
    if (var_node->type != NODE_LIST) {
      fprintf(stderr, "invalid let binding\n");
      return 1;
    }

    sub_node = var_node->list->fst;
    if (sub_node->type != NODE_ATOM) {
      fprintf(stderr, "invalid let variable name\n");
      return 1;
    }

    table->names[i] = sub_node->atom->name;
    sub_node = sub_node->next;

    // TODO: allow expressions
    if (sub_node->type != NODE_ATOM) {
      fprintf(stderr, "let variable values must be constants\n");
      return 1;
    }

    var_expr = expr_new();
    if (parse_expr(module, parent, sub_node, var_expr) != 0) {
      return 1;
    }
    table->values[i].body = var_expr;
    table->values[i].type = var_expr->type;

    var_node = var_node->next;
  }

  expr->let_body = expr_new();
  if (list->len == 3) {
    if (parse_expr(module, table, node->next, expr->let_body) != 0) {
      return 1;
    }
  } else {
    expr->let_body->form = EXPR_PROGN;
    expr->let_body->num_exprs = list->len - 2;
    if (parse_progn(module, table, node->next, expr->let_body) != 0) {
      return 1;
    }
  }
  return 0;
}

int parse_switch (module_t* module, symbol_table_t* table, list_t* list, expr_t* expr) {
  node_t* node = list->fst->next;
  if (parse_expr(module, table, node, expr->case_cond) != 0) {
    return 1;
  }

  node_t* sub_node;
  expr_t* curr_expr = expr->case_bodies;
  val_list_t* curr_val_list = expr->case_vals;
  for (int i = 0; i < expr->num_cases; i++) {
    node = node->next;
    if (node->type != NODE_LIST || node->list->len != 2) {
      fprintf(stderr, "invalid case: expected list with length 2\n");
      return 1;
    }
    sub_node = node->list->fst;
    switch (sub_node->type) {
      case NODE_ATOM:
        curr_val_list->len = 1;
        curr_val_list->vals = malloc(sizeof(val_t));
        // TODO: handle else
        if (parse_atom(module, table, sub_node->atom, curr_val_list->vals) != 0) {
          return 1;
        }
        break;
      case NODE_LIST:
        curr_val_list->len = sub_node->list->len;
        curr_val_list->vals = malloc(sizeof(val_t) * curr_val_list->len);
        node_t* val_node = sub_node->list->fst;
        for (int j = 0; j < curr_val_list->len; j++) {
          if (val_node->type != NODE_ATOM) {
            fprintf(stderr, "case can only match atoms\n");
            return 1;
          }
          if (parse_atom(module, table, val_node->atom, curr_val_list->vals + j) != 0) {
            return 1;
          }
          val_node = val_node->next;
        }
        break;
    }

    if (parse_expr(module, table, sub_node->next, curr_expr) != 0) {
      return 1;
    }

    curr_expr++;
    curr_val_list++;
  }
  return 0;
}

int parse_progn (module_t* module, symbol_table_t* table, node_t* node, expr_t* expr) {
  expr->exprs = expr_new_i(expr->num_exprs);
  expr_t* curr;
  for (curr = expr->exprs; curr < expr->exprs + expr->num_exprs; curr++) {
    if (parse_expr(module, table, node, curr) != 0) {
      return 1;
    }
    node = node->next;
  }
  expr->type = curr->type;
  return 0;
}

int parse_funcall (module_t* module, symbol_table_t* table, list_t* list, expr_t* expr) {
  expr->fn_name = list->fst->atom->name;
  expr->fn = symbol_table_get(table, expr->fn_name);
  if (expr->fn == NULL) {
    expr->fn = module_deps_symbol_find(module, expr->fn_name);
  }
  if (expr->fn == NULL) {
    fprintf(stderr, "symbol %s not found (%d:%d)\n", expr->fn_name, list->fst->atom->line, list->fst->atom->pos);
    return 1;
  }
  if (expr->fn->type->meta != TYPE_FUNC) {
    fprintf(stderr, "%s is not a function\n", expr->fn_name);
    return 1;
  }

  expr->num_params = list->len - 1;
  if (expr->num_params != expr->fn->type->num_fields - 1) {
    fprintf(stderr, "signature of %s does not match\n", expr->fn_name);
    return 1;
  }

  expr->params = expr_new_i(expr->num_params);

  expr_t* curr;
  node_t* node = list->fst->next;
  for (curr = expr->params; curr < expr->params + expr->num_params; curr++) {
    if (parse_expr(module, table, node, curr) != 0) {
      return 1;
    }
    node = node->next;
  }
  expr->type = expr->fn->type->fields[expr->fn->type->num_fields - 1];
  return 0;
}

int parse_expr (module_t* module, symbol_table_t* table, node_t* node, expr_t* expr) {
  switch (node->type) {
    case NODE_ATOM:
      if (node->atom->type == ATOM_IDENTIFIER) {
        expr->form = EXPR_VAR;
        expr->var_name = node->atom->name;
        expr->var = symbol_table_get(table, expr->var_name);
        if (expr->var == NULL) {
          expr->var = module_deps_symbol_find(module, expr->var_name);
        }
        if (expr->var == NULL) {
          fprintf(stderr, "var %s not found\n", expr->var_name);
          return 1;
        }
        if (expr->var->type->meta == TYPE_FUNC) {
          fprintf(stderr, "%s is a function\n", expr->var_name);
          return 1;
        }
        expr->type = expr->var->type;
        return 0;
      } else {
        expr->form = EXPR_CONST;
        expr->cnst = malloc(sizeof(val_t));
        if (parse_atom(module, table, node->atom, expr->cnst) != 0) {
          return 1;
        }
        expr->type = expr->cnst->type;
        return 0;
      }
    case NODE_LIST:
      if (node->list->fst->type == NODE_LIST) {
        fprintf(stderr, "cannot parse double list into expr\n");
        return 1;
      }
      char* name = node->list->fst->atom->name;
      if (strcmp(name, "if") == 0) {
        expr->form = EXPR_IF;
        expr_t* exprs = expr_new_i(3);
        expr->if_cond = exprs;
        expr->if_ = exprs + 1;
        expr->else_ = exprs + 2;
        return parse_if(module, table, node->list, expr);
      } else if (strcmp(name, "let") == 0) {
        expr->form = EXPR_LET;
        return parse_let(module, table, node->list, expr);
      } else if (strcmp(name, "progn") == 0) {
        expr->form = EXPR_PROGN;
        expr->num_exprs = node->list->len - 1;
        return parse_progn(module, table, node->list->fst->next, expr);
      } else if (strcmp(name, "cond") == 0) {
        expr->form = EXPR_SWITCH;
        expr->case_cond = expr_new();
        expr->num_cases = node->list->len - 2;
        expr->case_bodies = expr_new_i(expr->num_cases);
        expr->case_vals = malloc(sizeof(val_list_t) * expr->num_cases);
        return parse_switch(module, table, node->list, expr);
      } else {
        expr->form = EXPR_FUNCALL;
        return parse_funcall(module, table, node->list, expr);
      }
  }
}

int parse_defun (module_t* module, symbol_table_t* table, node_t* node) {
  node_t* sig_node = node->list->fst->next;
  if (sig_node->type != NODE_LIST || sig_node->list->fst->type != NODE_ATOM) {
    fprintf(stderr, "invalid arg list\n");
    return 1;
  }
  char* fn_name = sig_node->list->fst->atom->name;
  val_t* val = symbol_table_get(table, fn_name);
  if (val == NULL) {
    fprintf(stderr, "no type signature found for %s\n", fn_name);
    return 1;
  }
  type_t* type = val->type;
  if (type->meta != TYPE_FUNC) {
    fprintf(stderr, "%s is not a function\n", fn_name);
    return 1;
  }
  if (type->num_fields != sig_node->list->len) {
    fprintf(stderr, "%s definition does not match type signature\n", fn_name);
    return 1;
  }

  expr_t* expr = expr_new();
  expr->form = EXPR_LET;
  expr->let_body = expr_new();
  expr->let_table = symbol_table_new(table, type->num_fields - 1);
  val->body = expr;

  node_t* param_node = sig_node->list->fst->next; // skip function name
  for (int i = 0; i < type->num_fields - 1; i++) {
    if (param_node->type != NODE_ATOM || param_node->atom->type != ATOM_IDENTIFIER) {
      fprintf(stderr, "Function parameter arguments must be atoms");
      return 1;
    }
    expr->let_table->names[i] = param_node->atom->name;
    expr->let_table->values[i].type = type->fields[i];
    param_node = param_node->next;
  }

  if (node->list->len == 3) {
    if (parse_expr(module, expr->let_table, sig_node->next, expr->let_body) != 0) {
      return 1;
    }
  } else {
    expr->let_body->form = EXPR_PROGN;
    expr->let_body->num_exprs = node->list->len - 2;
    if (parse_progn(module, expr->let_table, sig_node->next, expr->let_body) != 0) {
      return 1;
    }
  }
  return 0;
}

symbol_table_t* symbol_table_new (symbol_table_t* parent, int len) {
  symbol_table_t* table = malloc(sizeof(symbol_table_t));
  table->parent = parent;
  table->num_symbols = len;
  table->max_symbols = len;
  table->names = malloc(sizeof(char*) * len);
  table->values = malloc(sizeof(val_t) * len);
  return table;
}

val_t* symbol_table_get (symbol_table_t* table, char* name) {
  for (int i = 0; i < table->num_symbols; i++) {
    if (strcmp(table->names[i], name) == 0) {
      return table->values + i;
    }
  }
  if (table->parent != NULL) {
    return symbol_table_get(table->parent, name);
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

  type_t type;
  MODULE_BUILTIN.num_types = 5;
  MODULE_BUILTIN.types = type_new_i(MODULE_BUILTIN.num_types);
  type = (type_t) { .name = (char*)"POLY", .meta = TYPE_PRIM,  .num_fields = 0, .is_template = 0 };
  memcpy(MODULE_BUILTIN.types, &type, sizeof(type_t));
  TYPE_POLY = MODULE_BUILTIN.types;
  type = (type_t) { .name = (char*)"I8",   .meta = TYPE_PRIM,  .num_fields = 0, .is_template = 0 };
  memcpy(MODULE_BUILTIN.types + 1, &type, sizeof(type_t));
  TYPE_I8 = MODULE_BUILTIN.types + 1;
  type = (type_t) { .name = (char*)"I32",  .meta = TYPE_PRIM,  .num_fields = 0, .is_template = 0 };
  memcpy(MODULE_BUILTIN.types + 2, &type, sizeof(type_t));
  TYPE_I32 = MODULE_BUILTIN.types + 2;
  type = (type_t) { .name = (char*)"Ptr",  .meta = TYPE_PARAM, .num_fields = 1, .is_template = 1, .fields = &TYPE_POLY };
  memcpy(MODULE_BUILTIN.types + 3, &type, sizeof(type_t));
  TYPE_PTR = MODULE_BUILTIN.types + 3;
  type = (type_t) { .name = (char*)"Ptr",  .meta = TYPE_PARAM, .num_fields = 1, .is_template = 0, .fields = &TYPE_I8 };
  memcpy(MODULE_BUILTIN.types + 4, &type, sizeof(type_t));
  TYPE_CSTR = MODULE_BUILTIN.types + 4;

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
  module->num_deps = 1;
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
  module->deps[0] = &MODULE_BUILTIN;
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
  module_t** dep = module->deps + 1;
  val_t* sym = module->table.values;
  int i_sym = 0;
  int i_type = 0;
  char* name;
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
        node_t* name_node = node->list->fst->next;
        if (name_node->type != NODE_ATOM) {
          fprintf(stderr, "invalid type name\n");
          return 1;
        }
        module->types[i_type].name = name_node->atom->name;
        if (parse_type(module, name_node->next, module->types + i_type) != 0) {
          return 1;
        }
        i_type++;
      } else if (strcmp(name, ":") == 0) {
        module->table.names[i_sym] = node->list->fst->next->atom->name;
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
    fprintf(stderr, "failed in lexing\n");
    return 1;
  }
  if (module_parse_node(&root, module) != 0) {
    fprintf(stderr, "failed in parsing\n");
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
