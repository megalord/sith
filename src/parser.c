#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
#include "lexer.h"
#include "parser.h"

#define NUM_EXPRS 9192
static expr_t exprs[NUM_EXPRS];
static int i_expr = 0;

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
    if (!islower(sub_node->atom->name[0])) {
      fprintf(stderr, "invalid variable name %s; must begin with lowercase letter\n", sub_node->atom->name);
      return 1;
    }

    table->names[i] = sub_node->atom->name;
    sub_node = sub_node->next;

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

int parse_match_destructure (module_t* module, symbol_table_t* table, node_t* node, type_t* ret, expr_t* pat, symbol_table_t** let_table) {
  int num_args = node->list->len; // -1 for fn name, +1 for return value
  node = node->list->fst;
  if (node->type != NODE_ATOM) {
    fprintf(stderr, "invalid match: pattern constructor is list\n");
    return 1;
  }
  pat->fn_name = node->atom->name;

  type_t** args = malloc(num_args * sizeof(type_t*));
  // get arg types
  //for (int i = 0; i < ret->num_fields; i++) {
  //  if (strcmp(ret->field_names[i], pat->fn_name) == 0) {
  //    break;
  //  }
  //}
  for (int i = 0; i < num_args - 1; i++) {
    args[i] = type_get_hole_builtin('a');
  }
  args[num_args - 1] = ret;

  pat->fn = find_fn(module, table, pat->fn_name, args, num_args);
  if (pat->fn == NULL) {
    fprintf(stderr, "no function found found for match\n");
    return 1;
  }

  *let_table = symbol_table_new(table, num_args - 1);
  assert(num_args == 2);
  /*
  for (int i = 0; i < num_args - 1; i++) {
    node = node->next;
    if (node->type != NODE_ATOM) {
      fprintf(stderr, "invalid match: pattern value is list\n");
      return 1;
    }

    (*let_table)->names[i] = node->atom->name;
    (*let_table)->values[i].type = pat->fn->type->fields[i];
  }
  */
  node = node->next;
  (*let_table)->names[0] = node->atom->name;
  int index = type_sum_index(ret, pat->fn_name);
  (*let_table)->values[0].type = ret->fields[index];
  return 0;
}

int parse_match (module_t* module, symbol_table_t* table, node_t* node, expr_t* expr) {
  if (parse_expr(module, table, node, expr->match_cond) != 0) {
    return 1;
  }

  if (expr->match_cond->type->meta != TYPE_SUM) {
    fprintf(stderr, "invalid match: can only match on sum type, got %d\n", expr->match_cond->type->meta);
    return 1;
  }

  node_t* sub_node;
  expr_t* curr_pat = expr->match_pats;
  expr_t* curr_body = expr->match_bodies;
  for (int i = 0; i < expr->num_pats; i++) {
    node = node->next;
    if (node->type != NODE_LIST || node->list->len != 2) {
      fprintf(stderr, "invalid match: clause must be list with length 2\n");
      return 1;
    }
    sub_node = node->list->fst;
    if (sub_node->type == NODE_ATOM) {
      curr_pat->form = EXPR_VAR;
      curr_pat->var_name = sub_node->atom->name;
      if (find_var(module, table, curr_pat->var_name, &curr_pat->var) != 0) {
        return 1;
      }
      if (parse_expr(module, table, sub_node->next, curr_body) != 0) {
        return 1;
      }
      curr_pat->type = curr_pat->var->type;
    } else {
      curr_pat->form = EXPR_FUNCALL;
      curr_body->form = EXPR_LET;
      if (parse_match_destructure(module, table, sub_node, expr->match_cond->type, curr_pat, &curr_body->let_table) != 0) {
        return 1;
      }
      curr_pat->type = curr_pat->fn->type->fields[curr_pat->fn->type->num_fields - 1];
      curr_body->let_body = expr_new();
      if (parse_expr(module, curr_body->let_table, sub_node->next, curr_body->let_body) != 0) {
        return 1;
      }
    }

    if (strcmp(curr_pat->type->name, expr->match_cond->type->name) != 0) {
      fprintf(stderr, "invalid match: pattern type does not fit condition\n");
      fprintf(stderr, "wanted: ");
      type_print(expr->match_cond->type);
      fprintf(stderr, ", got: ");
      type_print(curr_pat->type);
      fprintf(stderr, "\n");
      return 1;
    }

    curr_pat++;
    curr_body++;
  }
  return 0;
}

int parse_funcall (module_t* module, symbol_table_t* table, list_t* list, expr_t* expr) {
  expr->fn_name = list->fst->atom->name;
  expr->num_params = list->len - 1;
  expr->params = expr_new_i(expr->num_params);

  type_t** args = malloc((expr->num_params + 1) * sizeof(type_t*));
  node_t* node = list->fst->next;
  int i;
  for (i = 0; i < expr->num_params; i++) {
    if (parse_expr(module, table, node, expr->params + i) != 0) {
      return 1;
    }
    args[i] = expr->params[i].type;
    node = node->next;
  }
  args[i] = type_get_hole_builtin('a');

  expr->fn = find_fn(module, table, expr->fn_name, args, expr->num_params + 1);
  if (expr->fn == NULL) {
    fprintf(stderr, "no function %s found for funcall\n", expr->fn_name);
    type_print(args[0]);
    for (int j = 1; j < expr->num_params + 1; j++) {
      fprintf(stderr, " -> ");
      type_print(args[j]);
    }
    fprintf(stderr, "\n");
    return 1;
  }

  expr->type = type_fn_get_return(module, expr->fn->type, args);
  return 0;
}

int parse_expr (module_t* module, symbol_table_t* table, node_t* node, expr_t* expr) {
  switch (node->type) {
    case NODE_ATOM:
      if (node->atom->type == ATOM_IDENTIFIER) {
        expr->form = EXPR_VAR;
        expr->var_name = node->atom->name;
        if (find_var(module, table, node->atom->name, &expr->var) != 0) {
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
      } else if (strcmp(name, "match") == 0) {
        expr->form = EXPR_MATCH;
        expr->match_cond = expr_new();
        expr->num_pats = node->list->len - 2;
        expr->match_pats = expr_new_i(expr->num_pats);
        expr->match_bodies = expr_new_i(expr->num_pats);
        return parse_match(module, table, node->list->fst->next, expr);
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
  if (!islower(fn_name[0])) {
    fprintf(stderr, "invalid function name %s; must begin with lowercase letter\n", fn_name);
    return 1;
  }
  val_t* fn = symbol_table_get(table, fn_name, NULL);
  if (fn == NULL) {
    return 1;
  }
  type_t* type = fn->type;
  if (type->num_fields != sig_node->list->len) {
    fprintf(stderr, "%s definition does not match type signature\n", fn_name);
    return 1;
  }

  expr_t* expr = expr_new();
  expr->form = EXPR_LET;
  expr->let_body = expr_new();
  expr->let_table = symbol_table_new(table, type->num_fields - 1);
  fn->body = expr;

  node_t* param_node = sig_node->list->fst->next; // skip function name
  for (int i = 0; i < type->num_fields - 1; i++) {
    if (param_node->type != NODE_ATOM || param_node->atom->type != ATOM_IDENTIFIER) {
      fprintf(stderr, "Function parameter arguments must be atoms");
      return 1;
    }
    if (!islower(param_node->atom->name[0])) {
      fprintf(stderr, "invalid argument name %s; must begin with lowercase letter\n", param_node->atom->name);
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

int parse_type (module_t* mod, node_t* node, type_t* type) {
  type->field_names = NULL;
  switch (node->type) {
    case NODE_ATOM:
      type->meta = TYPE_ALIAS;
      type->num_fields = 1;
      type->fields = malloc(type->num_fields * sizeof(type_t*));
      type->fields[0] = type_find(mod, node);
      if (*type->fields == NULL) {
        fprintf(stderr, "type %s not found\n", node->atom->name);
        return 1;
      }
      break;
    case NODE_LIST:
      type->num_fields = node->list->len - 1;
      node = node->list->fst;
      if (node->type != NODE_ATOM) {
        fprintf(stderr, "invalid signature\n");
        return 1;
      }
      if (strcmp(node->atom->name, "->") == 0) {
        type->meta = TYPE_FUNC;
        type->fields = malloc(type->num_fields * sizeof(type_t*));
        return parse_type_func(mod, type, node->next);
      } else if (strcmp(node->atom->name, "+") == 0) {
        type->meta = TYPE_SUM;
        type->fields = NULL;
        return parse_type_sum(mod, type, node->next);
      } else if (strcmp(node->atom->name, "*") == 0) {
        type->meta = TYPE_PRODUCT;
        type->fields = NULL;
        return parse_type_product(mod, type, node->next);
      } else {
        type->meta = TYPE_PARAM;
        type->name = node->atom->name;
        type->fields = NULL;
        fprintf(stderr, "param types not supported\n");
        return 1;
      }
  }
  return 0;
}

int parse_type_template (module_t* mod, node_t* node, type_t* type) {
  node_t* name_node = node->list->fst;
  if (name_node->type != NODE_ATOM) {
    fprintf(stderr, "invalid template type name\n");
    return 1;
  }

  type->name = name_node->atom->name;
  type->num_args = node->list->len - 1;
  type->args = malloc(type->num_args * sizeof(char));

  for (int i = 0; i < type->num_args; i++) {
    name_node = name_node->next;
    if (name_node->type != NODE_ATOM) {
      fprintf(stderr, "invalid template type arg: must be atom\n");
      return 1;
    }
    if (strlen(name_node->atom->name) > 1 || !islower(name_node->atom->name[0])) {
      fprintf(stderr, "invalid template type arg: must be single lowercase letter\n");
      return 1;
    }
    type->args[i] = name_node->atom->name[0];
  }

  node = node->next;
  if (node->type == NODE_ATOM) {
    fprintf(stderr, "invalid template type body\n");
    return 1;
  }

  return parse_type(mod, node, type);
}

int parse_type_func (module_t* mod, type_t* type, node_t* node) {
  type_t* field;
  for (int i = 0; i < type->num_fields; i++) {
    field = type_find(mod, node);
    if (field == NULL) {
      fprintf(stderr, "type not found\n");
      node_print(node, 0);
      return 1;
    }
    if (field->meta == TYPE_HOLE) {
      // TODO: actually increment this to track all args
      type->num_args = 1;
      type->args = field->name;
    }
    type->fields[i] = field;
    node = node->next;
  }
  if (field->meta == TYPE_FUNC) {
    fprintf(stderr, "function return types not allowed\n");
    return 1;
  }
  return 0;
}

int parse_type_product (module_t* mod, type_t* type, node_t* node) {
  if (type->num_fields % 2 != 0) {
    fprintf(stderr, "invalid product type: wrong number of nodes");
    return 1;
  }
  type->num_fields = type->num_fields / 2;
  type->fields = malloc(type->num_fields * sizeof(type_t*));
  type->field_names = malloc(type->num_fields * sizeof(char*));

  node_t* sub_node;
  type_t* field;
  for (int i = 0; i < type->num_fields; i++) {
    if (node->type != NODE_ATOM) {
      fprintf(stderr, "invalid product type: field name must be an atom\n");
      return 1;
    }
    type->field_names[i] = node->atom->name;

    node = node->next;
    field = NULL;
    switch (node->type) {
      case NODE_ATOM:
        field = type_find(mod, node);
        if (field == NULL) {
          fprintf(stderr, "invalid product type: field type not found %s\n", node->atom->name);
          return 1;
        }
        break;
      case NODE_LIST:
        sub_node = node->list->fst;
        if (sub_node->type != NODE_ATOM) {
          fprintf(stderr, "invalid product type: field type invalid form\n");
          return 1;
        }
        field = type_find(mod, sub_node);
        if (field == NULL) {
          fprintf(stderr, "invalid product type: field type not found %s\n", sub_node->atom->name);
          return 1;
        }
        break;
    }
    type->fields[i] = field;
    node = node->next;
  }
  return 0;
}

int parse_type_sum (module_t* mod, type_t* type, node_t* node) {
  node_t* sub_node;
  type_t* field;
  type->field_names = malloc(type->num_fields * sizeof(char*));
  type->fields = malloc(type->num_fields * sizeof(type_t*));
  for (int i = 0; i < type->num_fields; i++) {
    field = NULL;
    switch (node->type) {
      case NODE_ATOM:
        type->field_names[i] = node->atom->name;
        type->fields[i] = NULL;
        break;
      case NODE_LIST:
        sub_node = node->list->fst;
        if (sub_node->type != NODE_ATOM) {
          fprintf(stderr, "invalid sum type\n");
          return 1;
        }
        type->field_names[i] = sub_node->atom->name;

        sub_node = sub_node->next;
        if (node->list->len == 2) {
          if (sub_node->type == NODE_ATOM && islower(sub_node->atom->name[0])) {
            field = type_get_hole_builtin(sub_node->atom->name[0]);
          } else {
            field = type_new();
            if (parse_type(mod, sub_node, field) != 0) {
              return 1;
            }
          }
        } else {
          // all must be atoms
          fprintf(stderr, "invalid sum type, too many values for constructor %s\n", type->field_names[i]);
          return 1;
        }
        break;
    }
    type->fields[i] = field;
    node = node->next;
  }
  return 0;
}
