#ifndef	_TYPES_H
#define	_TYPES_H

#include <llvm-c/Core.h>

typedef enum {
  TYPE_ALIAS,
  TYPE_HOLE,
  TYPE_FUNC,
  TYPE_PARAM,
  TYPE_PRIM,
  TYPE_PRODUCT,
  TYPE_SUM
} meta_type_t;

struct type_t;
typedef struct type_t {
  char* name;
  meta_type_t meta;
  int num_fields;
  struct type_t** fields;
  char** field_names; // for sum or product types
  int num_args;
  char* args; // for type templates
  LLVMTypeRef llvm; // populated during compilation
} type_t;

type_t* TYPE_HOLES;
type_t* TYPE_I8;
type_t* TYPE_I32;
type_t* TYPE_PTR;
type_t* TYPE_CSTR;

type_t* type_new_i (int i);
type_t* type_new ();

int type_matches_node (type_t* type, node_t* node);
int type_sum_index (type_t* sum, char* field_name);
type_t* type_sum_constructor (type_t* field, type_t* sum);
type_t* type_product_constructor (type_t* product);
type_t* type_product_getter (type_t* field, type_t* product);

type_t* type_instance (type_t* template, type_t** args, type_t* instance);
int type_fields_match (type_t** type_fields, type_t** arg_fields, int num_fields);
type_t** type_fn_get_return_args (type_t* fn_type, type_t** arg_types);
int type_eq (type_t* a, type_t* b);
int type_has_holes (type_t* t);
type_t* type_get_hole (char c);
type_t* type_get_hole_builtin (char c);
int type_builtins (type_t** types);
void type_print (type_t* type);

#endif
