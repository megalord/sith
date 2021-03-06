#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

#define INDENTATION 2

#define NUM_NODES 4096
static node_t nodes[NUM_NODES];
static int i_node = 0;

#define NUM_ATOMS 4096
static atom_t atoms[NUM_ATOMS];
static int i_atom = 0;

#define NUM_LISTS 4096
static list_t lists[NUM_LISTS];
static int i_list = 0;

static unsigned int line = 1;
static unsigned int pos = 1;


node_t* node_new () {
  assert(i_node < NUM_NODES);
  return nodes + i_node++;
}

atom_t* atom_new () {
  assert(i_atom < NUM_ATOMS);
  return atoms + i_atom++;
}

list_t* list_new () {
  assert(i_list < NUM_LISTS);
  return lists + i_list++;
}

buffer_t buf_create (int size) {
  buffer_t buf;
  buf.rem = size - 1;
  buf.size = size;
  buf.data = malloc(buf.size * sizeof(char));
  buf.data[0] = '\0';
  return buf;
}

void buf_grow (buffer_t* buf) {
  buf->rem = buf->rem + buf->size;
  char* data = malloc(buf->size * 2 * sizeof(char));
  memcpy(data, buf->data, buf->size);
  free(buf->data);
  buf->size = 2 * buf->size;
  buf->data = data;
}

void buf_write (buffer_t* buf, char* str) {
  int len = strlen(str);
  if (len > buf->rem) {
    buf_grow(buf);
    buf_write(buf, str);
  } else {
    strncat(buf->data, str, buf->rem - 1);
    buf->rem = buf->rem - len;
  }
}

void buf_write_char (buffer_t* buf, char c) {
  if (buf->rem <= 0) {
    buf_grow(buf);
  }
  buf->data[buf->size-1-buf->rem--] = c;
  buf->data[buf->size-1-buf->rem] = '\0';
}

int stream_peek (stream_t* stream) {
  if (stream->next != -2) {
    return stream->next;
  } else {
    int c = fgetc(stream->source);
    stream->next = c;
    return c;
  }
}

int stream_read (stream_t* stream) {
  int c;
  if (stream->next != -2) {
    c = stream->next;
    stream->next = -2;
  } else {
    c = fgetc(stream->source);
  }

  if (c == '\n') {
    line++;
    pos = 0;
  } else {
    pos++;
  }
  return c;
}

void read_until (stream_t* stream, buffer_t* buf, int d) {
  while (1) {
    int c = stream_peek(stream);
    if (c == EOF || c == d) {
      break;
    }
    if (c == '\\') {
      buf_write_char(buf, (char)stream_read(stream));
    }
    buf_write_char(buf, (char)stream_read(stream));
  }
}

int read_empty (stream_t* stream) {
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

void read_comment (stream_t* stream) {
  buffer_t buf = buf_create(64);
  stream_read(stream);
  read_until(stream, &buf, '\n');
  stream_read(stream);
}

int read_atom (stream_t* stream, atom_t* atom) {
  buffer_t str;
  atom->line = line;
  atom->pos = pos;
  int c = stream_peek(stream);
  switch (c) {
    case '\'':
      atom->type = ATOM_CHAR;
      str = buf_create(16);
      stream_read(stream);
      read_until(stream, &str, c);
      stream_read(stream);
      break;
    case '"':
      atom->type = ATOM_STRING;
      str = buf_create(16);
      stream_read(stream);
      read_until(stream, &str, c);
      stream_read(stream);
      break;
    default:
      if (('0' <= c && c <= '9') || c == '-') {
        atom->type = ATOM_INT;
      } else {
        atom->type = ATOM_IDENTIFIER;
      }
      str = buf_create(16);
      while (1) {
        int d = stream_peek(stream);
        if (d == ' ' || d == '\n' || d == ')') {
          break;
        }
        buf_write_char(&str, (char)stream_read(stream));
      }
      break;
  }
  atom->name = str.data;
  return  0;
}

int read_list (stream_t* stream, list_t* list) {
  int c = stream_read(stream);
  if (c != '(') {
    fprintf(stderr, "expected list, got %c\n", c);
    return 1;
  }
  node_t* node;
  node_t* prev;
  while (1) {
    c = stream_peek(stream);
    if (c == EOF || c == -2 || c == ')') {
      stream_read(stream);
      break;
    } else {
      if (c == ' ') {
        stream_read(stream);
      } else {
        node = node_new();
        node->next = NULL;
        if (read_node(stream, node) != 0) {
          return 1;
        }
        if (list->len == 0) {
          list->fst = node;
        } else {
          prev->next = node;
        }
        prev = node;
        list->len = list->len + 1;
      }
    }
  }
  return  0;
}

int read_node (stream_t* stream, node_t* node) {
  read_empty(stream);
  int c = stream_peek(stream);
  switch (c) {
    case EOF:
      return 1;
    case ';':
      read_comment(stream);
      return read_node(stream, node);
    case '(':
      node->type = NODE_LIST;
      node->list = list_new();
      node->list->len = 0;
      return read_list(stream, node->list);
    default:
      node->type = NODE_ATOM;
      node->atom = atom_new();
      return read_atom(stream, node->atom);
  }
  return 0;
}

int node_from_file (char* filename, node_t* root) {
  root->next = NULL;
  root->type = NODE_LIST;
  root->list = list_new();
  root->list->len = 0;
  root->list->fst = NULL;
  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    perror("Error reading file");
    fprintf(stderr, "%s\n", filename);
    return 1;
  }

  line = 0;
  pos = 0;
  stream_t stream;
  stream.source = f;
  stream.next = -2;
  read_empty(&stream);
  node_t* node;
  node_t* prev = NULL;
  while (1) {
    node = node_new();
    node->next = NULL;
    if (read_node(&stream, node) != 0) {
      break;
    }
    if (prev == NULL) {
      root->list->fst = node;
    } else {
      prev->next = node;
    }
    root->list->len = root->list->len + 1;
    prev = node;
  }
  fclose(f);
  return  0;
}
