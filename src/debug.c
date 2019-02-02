#include "lexer.h"
#include "parser.h"

const char* atom_print_type (atom_type_t type) {
  switch (type) {
    case ATOM_CHAR:
      return "char";
    case ATOM_IDENTIFIER:
      return "symbol";
    case ATOM_INT:
      return "int";
    case ATOM_STRING:
      return "str";
  }
}

void node_print (node_t* node, int depth) {
  if (node->type == NODE_ATOM) {
    fprintf(stderr, "%*s %s: %s\n", depth, "", atom_print_type(node->atom->type), node->atom->name);
  } else {
    if (node->type == NODE_LIST) {
      fprintf(stderr, "%*s list: %d\n", depth, "", node->list->len);
      node_print(node->list->fst, depth + INDENTATION);
    }
  }
  if (node->next != NULL) {
    node_print(node->next, depth);
  }
}

void val_print (char* name, val_t* val, int depth) {
  fprintf(stderr, "%*s %s :: ", depth, "", name);
  type_print(val->type);
}

void symbol_table_print (symbol_table_t* table, int depth) {
  fprintf(stderr, "TABLE (%d/%d):\n", table->num_symbols, table->max_symbols);
  for (int i = 0; i < table->num_symbols; i++) {
    val_print(table->names[i], table->values + i, depth);
    fprintf(stderr, "\n");
  }
}

void module_print (module_t* module, int depth) {
  fprintf(stderr, "%*s--------------------------\n", depth, "");
  fprintf(stderr, "%*sMODULE %s\n", depth, "", module->name);
  fprintf(stderr, "%*srequires:\n", depth, "");
  for (int i = 0; i < module->num_deps; i = i + 1) {
    fprintf(stderr, "%*s - %s\n", depth, "", module->deps[i]->name);
  }
  fprintf(stderr, "%*sdefines types (%d):\n", depth, "", module->num_types);
  for (int i = 0; i < module->num_types; i = i + 1) {
    fprintf(stderr, "%*s - ", depth, "");
    type_print(module->types + i);
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "%*sdefines types instances (%d/%d):\n", depth, "", module->num_type_instances, module->max_type_instances);
  for (int i = 0; i < module->num_type_instances; i++) {
    fprintf(stderr, " - ");
    type_print(module->type_instances + i);
    fprintf(stderr, "\n");
  }
  symbol_table_print(&module->table, depth);
}

void module_print_follow (module_t* module, int depth) {
  module_print(module, 0);
  for (int i = 0; i < module->num_deps; i = i + 1) {
    module_print_follow(module->deps[i], depth + 4);
  }
}

void expr_print (expr_t* expr, int depth) {
  switch (expr->form) {
    case EXPR_CONST:
      val_print("const", expr->cnst, 0);
      break;
    case EXPR_FUNCALL:
      fprintf(stderr, "%*sfuncall %s\n", depth, "", expr->fn_name);
      for (int i = 0; i < expr->num_params; i++) {
        fprintf(stderr, "%*s - ", depth, "");
        expr_print(expr->params + i, depth);
        fprintf(stderr, "\n");
      }
      break;
    case EXPR_IF:
      fprintf(stderr, "%*sif ", depth, "");
      expr_print(expr->if_cond, depth);
      fprintf(stderr, "\n%*s  then ", depth, "");
      expr_print(expr->if_, depth);
      fprintf(stderr, "\n%*s  else ", depth, "");
      expr_print(expr->else_, depth);
      fprintf(stderr, "\n");
      break;
    case EXPR_LET:
      fprintf(stderr, "%*slet ", depth, "");
      symbol_table_print(expr->let_table, depth + INDENTATION);
      expr_print(expr->let_body, depth);
      fprintf(stderr, "\n");
      break;
    case EXPR_MATCH:
      fprintf(stderr, "%*smatch ", depth, "");
      expr_print(expr->match_cond, 0);
      for (int i = 0; i < expr->num_pats; i++) {
        fprintf(stderr, "%*s - ", depth, "");
        expr_print(expr->match_pats + i, depth + INDENTATION);
        fprintf(stderr, "%*s   ", depth, "");
        expr_print(expr->match_bodies + i, depth + INDENTATION);
        fprintf(stderr, "\n");
      }
      break;
    case EXPR_PROGN:
      fprintf(stderr, "%*sprogn\n", depth, "");
      for (int i = 0; i < expr->num_exprs; i++) {
        fprintf(stderr, "%*s - ", depth, "");
        expr_print(expr->exprs + i, depth);
        fprintf(stderr, "\n");
      }
      break;
    case EXPR_SWITCH:
      fprintf(stderr, "%*s switch\n", depth, "");
      break;
    case EXPR_VAR:
      fprintf(stderr, "%*svar ", depth, "");
      val_print(expr->var_name, expr->var, 0);
      fprintf(stderr, "\n");
      break;
    //default:
    //  fprintf(stderr, "%*s expr unknown", depth, "");
    //  break;
  }
}

void ast_print (module_t* mod, int depth) {
  for (int i = 0; i < mod->table.num_symbols; i++) {
    if (mod->table.values[i].type->meta == TYPE_FUNC) {
      fprintf(stderr, "%*sdefn %s\n", depth, "", mod->table.names[i]);
      if (mod->table.values[i].body != NULL) {
        expr_print(mod->table.values[i].body, depth + INDENTATION);
      }
    }
  }
}
