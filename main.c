#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "version.h"
#include "lexer.c"
#include "jit.c"

symbol_t* table_get (symbol_table_t* table, char* name) {
  for (int i = 0; i < table->len; i++) {
    if (strcmp(table->symbols[i].name, name) == 0) {
      return &table->symbols[i];
    }
  }
  return NULL;
}

int print_type (FILE* fp, symbol_t* symbol) {
  if (symbol->is_fn) {
    fprintf(stderr, "cannot print function types\n");
    return 1;
  }
  fprintf(fp, "%s", atom_print_type(symbol->data.type));
  if (symbol->data.type == ATOM_IDENTIFIER) {
    fprintf(fp, " (%s)", symbol->name);
  }
  return 0;
}

int str_includes (char* input, char* test) {
  if (strlen(test) == 0) {
    return 1;
  } else {
    if (strlen(input) == 0) {
      return 0;
    }
  }
  char* fst = strchr(input, test[0]);
  if (fst == NULL) {
    return 0;
  }
  for (int i = 0; i < strlen(test); i = i + 1) {
    if (fst[i] != test[i]) {
      return 0;
    }
  }
  return  1;
}

void print_symbol (symbol_t* symbol, int depth) {
  if (symbol->is_fn) {
    printf("%*s", depth, "");
    printf(" %s_%d ::", symbol->name, symbol->data.fn->arity);
    for (int i = 0; i < symbol->data.fn->arity; i = i + 1) {
      print_symbol(&symbol->data.fn->args[i], 1);
      printf(" ->");
    }
    print_symbol(symbol->data.fn->ret, 1);
  } else {
    printf("%*s", depth, "");
    print_type(stdout, symbol);
  }
}

void print_module (module_t* module) {
  printf("--------------------------\n");
  printf("MODULE %s\n", module->name);
  printf("requires:\n");
  for (int i = 0; i < module->num_deps; i = i + 1) {
    printf(" - %s\n", module->deps[i].name);
  }
  printf("defines symbols:\n");
  for (int i = 0; i < module->table.len; i = i + 1) {
    printf(" - ");
    print_symbol(&module->table.symbols[i], 0);
    printf("\n");
  }
  printf("function definitions:\n");
  for (int i = 0; i < module->table.len; i = i + 1) {
    if (module->table.symbols[i].is_fn) {
      printf(" - %s\n", module->table.symbols[i].name);
      node_print(module->table.symbols[i].data.fn->body->fst, 0);
      printf("\n");
    }
  }
  for (int i = 0; i < module->num_deps; i = i + 1) {
    print_module(&module->deps[i]);
  }
}

int parse_filename (char* filename, char** basename) {
  char* ext = strchr(filename, '.');
  if (ext == NULL) {
    fprintf(stderr, "invalid file name\n");
    return 1;
  }
  int filename_len = ext - filename;
  *basename = malloc(filename_len + 1);
  strncpy(*basename, filename, filename_len);
  char* tmp = *basename;
  tmp[filename_len] = '\0';
  return  0;
}

int parse_type (node_t* node, symbol_t* symbol) {
  if (node->type == NODE_LIST) {
    symbol->is_fn = 1;
    func_t* fn = malloc(sizeof(func_t));
    fn->arity = node->list->len - 2;
    fn->args = malloc(fn->arity * sizeof(symbol_t));
    node = node->list->fst;
    if (node->type != NODE_ATOM || strcmp(node->atom->name, "->") != 0) {
      fprintf(stderr, "only func and var types supported\n");
      return 1;
    }
    node = node->next;
    fn->ret = malloc(sizeof(symbol_t));
    if (parse_type(node, fn->ret) != 0) {
      return 1;
    }
    node = node->next;
    for (int i = 0; i < fn->arity; i = i + 1) {
      if (parse_type(node, &fn->args[i]) != 0) {
        return 1;
      }
      node = node->next;
    }
    symbol->data.fn = fn;
  } else {
    symbol->is_fn = 0;
    symbol->name = node->atom->name;
    symbol->data.type = node->atom->type;
  }
  return  0;
}

int parse_module (node_t* root, module_t* module) {
  module->num_deps = 0;
  module->table.len = 0;
  if (root->type != NODE_LIST) {
    fprintf(stderr, "root node must be a list\n");
    return 1;
  }
  node_t* node = root->list->fst;
  for (int i = 0; i < root->list->len; i = i + 1) {
    if (node->type == NODE_LIST && node->list->fst->type == NODE_ATOM) {
      if (strcmp(node->list->fst->atom->name, "include") == 0) {
        module->num_deps = module->num_deps + 1;
      } else {
        if (strcmp(node->list->fst->atom->name, ":") == 0) {
          module->table.len = module->table.len + 1;
        }
      }
    }
    node = node->next;
  }
  module->deps = malloc(module->num_deps * sizeof(module_t));
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
    }
    if (node->list->fst->type == NODE_ATOM) {
      char* name = node->list->fst->atom->name;
      if (strcmp(name, ":") == 0) {
        module->table.symbols[i_sym].name = node->list->fst->next->atom->name;
        if (parse_type(node->list->fst->next->next, &module->table.symbols[i_sym]) != 0) {
          return 1;
        }
        i_sym = i_sym + 1;
        free(node);
        node = node->next;
        continue;
      }
      if (strcmp(name, "include") == 0) {
        char* filename = node->list->fst->next->atom->name;
        if (str_includes(filename, ".sith")) {
          if (parse(filename, &module->deps[i_dep]) != 0) {
            return 1;
          }
        } else {
          module->deps[i_dep].name = filename;
        }
        i_dep = i_dep + 1;
        free(node);
        node = node->next;
        continue;;
      }
      if (strcmp(name, "defun") == 0) {
        node_t* fn_node = node->list->fst->next;
        node_t* sig_node = fn_node->list->fst;
        fn_node = fn_node->next;
        symbol_t* sym = table_get(&module->table, sig_node->atom->name);
        if (sym == NULL) {
          fprintf(stderr, "symbol %s not found\n", node->list->fst->atom->name);
          return 1;
        }
        for (int i = 0; i < sym->data.fn->arity; i = i + 1) {
          sig_node = sig_node->next;
          sym->data.fn->args[i].name = sig_node->atom->name;
        }
        sym->data.fn->body = malloc(sizeof(list_t));
        sym->data.fn->body->len = node->list->len - 2;
        sym->data.fn->body->fst = fn_node;
        free(node);
        node = node->next;
        continue;;
      }
    }
    if (prev != NULL) {
      prev->next = node;
    }
    prev = node;
    node = node->next;
  }
  return  0;
}

int parse (char* filename, module_t* module) {
  if (parse_filename(filename, &module->name) != 0) {
    return 1;
  }
  node_t root;
  node_from_file(filename, &root);
  return  parse_module(&root, module);
}

int main (int argc, char** argv) {
  if (argc == 1) {
    puts("sith compiler\n\nUsage:\n  sith [command]\n\nAvailable Commands:\n  build\n  parse\n  read\n  version");
    return 0;
  }
  if (strcmp(argv[1], "version") == 0) {
    puts(VERSION);
    return 0;
  }
  if (argc != 3) {
    fprintf(stderr, "wrong number of args\n");
    return 1;
  }
  char* filename = argv[2];
  module_t module;
  node_t root;
  if (parse_filename(filename, &module.name) != 0) {
    return 1;
  }
  node_from_file(filename, &root);
  if (strcmp(argv[1], "read") == 0) {
    node_print(&root, 0);
    return 0;
  }
  parse_module(&root, &module);
  if (strcmp(argv[1], "parse") == 0) {
    print_module(&module);
    return 0;
  }
  int result = compile_root(&module);
  printf("exited with status %u\n", result);
  return  0;
}
