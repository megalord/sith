#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int str_includes (char* input, const char* test) {
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
