#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "lexer.h"
#include "parser.h"

#define NUM_TYPES 512
static type_t types[NUM_TYPES];
static int i_type = 0;

type_t* type_new () {
  assert(i_type < NUM_TYPES);
  return types + i_type++;
}

type_t* type_new_i (int i) {
  assert(i_type + i - 1 < NUM_TYPES);
  type_t* start = types + i_type;
  i_type += i;
  return start;
}

int parse_type_func (module_t* mod, type_t* type, node_t* node);
int parse_type_sum (module_t* mod, type_t* type, node_t* node);
int parse_type_product (module_t* mod, type_t* type, node_t* node);
int type_matches_node (type_t* type, node_t* node);
type_t* type_find (module_t* mod, node_t* node);
type_t* module_type_find (module_t* mod, char* name, int* i0, int* j0, int* k0);


/*
int parse_deftype (module_t* mod, node_t* node, type_t* type) {
  node_t* name_node = node->list->fst->next;
  if (name_node->type == NODE_ATOM) {
    type->name = name_node->atom->name;
    type->is_template = 0;
    return parse_type(module, name_node->next, type);
  } else if (name_node->type == NODE_LIST && name_node->list->fst->type == NODE_ATOM) {
    type->num_fields = name_node->list->len;
    name_node = name_node->list->fst;
    type->name = name_node->atom->name;
    type->is_template = 1;
    type->meta == TYPE_FUNC;
    type->fields = malloc(type->num_fields * sizeof(type_t*));
    type->field_names = malloc(type->num_fields * sizeof(char*));
    int i;
    for (i = 0; i < type->num_fields - 1; i++) {
      name_node = name_node->next;
      if (name_node->type != NODE_ATOM) {
        fprintf(stderr, "invalid template type\n");
        return 1;
      }
      type->field_names[i] = name_node->atom->name;
      type->fields[i] = TYPE_POLY;
    }
    type->fields[i] = type_new();
    return parse_type(mod, node->list->fst->next, type->fields + i);
  } else {
    fprintf(stderr, "invalid type name\n");
    return 1;
  }
}
*/

int parse_type (module_t* mod, node_t* node, type_t* type) {
  type->field_names = NULL;
  switch (node->type) {
    case NODE_ATOM:
      type->num_fields = 1;
      type_t* field = type_find(mod, node);
      if (field == NULL) {
        fprintf(stderr, "type %s not found\n", node->atom->name);
        return 1;
      }
      type->fields = &field;
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

int parse_type_internal (module_t* mod, node_t* node, type_t** type) {
  if (node->type == NODE_ATOM && islower(node->atom->name[0])) {
    *type = TYPE_POLY;
    return 0;
  }

  *type = type_new();
  return parse_type(mod, node, *type);
}


int parse_type_func (module_t* mod, type_t* type, node_t* node) {
  type_t* field;
  for (int i = 0; i < type->num_fields; i++) {
    field = type_find(mod, node);
    if (field == NULL) {
      fprintf(stderr, "type not found\n");
      return 1;
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
          if (parse_type_internal(mod, sub_node, &field) != 0) {
            return 1;
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

int type_matches_node (type_t* type, node_t* node) {
  switch (node->type) {
    case NODE_ATOM:
      return type->meta != TYPE_PARAM && strcmp(type->name, node->atom->name) == 0;
    case NODE_LIST:
      if (type->meta != TYPE_PARAM || type->num_fields != node->list->len - 1) {
        return 0;
      }
      node = node->list->fst;
      if (node->type != NODE_ATOM) {
        fprintf(stderr, "could not determine type name\n"); // this should never happen
        return 0;
      }
      if (strcmp(type->name, node->atom->name) != 0) {
        return 0;
      }
      for (int i = 1; i < type->num_fields; i++) {
        node = node->next;
        if (!type_matches_node(type->fields[i], node)) {
          return 0;
        }
      }
      return 1;
  }
}

type_t* type_find (module_t* mod, node_t* node) {
  int i = 0, j = 0, k = 0;
  node_t* sub_node;
  switch (node->type) {
    case NODE_ATOM:
      return module_type_find(mod, node->atom->name, &i, &j, &k);
    case NODE_LIST:
      sub_node = node->list->fst;
      if (sub_node->type != NODE_ATOM) {
        fprintf(stderr, "could not determine type name\n");
        return NULL;
      }
      char* name = sub_node->atom->name;
      type_t* param_type = NULL;
      type_t* type = module_type_find(mod, name, &i, &j, &k);
      while (type != NULL) {
        // if the type is parametrized, save it for later to create an instance
        if (type->is_template) {
          param_type = type;
        } else if (type_matches_node(type, node)) {
          return type;
        }
        type = module_type_find(mod, name, &i, &j, &k);
      }

      if (param_type != NULL) {
        for (int l = 1; l < node->list->len; l++) {
          sub_node = sub_node->next;
          if (sub_node->type == NODE_ATOM && strlen(sub_node->atom->name) == 1 && islower(sub_node->atom->name[0])) {
            return param_type;
          }
        }
        //return type_add_instance(mod, param_type, node);
      }

      return NULL;
  }
}

type_t* module_type_add (module_t* mod, type_t* type) {
  if (mod->max_types == 0) {
    fprintf(stderr, "module_type_add called on module with no types\n");
    abort();
  }
  if (mod->num_types == mod->max_types) {
    mod->max_types = mod->max_types * 2;
    type_t* types = malloc(sizeof(type_t) * mod->max_types);
    memcpy(types, mod->types, sizeof(type_t) * mod->num_types);
    free(mod->types);
    mod->types = types;
  }
  type_t* copy = mod->types + mod->num_types;
  mod->types[mod->num_types] = *type;
  mod->num_types++;
  return copy;
}

type_t* module_type_find (module_t* mod, char* name, int* i0, int* j0, int* k0) {
  for (int i = *i0; i < mod->num_types; i++) {
    if (mod->types[i].name != NULL && strcmp(mod->types[i].name, name) == 0) {
      *i0 = i + 1;
      return mod->types + i;
    }
  }
  *i0 = mod->num_types;

  if (*j0 < 0) {
    return NULL;
  }

  type_t* type;

  int no_deps = -1;
  for (int j = *j0; j < mod->num_deps; j++) {
    type = module_type_find(mod->deps[j], name, k0, &no_deps, &no_deps);
    if (type != NULL) {
      *j0 = j;
      return type;
    }
    *k0 = 0;
  }
  *j0 = mod->num_deps;

  return NULL;
}

type_t* type_sum_constructor (type_t* field, type_t* sum) {
  if (field == NULL) {
    return sum;
  }
  type_t* type = type_new();
  type->meta = TYPE_FUNC;
  type->is_template = sum->is_template;
  type->field_names = NULL;
  if (field == TYPE_POLY) {
    type->num_fields = 2;
    type->fields = malloc(type->num_fields * sizeof(type_t*));
    type->fields[0] = TYPE_POLY;
    type->fields[1] = sum;
  } else {
    type->num_fields = field->num_fields + 1;
    type->fields = malloc(type->num_fields * sizeof(type_t*));
    int i;
    for (i = 0; i < type->num_fields - 1; i++) {
      type->fields[i] = field->fields[i];
    }
    type->fields[i] = sum;
  }
  return type;
}

int type_sum_index (type_t* sum, char* field_name) {
  assert(sum->meta == TYPE_SUM);
  for (int i = 0; i < sum->num_fields; i++) {
    if (strcmp(sum->field_names[i], field_name) == 0) {
      return i;
    }
  }
  fprintf(stderr, "unknown sum type field %s\n", field_name);
  assert(0);
}

type_t* type_product_constructor (type_t* product) {
  type_t* type = type_new();
  type->name = product->name;
  type->meta = TYPE_FUNC;
  type->is_template = product->is_template;
  type->field_names = NULL;
  type->num_fields = product->num_fields + 1;
  type->fields = malloc(type->num_fields * sizeof(type_t*));
  int i;
  for (i = 0; i < product->num_fields; i++) {
    type->fields[i] = product->fields[i];
  }
  type->fields[i] = product;
  return type;
}

type_t* type_product_getter (type_t* field, type_t* product) {
  type_t* type = type_new();
  type->meta = TYPE_FUNC;
  type->is_template = product->is_template;
  type->field_names = NULL;
  type->num_fields = 2;
  type->fields = malloc(type->num_fields * sizeof(type_t*));
  type->fields[0] = product;
  type->fields[1] = field;
  return type;
}

int type_add_constructors (module_t* mod, type_t* type) {
  val_t val = {};
  switch (type->meta) {
    case TYPE_SUM:
      for (int i = 0; i < type->num_fields; i++) {
        val.type = type_sum_constructor(type->fields[i], type);
        symbol_table_add(&mod->table, type->field_names[i], &val);
      }
      return 0;
    case TYPE_PRODUCT:
      val.type = type_product_constructor(type);
      symbol_table_add(&mod->table, type->name, &val);
      for (int i = 0; i < type->num_fields; i++) {
        val.type = type_product_getter(type->fields[i], type);
        val.type->name = type->field_names[i];
        symbol_table_add(&mod->table, type->field_names[i], &val);
      }
      return 0;
    case TYPE_ALIAS:
    case TYPE_PRIM:
    case TYPE_FUNC:
    case TYPE_PARAM:
      return 0;
  }
}

int type_add_instance (module_t* mod, type_t* src, type_t* desired) {
  if (src->meta == TYPE_FUNC) {
    type_t* poly; // TODO list
    for (int i = 0; i < src->num_fields - 1; i++) { // ignore return value
      if (src->fields[i] == TYPE_POLY) {
        poly = desired->fields[i];
        break;
      }
    }
    // desired
    return type_add_instance(mod, src->fields[src->num_fields - 1]);
  }

  type_t instance = { .name = src->name, .meta = src->meta, .num_fields = src->num_fields };
  instance.fields = malloc(instance.num_fields * sizeof(type_t*));
  for (int i = 0; i < instance.num_fields; i++) {
    if (src->fields[i] != TYPE_POLY) {
      instance.fields[i] = src->fields[i];
    } else if (desired->fields[i] != TYPE_POLY) {
      instance.fields[i] = desired->fields[i];
    } else {
      fprintf(stderr, "source and desired type for field %d are indeterminate\n", i);
      return 1;
    }
  }
  type_t* new = module_type_add(mod, &instance);
  return type_add_constructors(mod, new);
}

int type_add_instance_constructor (module_t* mod, type_t* src, type_t* desired) {
  if (src->meta != TYPE_FUNC) {
    fprintf(stderr, "cannot add instance constructor of non-function\n");
    return 1;
  }

  type_t* poly; // TODO list
  int i;
  for (i = 0; i < src->num_fields - 1; i++) { // ignore return value
    if (src->fields[i] == TYPE_POLY) {
      poly = desired->fields[i];
      break;
    }
  }

  return type_add_instance(mod, src->fields[src->num_fields - 1], );
}

int type_eq (type_t* a, type_t* b) {
  if (a == b) {
    return 1;
  }

  if (a == NULL || b == NULL) {
    return 0;
  }

  if (a == TYPE_POLY || b == TYPE_POLY) {
    return 2;
  }

  if (a->meta != b->meta || a->num_fields != b->num_fields) {
    return 0;
  }

  for (int i = 0; i < a->num_fields; i++) {
    if (type_eq(a->fields[i], b->fields[i]) == 0) {
      return 0;
    }
  }

  return (a->is_template || b->is_template) ? 2 : 1;
}
