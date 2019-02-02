#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "types.h"

#define NUM_TYPES 512
static type_t types[NUM_TYPES];
static int i_type = 0;

static char HOLE_NAMES[52];

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


type_t* type_sum_constructor (type_t* field, type_t* sum) {
  if (field == NULL) {
    return sum;
  }
  type_t* type = type_new();
  type->meta = TYPE_FUNC;
  type->field_names = NULL;

  // Assumes each sum field has only one value, either a primitive or struct
  type->num_fields = 2;
  type->fields = malloc(type->num_fields * sizeof(type_t*));
  type->fields[0] = field;
  type->fields[1] = sum;
  if (field->meta == TYPE_HOLE) {
    type->num_args = 1;
    type->args = field->name;
  } else {
    type->num_args = 0;
    type->args = NULL;
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
  return 0;
}

type_t* type_product_constructor (type_t* product) {
  type_t* type = type_new();
  type->name = product->name;
  type->meta = TYPE_FUNC;
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
  type->field_names = NULL;
  type->num_fields = 2;
  type->fields = malloc(type->num_fields * sizeof(type_t*));
  type->fields[0] = product;
  type->fields[1] = field;
  return type;
}

/*
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
*/

type_t* type_instance (type_t* template, type_t** args, type_t* instance) {
  instance->name = template->name;
  instance->meta = template->meta;
  instance->num_fields = template->num_fields;
  instance->fields = malloc(instance->num_fields * sizeof(type_t*));
  instance->field_names = (template->field_names == NULL) ? NULL : malloc(instance->num_fields * sizeof(char*));
  instance->num_args = 0;
  instance->args = NULL;

  int num_args_applied = 0;
  for (int i = 0; i < instance->num_fields; i++) {
    if (template->field_names != NULL) {
      instance->field_names[i] = template->field_names[i];
    }
    if (template->fields[i] == NULL || template->fields[i]->meta != TYPE_HOLE) {
      instance->fields[i] = template->fields[i];
      continue;
    }

    // find the arg that applies to this field
    int j;
    for (j = 0; j < template->num_args; j++) {
      if (template->fields[i]->name[0] == template->args[j]) {
        break;
      }
    }
    assert(j < template->num_args);
    instance->fields[i] = args[j];
    num_args_applied++;
  }
  assert(num_args_applied == template->num_args);
  return instance;
}

/*
type_t* type_eval_constructor (type_t* constructor, type_t** arg_types, int num_args) {
  // first find mapping between args for the constructor and args for the type
  char* arg_names = malloc(constructor->num_fields * sizeof(char));
  for (int i = 0; i < constructor->num_fields; i++) {
    arg_names[i] = constructor->field_names[i][0];
    assert(constructor->field_names[i][1] == '\0');
  }

  return type_apply_template(constructor->fields[constructor->num_fields - 1], arg_names, arg_types, num_args);
}
*/

int type_fields_match (type_t** type_fields, type_t** arg_fields, int num_fields) {
  for (int i = 0; i < num_fields; i++) {
    if (type_fields[i] != arg_fields[i]) {
      return 0;
    }
  }
  return 1;
}

/*
type_t* type_instance_by_names (type_t* template, char* arg_names, type_t** arg_types, int num_args) {
  int* holes = malloc(num_args * sizeof(int));
  int j = 0;
  for (int i = 0; i < template->num_fields; i++) {
    if (template->fields[i]->meta == TYPE_HOLE) {
      assert(j < num_args);
      holes[j] = i;
      j++;
    }
  }

  for (int i = 0; i < i_type_instance; i++) {
    if (strcmp(type_instances[i].name, template->name) == 0) {
      for (int j = 0; j < num_args; j++) {
        if (type_instances[i].fields[holes[j]] == arg_types[j]) {
          return type_instances + i;
        }
      }
    }
  }

  return type_apply_template(template, NULL, arg_types, num_args);
}
*/

type_t** type_fn_get_return_args (type_t* fn, type_t** args) {
  type_t* ret = fn->fields[fn->num_fields - 1];

  // find mapping between args for the constructor and args for the type
  type_t** ret_args = malloc(ret->num_args * sizeof(type_t*));
  int num_args_found = 0;
  for (int i = 0; i < fn->num_fields - 1; i++) {
    if (fn->fields[i]->meta == TYPE_HOLE) {
      for (int j = 0; j < ret->num_args; j++) {
        if (fn->fields[i]->name[0] == ret->args[j]) {
          ret_args[j] = args[i];
          num_args_found++;
        }
      }
    }
  }

  assert(num_args_found == ret->num_args);
  return ret_args;
}

/*
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
*/

int type_eq (type_t* a, type_t* b) {
  if (a == b) {
    return 1;
  }

  if (a == NULL || b == NULL) {
    return 0;
  }

  if (a->meta == TYPE_HOLE || b->meta == TYPE_HOLE) {
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

  return 1;
}

int type_count_holes (type_t* t) {
  int count = 0;
  for (int i = 0; i < t->num_fields; i++) {
    if (t->fields[i] != NULL && t->fields[i]->meta == TYPE_HOLE) {
      count++;
    }
  }
  return count;
}

int type_has_holes (type_t* t) {
  for (int i = 0; i < t->num_fields; i++) {
    if (t->fields[i] != NULL && t->fields[i]->meta == TYPE_HOLE) {
      return 1;
    }
  }
  return 0;
}

type_t* type_get_hole_builtin (char c) {
  assert(islower(c));
  return TYPE_HOLES + (c - 'a');
}

int type_builtins (type_t** types) {
  int num_types = 26 + 3;
  type_t type;
  *types = type_new_i(num_types);

  TYPE_HOLES = *types;
  int i, j;
  for (i = 0; i < 26; i++) {
    j = i * 2;
    HOLE_NAMES[j] = 'a' + i;
    HOLE_NAMES[j + 1] = '\0';
    type = (type_t) { .name = HOLE_NAMES + j, .meta = TYPE_HOLE, .num_fields = 0 };
    memcpy(TYPE_HOLES + i, &type, sizeof(type_t));
  }

  TYPE_I8 = *types + i;
  type = (type_t) { .name = (char*)"I8",   .meta = TYPE_PRIM,  .num_fields = 0 };
  memcpy(TYPE_I8, &type, sizeof(type_t));
  i++;

  TYPE_I32 = *types + i;
  type = (type_t) { .name = (char*)"I32",  .meta = TYPE_PRIM,  .num_fields = 0 };
  memcpy(TYPE_I32, &type, sizeof(type_t));
  i++;

  TYPE_PTR = *types + i;
  type = (type_t) {
    .name = (char*)"Ptr",
     .meta = TYPE_PARAM,
     .num_fields = 1,
     .fields = &TYPE_HOLES,
     .num_args = 1,
     .args = (char*)"a"
  };
  memcpy(TYPE_PTR, &type, sizeof(type_t));

  return num_types;
}

void type_print (type_t* type) {
  if (type == NULL) {
    return;
  }

  int i;

  switch (type->meta) {
    case TYPE_PRIM:
    case TYPE_HOLE:
    case TYPE_OPAQUE:
      fprintf(stderr, "%s", type->name);
      break;
    case TYPE_FUNC:
      for (i = 0; i < type->num_fields - 1; i++) {
        fprintf(stderr, " ");
        type_print(type->fields[i]);
        fprintf(stderr, " ->");
      }
      fprintf(stderr, " ");
      type_print(type->fields[i]);
      break;
    case TYPE_PARAM:
      fprintf(stderr, "(%s", type->name);
      for (i = 0; i < type->num_fields; i++) {
        fprintf(stderr, " %s", type->fields[i]->name);
      }
      fprintf(stderr, ")");
      break;
    case TYPE_PRODUCT:
      fprintf(stderr, "{ ");
      for (i = 0; i < type->num_fields; i++) {
        if (i != 0) {
          fprintf(stderr, ", ");
        }
        fprintf(stderr, "%s: ", type->field_names[i]);
        type_print(type->fields[i]);
      }
      fprintf(stderr, " }");
      break;
    case TYPE_SUM:
      fprintf(stderr, "%s { ", type->name);
      for (i = 0; i < type->num_fields; i++) {
        if (i != 0) {
          fprintf(stderr, " | ");
        }
        fprintf(stderr, "%s", type->field_names[i]);
        if (type->fields[i] != NULL) {
          fprintf(stderr, " :: ");
          type_print(type->fields[i]);
        }
      }
      fprintf(stderr, " }");
      break;
    default:
      fprintf(stderr, "?%d?", type->meta);
      break;
  }
}
