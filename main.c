#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int INDENTATION = 2;

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
  buf->size += buf->size;
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
        if (d == ' ' || d == '\n' || d == ')') {
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
      node->next = NULL;
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
      node->list->len = 0;
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
    print_node(node->list->fst, depth + INDENTATION);
  }
  if (node->next != NULL) {
    print_node(node->next, depth);
  }
}

bool is_infix(char *input) {
  if (strcmp(input, "lt") == 0 || strcmp(input, "lte") == 0 || strcmp(input, "eq") == 0 ||
      strcmp(input, "gt") == 0 || strcmp(input, "gte") == 0 || strcmp(input, "ne") == 0) {
    return true;
  } else if (strcmp(input, "set") == 0) {
    return true;
  } else if (strcmp(input, "+") == 0 || strcmp(input, "-") == 0 || strcmp(input, "*") == 0 ||
      strcmp(input, "/") == 0) {
    return true;
  } else if (strcmp(input, "and") == 0 || strcmp(input, "or") == 0) {
    return true;
  }
  return false;
}

void transform_fn_name(char *input, char **output) {
  *output = malloc(32);
  (*output)[0] = '\0';
  if (strcmp(input, "lt") == 0) {
    strncpy(*output, "<", 31);
  } else if (strcmp(input, "lte") == 0) {
    strncpy(*output, "<=", 31);
  } else if (strcmp(input, "eq") == 0) {
    strncpy(*output, "==", 31);
  } else if (strcmp(input, "gt") == 0) {
    strncpy(*output, ">", 31);
  } else if (strcmp(input, "gte") == 0) {
    strncpy(*output, ">=", 31);
  } else if (strcmp(input, "ne") == 0) {
    strncpy(*output, "!=", 31);
  } else if (strcmp(input, "set") == 0) {
    strncpy(*output, "=", 31);
  } else if (strcmp(input, "and") == 0) {
    strncpy(*output, "&&", 31);
  } else if (strcmp(input, "or") == 0) {
    strncpy(*output, "||", 31);
  } else {
    strncpy(*output, input, 31);
  }
}

int print_fn_call (list_t *list, int depth);
int print_statement (node_t *node, int depth) {
  if (node->type == NODE_LIST) {
    if (print_fn_call(node->list, depth) != 0) {
      return 1;
    }
  } else if (node->type == NODE_ATOM) {
    printf("%*s%s", depth, "", node->atom->name);
  }
  return 0;
}

void print_fn_call_infix (node_t *node, int depth) {
  char *operator;
  transform_fn_name(node->atom->name, &operator);
  node = node->next;
  if (node->type == NODE_ATOM) {
    printf("%*s%s", depth, "", node->atom->name);
  } else if (node->type == NODE_LIST) {
    print_fn_call(node->list, depth);
  }
  printf(" %s ", operator);
  node = node->next;
  if (node->type == NODE_ATOM) {
    printf("%s", node->atom->name);
  } else if (node->type == NODE_LIST) {
    print_fn_call(node->list, 0);
  }
  free(operator);
}

int print_def (list_t *list, int depth) {
  if (list->len != 3 && list->len != 4) {
    fprintf(stderr, "invalid def - needs 2 or 3 arguments, has %d\n", list->len - 1);
    return 1;
  }
  node_t *node = list->fst->next; // skip def
  if (node->type != NODE_ATOM) {
    fprintf(stderr, "def type must be an atom\n");
    return 1;
  }
  printf("%*s%s ", depth, "", node->atom->name);

  node = node->next;
  if (node->type != NODE_ATOM) {
    fprintf(stderr, "def variable name must be an atom\n");
    return 1;
  }
  printf("%s", node->atom->name);

  if (list->len == 4) {
    node = node->next;
    printf(" = ");
    if (print_statement(node, 0) != 0) {
      return 1;
    }
  }
  return 0;
}

int print_if (list_t *list, int depth) {
  if (list->len != 3 && list->len != 4) {
    fprintf(stderr, "invalid if - needs 1 or 2 statements, has %d\n", list->len - 2);
    return 1;
  }
  printf("%*sif (", depth, "");
  node_t *node = list->fst->next; // skip if
  if (node->type != NODE_LIST) {
    fprintf(stderr, "if conditional must be a list\n");
    return 1;
  }
  print_fn_call(node->list, 0);
  printf(") {\n");
  node = node->next;
  if (node->type != NODE_LIST) {
    fprintf(stderr, "if body must be a list\n");
    return 1;
  }
  print_fn_call(node->list, depth + INDENTATION);
  printf(";\n");
  if (list->len == 4) {
    printf("%*s} else {\n", depth, "");
    node = node->next;
    if (node->type != NODE_LIST) {
      fprintf(stderr, "else body must be a list\n");
      return 1;
    }
    print_fn_call(node->list, depth + INDENTATION);
    printf(";\n");
  }
  printf("%*s}", depth, "");
  return 0;
}

int print_while (list_t *list, int depth) {
  if (list->len != 3 && list->len != 4) {
    fprintf(stderr, "invalid while - needs 2 statements, has %d\n", list->len - 1);
    return 1;
  }
  printf("%*swhile (", depth, "");
  node_t *node = list->fst->next; // skip while
  if (node->type != NODE_LIST) {
    fprintf(stderr, "while conditional must be a list\n");
    return 1;
  }
  print_fn_call(node->list, 0);
  printf(") {\n");
  node = node->next;
  if (node->type != NODE_LIST) {
    fprintf(stderr, "while body must be a list\n");
    return 1;
  }
  print_fn_call(node->list, depth + INDENTATION);
  printf(";\n%*s}", depth, "");
  return 0;
}

int print_for (list_t *list, int depth) {
  if (list->len != 5) {
    fprintf(stderr, "invalid for - needs 4 statements, has %d\n", list->len - 1);
    return 1;
  }
  printf("%*sfor (", depth, "");
  node_t *node = list->fst->next; // skip for
  if (node->type != NODE_LIST) {
    fprintf(stderr, "for initialization must be a list\n");
    return 1;
  }
  print_fn_call(node->list, 0);
  printf("; ");

  node = node->next;
  if (node->type != NODE_LIST) {
    fprintf(stderr, "for condition must be a list\n");
    return 1;
  }
  print_fn_call(node->list, 0);
  printf("; ");

  node = node->next;
  if (node->type != NODE_LIST) {
    fprintf(stderr, "for afterthought must be a list\n");
    return 1;
  }
  print_fn_call(node->list, 0);
  printf(") {\n");

  node = node->next;
  if (node->type != NODE_LIST) {
    fprintf(stderr, "if body must be a list\n");
    return 1;
  }
  print_fn_call(node->list, depth + INDENTATION);
  printf(";\n%*s}", depth, "");
  return 0;
}

int print_case (list_t *list, int depth) {
  if (list->len < 3) {
    fprintf(stderr, "invalid case - needs at least 2 statements, has %d\n", list->len - 1);
    return 1;
  }
  printf("%*sswitch (", depth, "");
  node_t *node = list->fst->next; // skip case
  print_statement(node, 0);
  printf(") {\n");

  node = node->next;
  node_t *clause;
  while (node != NULL) {
    if (node->type != NODE_LIST) {
      fprintf(stderr, "case clause must be a list\n");
      return 1;
    }
    clause = node->list->fst;
    if (clause->type == NODE_ATOM && strcmp(clause->atom->name, "else") == 0) {
      printf("%*sdefault:\n", depth + INDENTATION, "");
    } else {
      printf("%*scase ", depth + INDENTATION, "");
      print_statement(clause, 0);
      printf(":\n");
    }

    clause = clause->next;
    if (clause->type != NODE_LIST) {
      fprintf(stderr, "case situation body must be a list\n");
      return 1;
    }
    print_fn_call(clause->list, depth + 2 * INDENTATION);
    printf(";\n%*sbreak;\n", depth + 2 * INDENTATION, "");
    node = node->next;
  }
  printf("%*s}", depth, "");
  return 0;
}

int print_fn_call (list_t *list, int depth) {
  node_t *node = list->fst;
  if (node->type != NODE_ATOM) {
    fprintf(stderr, "function call must start with atom\n");
    return 1;
  }
  if (is_infix(node->atom->name)) {
    print_fn_call_infix(node, depth);
    return 0;
  }
  if (strcmp(node->atom->name, "def") == 0) {
    return print_def(list, depth);
  }
  if (strcmp(node->atom->name, "if") == 0) {
    return print_if(list, depth);
  }
  if (strcmp(node->atom->name, "while") == 0) {
    return print_while(list, depth);
  }
  if (strcmp(node->atom->name, "for") == 0) {
    return print_for(list, depth);
  }
  if (strcmp(node->atom->name, "case") == 0) {
    return print_case(list, depth);
  }
  if (strcmp(node->atom->name, "progn") == 0) {
    node = node->next;
    while (node != NULL) {
      if (print_statement(node, depth) != 0) {
        return 1;
      }
      node = node->next;
      if (node != NULL) {
        printf(";\n");
      }
    }
    return 0;
  }

  char *name;
  transform_fn_name(node->atom->name, &name);
  printf("%*s%s(", depth, "", name);
  node = node->next;
  while (node != NULL) {
    if (node->type == NODE_ATOM) {
      if (node->atom->type == ATOM_STRING) {
        printf("\"");
      }
      printf("%s", node->atom->name);
      if (node->atom->type == ATOM_STRING) {
        printf("\"");
      }
    } else if (node->type == NODE_LIST) {
      if (print_fn_call(node->list, depth + INDENTATION) != 0) { // TODO: fix indentation
        return 1;
      }
    }
    node = node->next;
    if (node != NULL) {
      printf(", ");
    }
  }
  printf(")");
  return 0;
}

int print_c_fn_body (node_t *node) {
  if (node == NULL) {
    fprintf(stderr, "empty function body\n");
    return 1;
  }
  while (node != NULL) {
    if (node->next == NULL) {
      if (node->type == NODE_ATOM && strcmp(node->atom->name, "void") == 0) {
        return 0;
      }
      printf("%*sreturn", INDENTATION, "");
    }
    if (print_statement(node, INDENTATION) != 0) {
      return 1;
    }
    printf(";\n");
    node = node->next;
  }
  return 0;
}

int print_c_fn (node_t *sig_node, node_t *defun_node) {
  if (sig_node->type != NODE_LIST || defun_node->type != NODE_LIST) {
    fprintf(stderr, "expected two lists\n");
    return 1;
  }
  if (sig_node->list->len != 3) {
    fprintf(stderr, "invalid type definition\n");
    return 1;
  }
  sig_node = sig_node->list->fst;
  if (sig_node->type != NODE_ATOM || strcmp(sig_node->atom->name, ":") != 0) {
    fprintf(stderr, "expected ':' atom\n");
    return 1;
  }
  sig_node = sig_node->next;
  if (sig_node->type != NODE_ATOM) {
    fprintf(stderr, "invalid function name\n");
    return 1;
  }
  char *fn_name = sig_node->atom->name;
  sig_node = sig_node->next;
  if (sig_node->type != NODE_LIST) {
    fprintf(stderr, "type signature must be a function\n");
    return 1;
  }
  if (sig_node->list->len < 2) {
    fprintf(stderr, "invalid function type definition\n");
    return 1;
  }
  node_t *node = sig_node->list->fst;
  if (node->type != NODE_ATOM || strcmp(node->atom->name, "->") != 0) {
    fprintf(stderr, "expected '->' atom\n");
    return 1;
  }
  node = node->next;
  if (node->type != NODE_ATOM) {
    fprintf(stderr, "lambdas not supported\n");
    return 1;
  }
  printf("%s %s (", node->atom->name, fn_name);
  defun_node = defun_node->list->fst;
  if (defun_node->type != NODE_ATOM || strcmp(defun_node->atom->name, "defun") != 0) {
    fprintf(stderr, "function definition must follow type definition\n");
    return 1;
  }
  defun_node = defun_node->next;
  if (sig_node->list->len - 1 != defun_node->list->len) {
    fprintf(stderr, "type definition doesn't match function signature\n");
    return 1;
  }
  int num_args = defun_node->list->len - 1;
  node_t *arg_node = defun_node->list->fst;
  if (arg_node->type != NODE_ATOM) {
    fprintf(stderr, "invalid function name\n");
    return 1;
  }
  if (strcmp(fn_name, arg_node->atom->name) != 0) {
    fprintf(stderr, "function name doesn't match type definition\n");
    return 1;
  }
  arg_node = arg_node->next; // first argument
  node = node->next;
  for (int i = 0; i < num_args; i++) {
    if (i != 0) {
      printf(", ");
    }
    if (arg_node->type != NODE_ATOM || node->type != NODE_ATOM) {
      fprintf(stderr, "lambdas not supported\n");
      return 1;
    }
    printf("%s %s", node->atom->name, arg_node->atom->name);
    arg_node = arg_node->next;
    node = node->next;
  }
  printf(") {\n");
  if (print_c_fn_body(defun_node->next) != 0) {
    return 1;
  }
  printf("}\n");
  return 0;
}

int print_c_include (node_t *node) {
  if (node->type != NODE_ATOM) {
    fprintf(stderr, "expected atom\n");
    return 1;
  }
  if (node->atom->type != ATOM_STRING) {
    fprintf(stderr, "expected string\n");
    return 1;
  }
  printf("#include <%s>\n", node->atom->name);
  return 0;
}

int print_c (node_t *node) {
  node_t *curr;
  if (node->type != NODE_LIST) {
    fprintf(stderr, "expected list\n");
    return 1;
  }
  curr = node->list->fst;
  if (curr->type != NODE_ATOM) {
    fprintf(stderr, "expected atom\n");
    return 1;
  }
  if (strcmp(curr->atom->name, ":") == 0) {
    return print_c_fn(node, node->next);
  } else if (strcmp(curr->atom->name, "defun") == 0) {
    // typeless functions are not yet supported
    return 0;
  } else if (strcmp(curr->atom->name, "include") == 0) {
    return print_c_include(curr->next);
  }
  return 0;
}

int parse (char *filename, node_t *root) {
  root->type = NODE_LIST;
  root->list = malloc(sizeof(list_t));
  root->list->len = 0;
  root->list->fst = NULL;

  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    perror("Error reading file");
    return 1;
  }

  stream_t stream = { .source = f, .next = -2 };

  read_empty(&stream);
  node_t *node, *prev = NULL;
  while (1) {
    node = malloc(sizeof(node_t));
    node->next = NULL;
    if (read_node(&stream, node) != 0) {
      free(node);
      break;
    }
    if (prev == NULL) {
      root->list->fst = node;
    } else {
      prev->next = node;
    }
    root->list->len++;
    prev = node;
  }

  fclose(f);
  return 0;
}

int main (int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "wrong number of args\n");
    return 1;
  }

  node_t root;
  if (strcmp(argv[1], "build") == 0) {
    parse(argv[2], &root);
    node_t *node = root.list->fst;
    while (node != NULL) {
      print_c(node);
      node = node->next;
    }
  } else if (strcmp(argv[1], "parse") == 0) {
    parse(argv[2], &root);
    print_node(&root, 0);
  } else {
    fprintf(stderr, "operation not supported\n");
    return 1;
  }
  return 0;
}
