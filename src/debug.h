#ifndef	_DEBUG_H
#define	_DEBUG_H

#include "lexer.h"
#include "parser.h"

const char* atom_print_type (atom_type_t type);
void node_print (node_t* node, int depth);
void type_print (type_t* type);
void val_print (char* name, val_t* val, int depth);
void symbol_table_print (symbol_table_t* table, int depth);
void module_print (module_t* module);

#endif
