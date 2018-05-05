#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "version.h"
#include "lexer.c"
//#include "jit.c"
#include "module.c"
#include "debug.c"

int str_includes (char* input, char* test) {
  int len = strlen(test);
  if (len == 0) {
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
  for (int i = 0; i < len; i = i + 1) {
    if (fst[i] != test[i]) {
      return 0;
    }
  }
  return  1;
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

/*int parse (char* filename, sj_module_t* module) {
  if (parse_filename(filename, &module->name) != 0) {
    return 1;
  }
  node_t root;
  node_from_file(filename, &root);
  return parse_module(&root, module);
}*/

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
  if (parse_filename(filename, &module.name) != 0) {
    return 1;
  }

  node_t root;
  node_from_file(filename, &root);
  if (strcmp(argv[1], "read") == 0) {
    node_print(&root, 0);
    return 0;
  }

  if (module_compile(&root, &module) != 0) {
    return 1;
  }

  if (strcmp(argv[1], "parse") == 0) {
    module_print(&module);
    return 0;
  }
  //int result = exec_main(&module);
  //printf("exited with status %u\n", result);
  return  0;
}
