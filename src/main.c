#include <stdio.h>
#include <string.h>

#include "codegen.h"
#include "debug.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"
#include "version.h"

int main (int argc, char** argv) {
  if (argc == 1) {
    puts("sith compiler\n\nUsage:\n  sith [command]\n\nAvailable Commands:\n  build\n  parse\n  read\n  run\n  version");
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
  char* mod_name;
  if (parse_filename(filename, &mod_name) != 0) {
    return 1;
  }

  if (strcmp(argv[1], "read") == 0) {
    node_t root;
    node_from_file(filename, &root);
    node_print(&root, 0);
    return 0;
  }

  if (module_cache_init() != 0) {
    return 1;
  }

  module_t module = { .name = mod_name };
  if (module_parse_file(filename, &module) != 0) {
    return 1;
  }

  if (strcmp(argv[1], "parse") == 0) {
    module_print(&module);
    return 0;
  }

  if (module_compile(&module) != 0) {
    return 1;
  }

  if (strcmp(argv[1], "buildir") == 0) {
    return emit_ir(&module);
  }

  if (strcmp(argv[1], "build") == 0) {
    return emit_object_code(&module);
  }

  if (strcmp(argv[1], "run") == 0) {
    return exec_main(&module);
  }
  return 1;
}
