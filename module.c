#include "module.h"

void symbol_table_print (symbol_table_t* table, int depth);
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
  module->table.symbols = malloc(module->table.num_symbols * sizeof(symbol_t));
  return 0;
}

int module_compile (node_t* root, module_t* module) {
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
        //if (str_includes(filename, ".sith")) {
        //  if (parse(filename, &module->deps[i_dep]) != 0) {
        //    return 1;
        //  }
        //}
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
        //node_t* body_node = sig_node->next;
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
        //if (compile_fn(ctx, &module->table, sig_node->list, body_node, sym) != 0) {
        //  fprintf(stderr, "error compile funtion %s\n", sym->name);
        //  return 1;
        //}
      }
    }
    node = node->next;
  }
  return 0;
}
