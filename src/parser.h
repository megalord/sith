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
  EXPR_PROGN,
  EXPR_SWITCH,
  EXPR_VAR
} expr_type_t;

typedef struct expr_t {
  expr_type_t type;
  union {
    // EXPR_CONST
    struct { struct val_t* cnst; };
    // EXPR_FUNCALL
    struct { char* fn_name; int num_params; struct expr_t* params; };
    // EXPR_IF
    struct { struct expr_t *if_cond, *if_, *else_; };
    // EXPR_LET
    struct { struct symbol_table_t* let_table; struct expr_t* let_body; };
    // EXPR_PROGN
    struct { int num_exprs; struct expr_t* exprs; };
    // EXPR_SWITCH
    struct { int num_cases; struct expr_t *case_cond, *case_bodies; struct val_list_t* case_vals; };
    // EXPR_VAR
    struct { char* var_name; };
  };
} expr_t;

typedef enum {
  TYPE_PRIM,
  TYPE_FUNC,
  TYPE_PARAM,
  TYPE_PRODUCT,
  TYPE_SUM
} meta_type_t;

typedef struct type_t {
  char* name;
  meta_type_t meta;
  int num_fields;
  struct type_t* fields;
  char** field_names; // for sum or product types
} type_t;

typedef struct val_t {
  type_t* type;
  union {
    // type.meta == TYPE_PRIM
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

int parse_type (node_t* node, type_t* type);
int parse_if (symbol_table_t* table, list_t* list, expr_t* expr);
int parse_let (symbol_table_t* parent, list_t* list, expr_t* expr);
int parse_switch (symbol_table_t* table, list_t* list, expr_t* expr);
int parse_progn (symbol_table_t* table, node_t* node, expr_t* expr);
int parse_funcall (symbol_table_t* table, list_t* list, expr_t* expr);
int parse_expr (symbol_table_t* table, node_t* node, expr_t* expr);
int parse_defun (symbol_table_t* table, node_t* node);
symbol_table_t* symbol_table_new (symbol_table_t* parent, int len);
val_t* symbol_table_get (symbol_table_t* table, char* name);
val_t* symbol_table_add (symbol_table_t* table, char* name, val_t* val);

#endif
