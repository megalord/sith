#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

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

int parse_type (module_t* mod, node_t* node, type_t* type) {
  type->is_template = 0;
  if (node->type == NODE_LIST) {
    type->is_template = 1;
    // do something with parameter names
  }
  node = node->next;

  type->field_names = NULL;
  switch (node->type) {
    case NODE_ATOM:
      type->meta = TYPE_ALIAS;
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
        field = type_new();
        field->name = sub_node->atom->name;
        field->meta = TYPE_FUNC;
        field->is_template = type->is_template;
        field->num_fields = node->list->len;
        field->field_names = NULL;
        field->fields = malloc(field->num_fields * sizeof(type_t*));
        int j;
        for (j = 0; j < field->num_fields - 1; j++) {
          sub_node = sub_node->next;
          if (sub_node->type != NODE_ATOM) {
            fprintf(stderr, "invalid sum type\n");
            return 1;
          }
          if (strlen(sub_node->atom->name) == 1 && islower(sub_node->atom->name[0])) {
            // TODO: lookup name
            field->fields[j] = TYPE_POLY;
          } else {
            field->fields[j] = type_find(mod, sub_node);
            if (field->fields[i] == NULL) {
              fprintf(stderr, "type %s not found\n", sub_node->atom->name);
              return 1;
            }
          }
        }
        field->fields[j] = type;
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

type_t* module_type_new (module_t* mod) {
  // realloc mod->types
  return NULL;
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
  type->name = field->name;
  type->meta = TYPE_FUNC;
  type->is_template = 0;
  type->field_names = NULL;
  type->num_fields = field->num_fields + 1;
  type->fields = malloc(field->num_fields * sizeof(type_t*));
  int i;
  for (i = 0; i < sum->num_fields; i++) {
    type->fields[i] = field->fields[i];
  }
  type->fields[i] = sum;
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
  type->is_template = 0;
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
  type->is_template = 0;
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
        val.type = (type->fields[i] == NULL) ? type : type->fields[i];
        //val.type = type_sum_constructor(type->fields[i], type);
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

/*
type_t* type_add_instance (module_t* mod, type_t* src, type_t* fields) {
  type_t* instance = module_type_new(mod);
  instance->name = src->name;
  instance->meta = src->meta;
  instance->num_fields = src->num_fields;
  for (int i = 0; i < src->num_fields; i++) {
    instance->fields[i] = *src->fields + i;
  }
  instance->fields = &fields;
  return instance;
}
*/
