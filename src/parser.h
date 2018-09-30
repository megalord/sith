#ifndef	_PARSER_H
#define	_PARSER_H

#include <llvm-c/Core.h>

#include "lexer.h"

struct expr_t;
struct symbol_table_t;
struct type_t;
struct val_t;
struct val_list_t;

typedef enum {
  EXPR_CONST,
  EXPR_FUNCALL,
  EXPR_IF,
  EXPR_LET,
  EXPR_MATCH,
  EXPR_PROGN,
  EXPR_SWITCH,
  EXPR_VAR
} expr_form_t;

typedef struct expr_t {
  expr_form_t form;
  struct type_t* type;
  union {
    // EXPR_CONST
    struct { struct val_t* cnst; };
    // EXPR_FUNCALL
    struct { char* fn_name; struct val_t* fn; int num_params; struct expr_t* params; };
    // EXPR_IF
    struct { struct expr_t *if_cond, *if_, *else_; };
    // EXPR_LET
    struct { struct symbol_table_t* let_table; struct expr_t* let_body; };
    // EXPR_MATCH
    struct { int num_pats; struct expr_t *match_cond, *match_pats, *match_bodies; };
    // EXPR_PROGN
    struct { int num_exprs; struct expr_t* exprs; };
    // EXPR_SWITCH
    struct { int num_cases; struct expr_t *case_cond, *case_bodies; struct val_list_t* case_vals; };
    // EXPR_VAR
    struct { char* var_name; struct val_t* var; };
  };
} expr_t;

typedef enum {
  TYPE_ALIAS,
  TYPE_PRIM,
  TYPE_FUNC,
  TYPE_PARAM,
  TYPE_PRODUCT,
  TYPE_SUM
} meta_type_t;

typedef struct type_t {
  char* name;
  meta_type_t meta;
  int is_template; // for param type templates
  int num_fields;
  struct type_t** fields;
  char** field_names; // for sum or product types
  LLVMTypeRef llvm; // populated during compilation
} type_t;

typedef struct val_t {
  type_t* type;
  union {
    // type.meta == TYPE_PRIM or TYPE_SUM without data
    struct { void* data; }; // use type.name, should be either string or int
    // type.meta == TYPE_FUNC or let binding
    struct { struct expr_t* body; };
    // otherwise
    struct { int num_fields; struct val_t* fields; };
  };
  LLVMValueRef llvm; // populated during compilation
} val_t;
/*
 * {
 *   .type = { .name = NULL, .meta = TYPE_FUNC, .num_fields = 2, .fields = [
 *     { .name = "Int", .meta = TYPE_PRIM, .num_fields = 0 },
 *     { .name = "Str", .meta = TYPE_PRIM, .num_fields = 0 }
 *   ],
 *   .len = 2 // must match type.num_fields
 *   .exprs = [
 *     { .type = EXPR_CONST, .cnst = { .type = { .name = "Int" .meta = TYPE_PRIM }, .data = 0 },
 *     { .type = EXPR_CONST, .cnst = { .type = { .name = "Str" .meta = TYPE_PRIM }, .data = "foo" }
 *   ]
 * }
 */

typedef struct val_list_t {
  int len;
  val_t* vals;
} val_list_t;

typedef struct symbol_table_t {
  struct symbol_table_t* parent; // scoping
  int num_symbols;
  int max_symbols;
  char** names;
  struct val_t* values;
} symbol_table_t;

struct module_t;
typedef struct module_t {
  char* name;
  int num_deps;
  struct module_t** deps;
  int num_types;
  type_t* types;
  symbol_table_t table;
  LLVMModuleRef llvm;
} module_t;

typedef struct {
  int len, max;
  module_t* modules;
} module_cache_t;

type_t* TYPE_POLY;
type_t* TYPE_I8;
type_t* TYPE_I32;
type_t* TYPE_PTR;
type_t* TYPE_CSTR;

type_t* type_new_i (int i);
type_t* type_new ();

int type_add_constructors (module_t* mod, type_t* type);
int parse_type (module_t* module, node_t* node, type_t* type);
int parse_atom (module_t* module, symbol_table_t* table, atom_t* atom, val_t* val);
int parse_if (module_t* module, symbol_table_t* table, list_t* list, expr_t* expr);
int parse_let (module_t* module, symbol_table_t* parent, list_t* list, expr_t* expr);
int parse_switch (module_t* module, symbol_table_t* table, list_t* list, expr_t* expr);
int parse_progn (module_t* module, symbol_table_t* table, node_t* node, expr_t* expr);
int parse_funcall (module_t* module, symbol_table_t* table, list_t* list, expr_t* expr);
int parse_expr (module_t* module, symbol_table_t* table, node_t* node, expr_t* expr);
int parse_defun (module_t* module, symbol_table_t* table, node_t* node);
symbol_table_t* symbol_table_new (symbol_table_t* parent, int len);
val_t* symbol_table_get (symbol_table_t* table, char* name);
val_t* symbol_table_add (symbol_table_t* table, char* name, val_t* val);
val_t* module_deps_symbol_find (module_t *mod, char* name);
int module_cache_init ();
int module_parse_file (char* filename, module_t* module);
module_t* module_load (char* filename);

#endif
