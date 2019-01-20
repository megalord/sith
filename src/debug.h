#ifndef	_DEBUG_H
#define	_DEBUG_H

#include "lexer.h"
#include "parser.h"
#include "types.h"

const char* atom_print_type (atom_type_t type);
void node_print (node_t* node, int depth);
void type_print (type_t* type);
void val_print (char* name, val_t* val, int depth);
void symbol_table_print (symbol_table_t* table, int depth);
void module_print (module_t* module, int depth);
void module_print_follow (module_t* module, int depth);
void expr_print (expr_t* expr, int depth);
void ast_print (module_t* mod, int depth);

#endif
