#include <jit/jit.h>

typedef struct {
  char* name;
  int is_fn;
  jit_type_t type;
  union {
    jit_value_t val;
    jit_function_t fn;
  } data;
} sj_symbol_t;

struct sj_symbol_table_t;
typedef struct sj_symbol_table_t {
  struct sj_symbol_table_t* parent;
  int len;
  sj_symbol_t* symbols;
} sj_symbol_table_t;

struct sj_module_t;
typedef struct sj_module_t {
  char* name;
  int len;
  struct sj_module_t* deps;
  sj_symbol_table_t table;
} sj_module_t;

jit_type_t type_cstring;

sj_symbol_t* sj_table_get (sj_symbol_table_t* table, char* name) {
  for (int i = 0; i < table->len; i++) {
    if (strcmp(table->symbols[i].name, name) == 0) {
      return &table->symbols[i];
    }
  }
  return NULL;
}

jit_type_t to_jit_type (char* type) {
  if (strcmp(type, "int") == 0) {
    return jit_type_int;
  } else if (strcmp(type, "char*") == 0) {
    return type_cstring;
  } else if (strcmp(type, "void") == 0) {
    return jit_type_void;
  }
  return jit_type_void;
}

jit_value_t c_puts (jit_function_t* fn, char* str) {
  jit_type_t puts_signature = jit_type_create_signature(jit_abi_cdecl, jit_type_int, &type_cstring, 1, 1);
  jit_value_t hostmemptr = jit_value_create_long_constant(*fn, type_cstring, (long)str);
  return jit_insn_call_native(*fn, "puts", puts, puts_signature, &hostmemptr, 1, JIT_CALL_NOTHROW);
}

jit_value_t c_printf (jit_function_t* fn, sj_symbol_t* symbols, int arity) {
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
}

jit_value_t eval_statement(jit_function_t* fn, sj_symbol_table_t* table, node_t* node) {
  if (node->type == NODE_LIST) {
    int arity = node->list->len - 1;
    node = node->list->fst;
    char* sub_fn_name = node->atom->name;
    for (int i = 0; i < arity; i++) {
      node = node->next;
      //if (node->type == NODE_ATOM) {
      //  symbols[i]
      //}
    }

    if (strcmp(sub_fn_name, "puts") == 0) {
      return c_puts(fn, node->next->atom->name);
    }
    if (strcmp(sub_fn_name, "printf") == 0) {
      return c_printf(fn, node->next->atom->name, arity);
    }

    //node = node->next;
    //jit_value_t* args = malloc(arity * sizeof(jit_value_t));
    //for (int i = 0; i < arity; i++) {
    //  // needs to first eval if list
    //  args[i] = node->atom->name; // ???
    //  node = node->next;
    //}
    //jit_value_t tmp = jit_insn_call(fn, sub_fn_name, sj_table_get(table, sub_fn_name), 0, args, arity, 0);
  }
  return 0;
}

int compile_fn (jit_context_t* ctx, func_t* src, jit_function_t* fn) {
  sj_symbol_table_t table;
  table.len = src->arity;
  table.symbols = malloc(table.len * sizeof(sj_symbol_t));

  jit_type_t* args = malloc(src->arity * sizeof(jit_type_t));
  for (int i = 0; i < src->arity; i++) {
    args[i] = to_jit_type(src->args[i].data.type);
    table.symbols[i].name = src->args[i].name;
    table.symbols[i].type = args[i];
    table.symbols[i].data.val = jit_value_get_param(*fn, i);
    table.symbols[i].is_fn = 0;
  }
  jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, to_jit_type(src->ret->data.type), args, src->arity, 1);
  *fn = jit_function_create(*ctx, sig);
  jit_type_free(sig);
  free(args);

  // Evaluate statements in function body.
  node_t* node = src->body->fst;
  jit_value_t result;
  for (int i = 0; i < src->body->len; i++) {
    result = eval_statement(fn, &table, node);
    node = node->next;
  }
  jit_insn_return(*fn, result);

  return 0;
}

int compile_module (jit_context_t* ctx, module_t* src, sj_module_t* mod) {
  mod->table.len = src->table.len;
  mod->table.parent = NULL;
  mod->table.symbols = malloc(mod->table.len * sizeof(sj_symbol_t));
  for (int i = 0; i < mod->table.len; i++) {
    mod->table.symbols[i].name = src->table.symbols[i].name;
    // assumes is_fn == 1
    compile_fn(ctx, src->table.symbols[i].data.fn, &mod->table.symbols[i].data.fn);
  }
  return 0;
}

int compile_root (module_t* src) {
  jit_init();
  jit_context_t ctx = jit_context_create();

  type_cstring = jit_type_create_pointer(jit_type_sys_char, 1);

  sj_module_t mod;
  mod.len = src->num_deps;
  sj_module_t* deps = malloc(mod.len * sizeof(sj_module_t));
  for (int i = 0; i < mod.len; i++) {
    compile_module(&ctx, &src->deps[i], &deps[i]);
  }
  compile_module(&ctx, src, &mod);
  sj_symbol_t* main = sj_table_get(&mod.table, "main");
  if (main != NULL) {
    void* args[0];
    jit_int result;
    jit_dump_function(stdout, main->data.fn, "F [uncompiled]");
    jit_function_compile(main->data.fn);
    jit_function_apply(main->data.fn, args, &result);
    jit_context_destroy(ctx);
    return (int)result;
  } else {
    jit_context_destroy(ctx);
    return 0;
  }
}
