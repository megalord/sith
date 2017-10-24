#include <jit/jit.h>
#include <jit/jit-dump.h>

#include "sjit.h"

jit_type_t type_cstring;

sj_symbol_t* sj_table_get (sj_symbol_table_t* table, char* name) {
  for (int i = 0; i < table->len; i++) {
    if (strcmp(table->symbols[i].name, name) == 0) {
      return &table->symbols[i];
    }
  }
  if (table->parent != NULL) {
    return sj_table_get(table->parent, name);
  }
  return NULL;
}

jit_type_t str_to_jit_type (char* type) {
  if (strcmp(type, "int") == 0) {
    return jit_type_int;
  } else if (strcmp(type, "char") == 0) {
    return jit_type_sys_char;
  } else if (strcmp(type, "char*") == 0) {
    return type_cstring;
  } else if (strcmp(type, "void") == 0) {
    return jit_type_void;
  } else {
    fprintf(stderr, "unknown type: %s\n", type);
    return jit_type_void;
  }
}

jit_value_t to_jit_value (jit_function_t* fn, sj_symbol_table_t* table, atom_t* atom) {
  sj_symbol_t* sym;
  switch (atom->type) {
    case ATOM_CHAR:
      return jit_value_create_nint_constant(*fn, jit_type_sys_char, atoi(atom->name));
    case ATOM_INT:
      return jit_value_create_nint_constant(*fn, jit_type_int, atoi(atom->name));
    case ATOM_STRING:
      return jit_value_create_long_constant(*fn, type_cstring, (long)atom->name);
    case ATOM_IDENTIFIER:
      sym = sj_table_get(table, atom->name);
      if (sym == NULL) {
        fprintf(stderr, "value named %s not found in table\n", atom->name);
        return NULL;
      }
      if (sym->is_fn) {
        fprintf(stderr, "value %s is a function\n", atom->name);
        return NULL;
      }
      return sym->data.val;
    default:
      fprintf(stderr, "unknown type\n");
      return NULL;
  }
}

jit_value_t c_puts (jit_function_t* fn, jit_value_t* str) {
  jit_type_t puts_signature = jit_type_create_signature(jit_abi_cdecl, jit_type_int, &type_cstring, 1, 1);
  return jit_insn_call_native(*fn, "puts", puts, puts_signature, str, 1, JIT_CALL_NOTHROW);
}

/*jit_value_t c_printf (jit_function_t* fn, sj_symbol_t* symbols, int arity) {
  jit_type_t* arg_types = malloc(arity * sizeof(jit_type_t));
  for (int i = 0; i < arity; i++) {
    arg_types[i] = symbols[i].type;
  }
  jit_value_t* arg_values = malloc(arity * sizeof(jit_value_t));
  for (int i = 0; i < arity; i++) {
    arg_values[i] = symbols[i].data.val;
  }
  jit_type_t printf_signature = jit_type_create_signature(jit_abi_cdecl, jit_type_int, arg_types, arity, 1);
  //jit_value_t hostmemptr = jit_value_create_long_constant(*fn, type_cstring, (long)str);
  return jit_insn_call_native(*fn, "printf", printf, printf_signature, arg_values, arity, JIT_CALL_NOTHROW);
}*/

int eval_statement(jit_function_t* fn, sj_symbol_table_t* table, node_t* node, jit_value_t* result) {
  if (node->type == NODE_LIST) {
    int arity = node->list->len - 1;
    node = node->list->fst;
    char* sub_fn_name = node->atom->name;

    jit_value_t* args = malloc(arity * sizeof(jit_value_t));
    for (int i = 0; i < arity; i++) {
      node = node->next;
      if (node->type == NODE_LIST) {
        eval_statement(fn, table, node, &args[i]);
      } else {
        args[i] = to_jit_value(fn, table, node->atom);
      }
    }

    if (strcmp(sub_fn_name, "puts") == 0) {
      *result = c_puts(fn, args);
      return 0;
    /*} else if (strcmp(sub_fn_name, "printf") == 0) {
      *result = c_printf(fn, node->next->atom->name, arity);
      return 0;*/
    }

    sj_symbol_t* sym = sj_table_get(table, sub_fn_name);
    if (sym == NULL) {
      fprintf(stderr, "symbol \"%s\" not found (%d:%d)\n", sub_fn_name, node->atom->line, node->atom->pos);
      return 1;
    }
    if (!sym->is_fn) {
      fprintf(stderr, "symbol \"%s\" is not a function (%d:%d)\n", sub_fn_name, node->atom->line, node->atom->pos);
      return 1;
    }

    *result = jit_insn_call(*fn, sym->name, sym->data.fn, NULL, args, arity, 0);
    return 0;
  }
  fprintf(stderr, "cannot eval atoms\n");
  return 1;
}

int parse_signature (list_t* list, jit_type_t* type) {
  int arity = list->len - 2;
  node_t* node = list->fst;
  if (node->type != NODE_ATOM || strcmp(node->atom->name, "->") != 0) {
    fprintf(stderr, "invalid signature");
    return 1;
  }

  jit_type_t* args = malloc(arity * sizeof(jit_type_t));
  for (int i = 0; i < arity; i++) {
    node = node->next;
    if (node->type != NODE_ATOM) {
      fprintf(stderr, "only primitive arguments are allowed");
      return 1;
    }
    args[i] = str_to_jit_type(node->atom->name);
  }

  node = node->next;
  if (node->type != NODE_ATOM) {
    fprintf(stderr, "only primitive return values are allowed\n");
    return 1;
  }
  jit_type_t ret = str_to_jit_type(node->atom->name);
  *type = jit_type_create_signature(jit_abi_cdecl, ret, args, arity, 1);
  return 0;
}

int compile_fn (jit_context_t* ctx, sj_symbol_table_t* parent, list_t* sig, node_t* body, sj_symbol_t* sym) {
  sym->data.fn = jit_function_create(*ctx, sym->type);

  sj_symbol_table_t table;
  table.len = sig->len - 1;
  table.symbols = malloc(table.len * sizeof(sj_symbol_t));
  table.parent = parent;

  node_t* node = sig->fst->next; // skip fn name

  for (int i = 0; i < table.len; i++) {
    if (node->type != NODE_ATOM) {
      fprintf(stderr, "invalid argument\n");
      return 1;
    }
    table.symbols[i].name = node->atom->name;
    table.symbols[i].type = jit_type_get_param(sym->type, i);
    table.symbols[i].data.val = jit_value_get_param(sym->data.fn, i);
    table.symbols[i].is_fn = 0;
    node = node->next;
  }

  // Evaluate statements in function body.
  jit_value_t result;
  while (body != NULL) {
    if (eval_statement(&sym->data.fn, &table, body, &result) != 0) {
      return 1;
    }
    body = body->next;
  }
  jit_insn_return(sym->data.fn, result);

  fprintf(stdout, "%s: ", sym->name);
  jit_dump_function(stdout, sym->data.fn, "F [uncompiled]");
  jit_function_compile(sym->data.fn);
  return 0;
}

int module_setup (sj_module_t* module, node_t* root) {
  module->len = 0;
  module->table.len = 0;
  module->table.parent = NULL;
  if (root->type != NODE_LIST) {
    fprintf(stderr, "root node must be a list\n");
    return 1;
  }
  node_t* node = root->list->fst;
  for (int i = 0; i < root->list->len; i = i + 1) {
    if (node->type == NODE_LIST && node->list->fst->type == NODE_ATOM) {
      if (strcmp(node->list->fst->atom->name, "include") == 0) {
        module->len = module->len + 1;
      } else if (strcmp(node->list->fst->atom->name, ":") == 0) {
        module->table.len = module->table.len + 1;
      }
    }
    node = node->next;
  }
  module->deps = malloc(module->len * sizeof(sj_module_t));
  module->table.symbols = malloc(module->table.len * sizeof(sj_symbol_t));
  return 0;
}

int module_compile (jit_context_t* ctx, node_t* root, sj_module_t* module) {
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
          module->table.symbols[i_sym].is_fn = 1;
          parse_signature(node->list->fst->next->next->list, &module->table.symbols[i_sym].type);
        }
        i_sym++;
      } else if (strcmp(name, "defun") == 0) {
        node_t* sig_node = node->list->fst->next;
        node_t* body_node = sig_node->next;
        if (sig_node->type != NODE_LIST || sig_node->list->fst->type != NODE_ATOM) {
          fprintf(stderr, "invalid arg list\n");
          continue;
        }
        char* fn_name = sig_node->list->fst->atom->name;
        sj_symbol_t* sym = sj_table_get(&module->table, fn_name);
        if (sym == NULL) {
          fprintf(stderr, "no type signature found for %s\n", sym->name);
          continue;
        }
        if (sym->is_fn != 1) {
          fprintf(stderr, "%s is not a function\n", sym->name);
          continue;
        }
        if (jit_type_num_params(sym->type) != sig_node->list->len - 1) {
          fprintf(stderr, "definition does not match type signature %s\n", sym->name);
          continue;
        }
        if (compile_fn(ctx, &module->table, sig_node->list, body_node, sym) != 0) {
          fprintf(stderr, "error compile funtion %s\n", sym->name);
          return 1;
        }
      }
    }
    node = node->next;
  }
  return 0;
}

int compile_root (node_t* node, sj_module_t* mod) {
  jit_init();
  jit_context_t ctx = jit_context_create();

  type_cstring = jit_type_create_pointer(jit_type_sys_char, 1);

  //sj_module_t* deps = malloc(mod.len * sizeof(sj_module_t));
  //for (int i = 0; i < mod.len; i++) {
  //  node_from_file(filename, &node);
  //  module_compile(&ctx, &deps[i], );
  //}
  if (module_compile(&ctx, node, mod) != 0) {
    return 1;
  }

  //jit_context_destroy(ctx);
  return 0;
}

int exec_main (sj_module_t* mod) {
  sj_symbol_t* main = sj_table_get(&mod->table, "main");
  if (main != NULL) {
    void* args[0];
    jit_int result;
    jit_function_apply(main->data.fn, args, &result);
    return (int)result;
  } else {
    fprintf(stderr, "no main function\n");
    return 1;
  }
}
