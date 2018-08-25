#include <assert.h>
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
int type_matches_node (type_t* type, node_t* node);
type_t* type_find (module_t* mod, node_t* node);
type_t* module_type_find (module_t* mod, char* name, int* i0, int* j0, int* k0);

int parse_type (module_t* mod, node_t* node, type_t* type) {
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
      return 0;
    case NODE_LIST:
      type->num_fields = node->list->len - 1;
      type->fields = malloc(type->num_fields * sizeof(type_t*));
      node = node->list->fst;
      if (node->type != NODE_ATOM) {
        fprintf(stderr, "invalid signature\n");
        return 1;
      }
      if (strcmp(node->atom->name, "->") == 0) {
        type->meta = TYPE_FUNC;
        return parse_type_func(mod, type, node->next);
      } else if (strcmp(node->atom->name, "+") == 0) {
        type->meta = TYPE_SUM;
        fprintf(stderr, "sum types not supported\n");
        return 1;
      } else if (strcmp(node->atom->name, "*") == 0) {
        type->meta = TYPE_PRODUCT;
        fprintf(stderr, "product types not supported\n");
        return 1;
      } else {
        type->meta = TYPE_PARAM;
        type->name = node->atom->name;
        fprintf(stderr, "param types not supported\n");
        return 1;
      }
  }
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

int type_matches_node (type_t* type, node_t* node) {
  switch (node->type) {
    case NODE_ATOM:
      return type->meta != TYPE_PARAM && !strcmp(type->name, node->atom->name);
    case NODE_LIST:
      if (type->meta != TYPE_PARAM || type->fields == NULL || type->num_fields != node->list->len - 1) {
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
  node_t* fst;
  switch (node->type) {
    case NODE_ATOM:
      return module_type_find(mod, node->atom->name, &i, &j, &k);
    case NODE_LIST:
      fst = node->list->fst;
      if (fst->type != NODE_ATOM) {
        fprintf(stderr, "could not determine type name\n");
        return NULL;
      }
      char* name = fst->atom->name;
      type_t* param_type = NULL;
      type_t* type = module_type_find(mod, name, &i, &j, &k);
      while (type != NULL) {
        if (type_matches_node(type, node)) {
          return type;
        }
        // if the type doesn't fully match but it's parametrized, save it for later to create an instance
        if (type->meta == TYPE_PARAM && type->fields == NULL && type->num_fields == node->list->len - 1) {
          param_type = type;
        }
        type = module_type_find(mod, name, &i, &j, &k);
      }

      //if (param_type != NULL) {
      //  return type_add_instance(mod, param_type, node);
      //}

      return NULL;
  }
}

type_t* module_type_new (module_t* mod) {
  // realloc mod->types
  return NULL;
}

type_t* module_type_find (module_t* mod, char* name, int* i0, int* j0, int* k0) {
  for (int i = *i0; i < mod->num_types; i++) {
    if (strcmp(mod->types[i].name, name) == 0) {
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
  }
  *j0 = mod->num_deps;

  return NULL;
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
