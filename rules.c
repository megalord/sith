#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"

typedef struct {
  char* name;
  atom_type_t type;
  int min_nodes;
  int max_nodes;
  node_type_t* node_types;
} rule_t;

rule_t if_rule = { .name = "if", .type = ATOM_IF, .min_nodes = 2, .max_nodes = 3 };
//rule_if.node_types = malloc(3 * sizeof(node_type_t*));
//rule_if.node_types[0] = NODE_LIST;
//rule_if.node_types[1] = NODE_LIST;
//rule_if.node_types[2] = NODE_LIST;

rule_t rules[] = { if_rule };

