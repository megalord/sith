#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  int rem, size;
  char *data;
} buffer_t;

buffer_t buf_create (int size) {
  buffer_t buf = { .rem = size - 1, .size = size };
  buf.data = malloc(buf.size * sizeof(char));
  buf.data[0] = '\0';
  return buf;
}

void buf_grow (buffer_t *buf) {
  buf->rem += buf->size;
  char *data = malloc(buf->size * 2 * sizeof(char));
  memcpy(data, buf->data, buf->size);
  free(buf->data);
  buf->data = data;
}

void buf_write (buffer_t *buf, char *x) {
  int len = strlen(x);
  if (len > buf->rem) {
    buf_grow(buf);
    buf_write(buf, x);
  } else {
    strncat(buf->data, x, buf->rem - 1);
    buf->rem -= len;
  }
}

void buf_write_char (buffer_t *buf, char x) {
  if (buf->rem <= 0) {
    buf_grow(buf);
  }
  buf->data[buf->size - 1 - buf->rem--] = x;
  buf->data[buf->size - 1 - buf->rem] = '\0';
}

typedef struct {
  FILE *source;
  int next;
} stream_t;

int stream_peek (stream_t *stream) {
  if (stream->next != -2) {
    return stream->next;
  } else {
    int c = fgetc(stream->source);
    stream->next = c;
    return c;
  }
}

int stream_read (stream_t *stream) {
  if (stream->next != -2) {
    int c = stream->next;
    stream->next = -2;
    return c;
  } else {
    return fgetc(stream->source);
  }
}

typedef enum {
  ATOM_IDENTIFIER,
  ATOM_STRING
} atom_type_t;

typedef struct {
  char *name;
  atom_type_t type;
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
  atom_t *atom;
  list_t *list;
  struct node_t *next;
} node_t;

void read_alpha () {
}

void read_num () {
}

void read_until (stream_t *stream, buffer_t *buf, int d) {
  while (1) {
    int c = stream_peek(stream);
    if (c == EOF || c == d) {
      break;
    }
    stream_read(stream);
    buf_write_char(buf, (char)c);
  }
}

int read_empty (stream_t *stream) {
  int c;
  while (1) {
    c = stream_peek(stream);
    if (c == EOF) {
      return EOF;
    }
    if (c == ' ' || c == '\n') {
      stream_read(stream);
    } else {
      return c;
    }
  }
}

void read_comment (stream_t *stream) {
  buffer_t buf = buf_create(64);
  stream_read(stream); // ;
  read_until(stream, &buf, '\n');
  stream_read(stream); // \n
}

int read_atom (stream_t *stream, atom_t *atom) {
  buffer_t str;
  int c = stream_peek(stream);
  switch (c) {
    case '"':
      atom->type = ATOM_STRING;
      str = buf_create(16);
      stream_read(stream);
      read_until(stream, &str, '"');
      stream_read(stream);
      break;
    default:
      atom->type = ATOM_IDENTIFIER;
      str = buf_create(16);
      while (1) {
        int d = stream_peek(stream);
        if (d == ' ' || d == ')') {
          break;
        }
        buf_write_char(&str, (char)stream_read(stream));
      }
  }
  atom->name = str.data;
  return 0;
}

int read_node (stream_t *stream, node_t *node);
int read_list (stream_t *stream, list_t *list) {
  int c = stream_read(stream);
  if (c != '(') {
    fprintf(stderr, "expected list, got %c\n", c);
    return 1;
  }
  node_t *node, *prev;
  while (1) {
    c = stream_peek(stream);
    if (c == EOF || c == -2 || c == ')') {
      stream_read(stream);
      break;
    } else if (c == ' ') {
      stream_read(stream);
    } else {
      node = malloc(sizeof(node_t));
      read_node(stream, node);
      if (list->len == 0) {
        list->fst = node;
      } else {
        prev->next = node;
      }
      prev = node;
      list->len++;
    }
  }
  return 0;
}

int read_node (stream_t *stream, node_t *node) {
  read_empty(stream);
  int c = stream_peek(stream);
  if (c == EOF) {
    fprintf(stderr, "unexpected eof\n");
    return 1;
  }
  switch (c) {
    case ';':
      read_comment(stream);
      return read_node(stream, node);
    case '(':
      node->type = NODE_LIST;
      node->list = malloc(sizeof(list_t));
      return read_list(stream, node->list);
    default:
      node->type = NODE_ATOM;
      node->atom = malloc(sizeof(atom_t));
      return read_atom(stream, node->atom);
  }
}

void print_node (node_t *node, int depth) {
  if (node->type == NODE_ATOM) {
    printf("%*s atom: %s\n", depth, "", node->atom->name);
  } else if (node->type == NODE_LIST) {
    printf("%*s list: %d\n", depth, "", node->list->len);
    print_node(node->list->fst, depth + 2);
  }
  if (node->next != NULL) {
    print_node(node->next, depth);
  }
}

void print_fn (list_t *list) {
  printf("int %s () {\n", list->fst->atom->name);
}

void print_fn_call (list_t *list) {
  node_t *node = list->fst;
  printf("%s(", node->atom->name);
  node = node->next;
  if (node->atom->type == ATOM_STRING) {
    printf("\"");
  }
  printf("%s", node->atom->name);
  if (node->atom->type == ATOM_STRING) {
    printf("\"");
  }
  printf(");\n");
}

int print_c (node_t *node) {
  node_t *curr;
  if (node->type != NODE_LIST) {
    fprintf(stderr, "expected list\n");
    return 1;
  }
  curr = node->list->fst; //defun
  if (curr->type != NODE_ATOM) {
    fprintf(stderr, "expected atom\n");
    return 1;
  }
  if (strcmp(curr->atom->name, "defun") == 0) {
    curr = curr->next;
    print_fn(curr->list);
    curr = curr->next;
    while (curr != NULL) {
      printf("  ");
      if (curr->next == NULL) {
        printf("return ");
      }

      if (curr->type == NODE_LIST) {
        print_fn_call(curr->list);
      } else if (curr->type == NODE_ATOM) {
        printf("%s;\n", curr->atom->name);
      }
      curr = curr->next;
    }
    printf("}\n");
  } else if (strcmp(curr->atom->name, "include") == 0) {
    curr = curr->next;
    if (curr->type != NODE_ATOM) {
      fprintf(stderr, "expected atom\n");
      return 1;
    }
    if (curr->atom->type != ATOM_STRING) {
      fprintf(stderr, "expected string\n");
      return 1;
    }
    printf("#include <%s>\n", curr->atom->name);
  }
  return 0;
}

int main (int argc, char **argv) {
  FILE *f;

  if (argc != 3) {
    fprintf(stderr, "wrong number of args\n");
    return 1;
  }

  if (strcmp(argv[1], "build") != 0) {
    fprintf(stderr, "operation not supported\n");
    return 1;
  }

  f = fopen(argv[2], "r");
  if (f == NULL) {
    perror("Error reading file");
    return 1;
  }

  stream_t stream = { .source = f, .next = -2 };

  read_empty(&stream);
  node_t node;
  while (1) {
    if (read_node(&stream, &node) != 0) {
      break;
    }
    //print_node(&node, 0);
    print_c(&node);
  }

  fclose(f);
  return 0;
}
