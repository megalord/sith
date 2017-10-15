#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "version.h"
#include "lexer.c"
int print_type (FILE* fp, symbol_t* symbol) {
  if (symbol->type != SYMBOL_VAR) {
    fprintf(stderr, "cannot print function types\n");
    return 1;
  };
  fprintf(fp, "%s", symbol->def.var);
  return  0;
}
int str_includes (char* input, char* test) {
  if (strlen(test) == 0) {
    return 1;
  } else {
    if (strlen(input) == 0) {
      return 0;
    };
  };
  char* fst = strchr(input, test[0]);
  if (fst == NULL) {
    return 0;
  };
  for (int i = 0; i < strlen(test); i = i + 1) {
    if (fst[i] != test[i]) {
      return 0;
    };
  };
  return  1;
}
void print_symbol (symbol_t* symbol, int depth) {
  if (symbol->type == SYMBOL_VAR) {
    printf("%*s", depth, "");
    print_type(stdout, symbol);
  } else {
    if (symbol->type == SYMBOL_FUNC) {
      func_t fn = *symbol->def.fn;
      printf("%*s", depth, "");
      printf(" %s_%d ::", symbol->name, fn.arity);
      for (int i = 0; i < fn.arity; i = i + 1) {
        print_symbol(&fn.args[i], 1);
        printf(" ->");
      };
      print_symbol(fn.ret, 1);
    };
  };
}
void print_module (module_t* module) {
  printf("--------------------------\n");
  printf("MODULE %s\n", module->name);
  printf("requires:\n");
  for (int i = 0; i < module->num_deps; i = i + 1) {
    printf(" - %s\n", module->deps[i].name);
  };
  printf("defines types:\n");
  for (int i = 0; i < module->table.num_types; i = i + 1) {
    printf(" - %s:\n", module->table.types[i].name);
    print_node(module->table.types[i].def, 2);
    printf("\n");
  };
  printf("defines symbols:\n");
  for (int i = 0; i < module->table.len; i = i + 1) {
    printf(" - ");
    print_symbol(&module->table.symbols[i], 0);
    printf("\n");
  };
  printf("code: %d\n", module->code.len);
  node_t* node = module->code.fst;
  for (int i = 0; i < module->code.len; i = i + 1) {
    print_node(node, 2);
    node = node->next;
  };
  printf("\n");
  for (int i = 0; i < module->num_deps; i = i + 1) {
    print_module(&module->deps[i]);
  };
}
int parse_filename (char* filename, char** basename) {
  char* ext = strchr(filename, '.');
  if (ext == NULL) {
    fprintf(stderr, "invalid file name\n");
    return 1;
  };
  int filename_len = ext - filename;
  *basename = malloc(filename_len + 1);
  strncpy(*basename, filename, filename_len);
  char* tmp = *basename;
  tmp[filename_len] = '\0';
  return  0;
}
int parse_sexpr (char* filename, node_t* root) {
  root->type = NODE_LIST;
  root->list = malloc(sizeof(list_t));
  root->list->len = 0;
  root->list->fst = NULL;
  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    perror("Error reading file");
    return 1;
  };
  stream_t stream;
  stream.source = f;
  stream.next = -2;
  read_empty(&stream);
  node_t* node;
  node_t* prev = NULL;
  while (1) {
    node = malloc(sizeof(node_t));
    node->next = NULL;
    if (read_node(&stream, node) != 0) {
      free(node);
      break;;
    };
    if (prev == NULL) {
      root->list->fst = node;
    } else {
      prev->next = node;
    };
    root->list->len = root->list->len + 1;
    prev = node;
  };
  fclose(f);
  return  0;
}
int parse_type (node_t* node, symbol_t* symbol) {
  if (node->type == NODE_LIST) {
    symbol->type = SYMBOL_FUNC;
    symbol->def.fn = malloc(sizeof(func_t));
    symbol->def.fn->arity = node->list->len - 2;
    symbol->def.fn->args = malloc(symbol->def.fn->arity * sizeof(symbol_t));
    node = node->list->fst;
    if (node->type != NODE_ATOM || strcmp(node->atom->name, "->") != 0) {
      fprintf(stderr, "only func and var types supported\n");
      return 1;
    };
    node = node->next;
    symbol->def.fn->ret = malloc(sizeof(symbol_t));
    parse_type(node, symbol->def.fn->ret);
    node = node->next;
    for (int i = 0; i < symbol->def.fn->arity; i = i + 1) {
      parse_type(node, &symbol->def.fn->args[i]);
      node = node->next;
    };
  } else {
    symbol->type = SYMBOL_VAR;
    symbol->def.var = node->atom->name;
  };
  return  0;
}
int parse_module (node_t* root, module_t* module) {
  module->num_deps = 0;
  module->table.num_types = 0;
  module->table.len = 0;
  module->code.len = 0;
  if (root->type != NODE_LIST) {
    fprintf(stderr, "root node must be a list\n");
    return 1;
  };
  node_t* node = root->list->fst;
  for (int i = 0; i < root->list->len; i = i + 1) {
    if (node->type == NODE_LIST && node->list->fst->type == NODE_ATOM) {
      if (strcmp(node->list->fst->atom->name, "include") == 0) {
        module->num_deps = module->num_deps + 1;
      } else {
        if (strcmp(node->list->fst->atom->name, ":") == 0) {
          module->table.len = module->table.len + 1;
        } else {
          if (strcmp(node->list->fst->atom->name, "deftype") == 0) {
            module->table.num_types = module->table.num_types + 1;
          };
        };
      };
    };
    node = node->next;
  };
  module->deps = malloc(module->num_deps * sizeof(module_t));
  module->table.types = malloc(module->table.num_types * sizeof(module_t));
  module->table.symbols = malloc(module->table.len * sizeof(symbol_t));
  symbol_t* symbol;
  node = root->list->fst;
  node_t* prev = NULL;
  int i_dep = 0;
  int i_sym = 0;
  int i_type = 0;
  for (int i = 0; i < root->list->len; i = i + 1) {
    if (node->type != NODE_LIST) {
      fprintf(stderr, "sub-root node must be a list\n");
      return 1;
    };
    if (node->list->fst->type == NODE_ATOM) {
      char* name = node->list->fst->atom->name;
      if (strcmp(name, ":") == 0) {
        module->table.symbols[i_sym].name = node->list->fst->next->atom->name;
        parse_type(node->list->fst->next->next, &module->table.symbols[i_sym]);
        i_sym = i_sym + 1;
        free(node);
        node = node->next;
        continue;;
      };
      if (strcmp(name, "include") == 0) {
        char* filename = node->list->fst->next->atom->name;
        if (str_includes(filename, ".sith")) {
          if (parse(filename, &module->deps[i_dep]) != 0) {
            return 1;
          };
        } else {
          module->deps[i_dep].name = filename;
        };
        i_dep = i_dep + 1;
        free(node);
        node = node->next;
        continue;;
      };
      if (strcmp(name, "deftype") == 0) {
        module->table.types[i_type].name = node->list->fst->next->atom->name;
        module->table.types[i_type].def = node->list->fst->next->next;
        i_type = i_type + 1;
        free(node);
        node = node->next;
        continue;;
      };
    };
    if (prev == NULL) {
      module->code.fst = node;
    } else {
      prev->next = node;
    };
    module->code.len = module->code.len + 1;
    prev = node;
    node = node->next;
  };
  return  0;
}
int parse (char* filename, module_t* module) {
  if (parse_filename(filename, &module->name) != 0) {
    return 1;
  };
  node_t root;
  parse_sexpr(filename, &root);
  return  parse_module(&root, module);
}
void make_type (type_t* type, char* src, int src_len, char* dest, int dest_len) {
  type->name = malloc(src_len);
  strncpy(type->name, src, src_len);
  type->def = malloc(sizeof(node_t));
  type->def->type = NODE_ATOM;
  type->def->atom = malloc(sizeof(atom_t));
  type->def->atom->name = malloc(dest_len);
  strncpy(type->def->atom->name, dest, dest_len);
}
void init_global_table () {
  global_table.len = 0;
  global_table.num_types = 2;
  global_table.types = malloc(global_table.num_types * sizeof(type_t));
  make_type(&global_table.types[0], "Integer", sizeof("Integer"), "int", sizeof("int"));
  make_type(&global_table.types[1], "String", sizeof("String"), "char*", sizeof("char*"));
}
int main (int argc, char** argv) {
  if (argc == 1) {
    puts("sith compiler\n\nUsage:\n  sith [command]\n\nAvailable Commands:\n  build\n  parse\n  version");
    return 0;
  };
  if (strcmp(argv[1], "version") == 0) {
    puts(VERSION);
    return 0;
  };
  if (argc != 3) {
    fprintf(stderr, "wrong number of args\n");
    return 1;
  };
  char* filename = argv[2];
  if (strcmp(argv[1], "parse") == 0) {
    module_t module;
    if (parse(filename, &module) != 0) {
      return 1;
    };
    print_module(&module);
  } else {
    fprintf(stderr, "operation not supported\n");
    return 1;
  };
  return  0;
}
