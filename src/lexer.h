#ifndef	_LEXER_H
#define	_LEXER_H

#include <stdio.h>

int INDENTATION;

typedef struct {
  int rem, size;
  char *data;
} buffer_t;

typedef struct {
  FILE *source;
  int next;
} stream_t;

typedef enum {
  ATOM_CHAR,
  ATOM_IDENTIFIER,
  ATOM_INT,
  ATOM_STRING
} atom_type_t;

typedef struct {
  char *name;
  atom_type_t type;
  unsigned int line;
  unsigned int pos;
} atom_t;

struct node_t;
typedef struct {
  int len;
  struct node_t *fst;
} list_t;

typedef enum {
  NODE_ATOM,
  NODE_LIST
} node_type_t;

typedef struct node_t {
  node_type_t type;
  union {
    atom_t* atom;
    list_t* list;
  };
  struct node_t *next;
} node_t;

int read_node (stream_t *stream, node_t *node);
int node_from_file (char* filename, node_t* root);

#endif
