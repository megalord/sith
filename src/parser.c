#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"

type_t TYPE_I8 = { .name = (char*)"I8", .meta = TYPE_PRIM, .num_fields = 0 };
type_t TYPE_I32 = { .name = (char*)"I32", .meta = TYPE_PRIM, .num_fields = 0 };
type_t TYPE_STR = { .name = (char*)"Str", .meta = TYPE_PRIM, .num_fields = 0 };

int parse_atom (symbol_table_t* table, atom_t* atom, val_t* val) {
  switch (atom->type) {
    case ATOM_CHAR:
      fprintf(stderr, "ATOM_CHAR not supported\n");
      return 1;
    case ATOM_IDENTIFIER:
      // value resolved in next phase
      break;
    case ATOM_INT:
      val->type = &TYPE_I32;
      val->data = malloc(sizeof(int));
      *(int*)val->data = (int)atoi(atom->name);
      if (val->data == 0 && strcmp(atom->name, "0") != 0) {
        fprintf(stderr, "unable to parse atom into int\n");
        return 1;
      }
      break;
    case ATOM_STRING:
      val->type = &TYPE_STR;
      val->data = atom->name;
      break;
  }
  return 0;
}

int parse_type (node_t* node, type_t* type) {
  switch (node->type) {
    case NODE_ATOM:
      type->name = node->atom->name;
      type->meta = TYPE_PRIM;
      type->num_fields = 0;
      type->fields = NULL;
      type->field_names = NULL;
      return 0;
    case NODE_LIST:
      type->num_fields = node->list->len - 1;
      node = node->list->fst;
      if (node->type != NODE_ATOM) {
        fprintf(stderr, "invalid signature\n");
        return 1;
      }
      if (strcmp(node->atom->name, "->") == 0) {
        type->meta = TYPE_FUNC;
      } else if (strcmp(node->atom->name, "+") == 0) {
        type->meta = TYPE_SUM;
      } else if (strcmp(node->atom->name, "*") == 0) {
        type->meta = TYPE_PRODUCT;
      } else {
        type->meta = TYPE_PARAM;
        type->name = node->atom->name;
      }
      type->fields = malloc(type->num_fields * sizeof(type_t));
      type->field_names = NULL;
      type_t* field;
      for (field = type->fields; field < type->fields + type->num_fields; field++) {
        node = node->next;
        field->name = NULL;
        if (parse_type(node, field) != 0) {
          return 1;
        }
      }
      if (field->meta == TYPE_FUNC) {
        fprintf(stderr, "function return types not allowed\n");
        return 1;
      }
      return 0;
  }
}

int parse_if (symbol_table_t* table, list_t* list, expr_t* expr) {
  node_t* node = list->fst->next;
  if (parse_expr(table, node, expr->if_cond) != 0) {
    return 1;
  }
  node = node->next;
  if (parse_expr(table, node, expr->if_) != 0) {
    return 1;
  }
  node = node->next;
  if (parse_expr(table, node, expr->else_) != 0) {
    return 1;
  }
  return 0;
}

int parse_switch (symbol_table_t* table, list_t* list, expr_t* expr) {
  node_t* node = list->fst->next;
  if (parse_expr(table, node, expr->case_cond) != 0) {
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
        if (parse_atom(table, sub_node->atom, curr_val_list->vals) != 0) {
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
          if (parse_atom(table, val_node->atom, curr_val_list->vals + j) != 0) {
            return 1;
          }
          val_node = val_node->next;
        }
        break;
    }

    if (parse_expr(table, sub_node->next, curr_expr) != 0) {
      return 1;
    }

    curr_expr++;
    curr_val_list++;
  }
  return 0;
}

int parse_progn (symbol_table_t* table, node_t* node, expr_t* expr) {
  expr_t* curr;
  for (curr = expr->exprs; curr < expr->exprs + expr->num_exprs; curr++) {
    if (parse_expr(table, node, curr) != 0) {
      return 1;
    }
    node = node->next;
  }
  return 0;
}

int parse_funcall (symbol_table_t* table, list_t* list, expr_t* expr) {
  expr_t* curr;
  node_t* node = list->fst->next;
  for (curr = expr->params; curr < expr->params + expr->num_params; curr++) {
    if (parse_expr(table, node, curr) != 0) {
      return 1;
    }
    node = node->next;
  }
  return 0;
}

int parse_expr (symbol_table_t* table, node_t* node, expr_t* expr) {
  switch (node->type) {
    case NODE_ATOM:
      if (node->atom->type == ATOM_IDENTIFIER) {
        expr->type = EXPR_VAR;
        expr->var_name = node->atom->name;
        return 0;
      } else {
        expr->type = EXPR_CONST;
        expr->cnst = malloc(sizeof(val_t));
        return parse_atom(table, node->atom, expr->cnst);
      }
    case NODE_LIST:
      if (node->list->fst->type == NODE_LIST) {
        fprintf(stderr, "cannot parse double list into expr\n");
        return 1;
      }
      char* name = node->list->fst->atom->name;
      if (strcmp(name, "if") == 0) {
        expr->type = EXPR_IF;
        expr_t* exprs = malloc(sizeof(expr_t) * 3);
        expr->if_cond = exprs;
        expr->if_ = exprs + 1;
        expr->else_ = exprs + 2;
        return parse_if(table, node->list, expr);
      } else if (strcmp(name, "progn") == 0) {
        expr->type = EXPR_PROGN;
        expr->num_exprs = node->list->len - 1;
        expr->exprs = malloc(sizeof(expr_t) * expr->num_exprs);
        return parse_progn(table, node->list->fst->next, expr);
      } else if (strcmp(name, "cond") == 0) {
        expr->type = EXPR_SWITCH;
        expr->case_cond = malloc(sizeof(expr_t));
        expr->num_cases = node->list->len - 2;
        expr->case_bodies = malloc(sizeof(expr_t) * expr->num_cases);
        expr->case_vals = malloc(sizeof(val_list_t) * expr->num_cases);
        return parse_switch(table, node->list, expr);
      } else {
        expr->type = EXPR_FUNCALL;
        expr->fn_name = name;
        expr->num_params = node->list->len - 1;
        expr->params = malloc(sizeof(expr_t) * expr->num_params);
        return parse_funcall(table, node->list, expr);
      }
  }
}

int parse_defun (symbol_table_t* table, node_t* node) {
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
  type->field_names = malloc((type->num_fields - 1) * sizeof(char*));
  node_t* param_node = sig_node->list->fst->next; // skip function name
  for (int j = 0; j < type->num_fields - 1; j++) {
    if (param_node->type != NODE_ATOM || param_node->atom->type != ATOM_IDENTIFIER) {
      fprintf(stderr, "Function parameter arguments must be atoms");
      return 1;
    }
    type->field_names[j] = param_node->atom->name;
    param_node = param_node->next;
  }
  expr_t* progn = malloc(sizeof(expr_t));
  progn->type = EXPR_PROGN;
  progn->num_exprs = node->list->len - 2;
  progn->exprs = malloc(sizeof(expr_t) * progn->num_exprs);
  val->body = progn;
  parse_progn(table, sig_node->next, progn);
  return 0;
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
