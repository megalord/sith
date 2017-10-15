buffer_t buf_create (int size) {
  buffer_t buf;
  buf.rem = size - 1;
  buf.size = size;
  buf.data = malloc(buf.size * sizeof(char));
  buf.data[0] = '\0';
  return  buf;
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
  };
}
void buf_write_char (buffer_t* buf, char c) {
  if (buf->rem <= 0) {
    buf_grow(buf);
  };
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
  };
}
int stream_read (stream_t* stream) {
  if (stream->next != -2) {
    int c = stream->next;
    stream->next = -2;
    return c;
  } else {
    return fgetc(stream->source);
  };
}
void read_until (stream_t* stream, buffer_t* buf, int d) {
  while (1) {
    int c = stream_peek(stream);
    if (c == EOF || c == d) {
      break;;
    };
    if (c == '\\') {
      buf_write_char(buf, (char)stream_read(stream));
    };
    buf_write_char(buf, (char)stream_read(stream));
  };
}
int read_empty (stream_t* stream) {
  int c;
  while (1) {
    c = stream_peek(stream);
    if (c == EOF) {
      return EOF;
    };
    if (c == ' ' || c == '\n') {
      stream_read(stream);
    } else {
      return c;
    };
  };
}
void read_comment (stream_t* stream) {
  buffer_t buf = buf_create(64);
  stream_read(stream);
  read_until(stream, &buf, '\n');
  stream_read(stream);
}
int read_atom (stream_t* stream, atom_t* atom) {
  buffer_t str;
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
      atom->type = ATOM_IDENTIFIER;
      str = buf_create(16);
      while (1) {
        int d = stream_peek(stream);
        if (d == ' ' || d == '\n' || d == ')') {
          break;;
        };
        buf_write_char(&str, (char)stream_read(stream));
      };
      break;
  };
  atom->name = str.data;
  return  0;
}
int read_list (stream_t* stream, list_t* list) {
  int c = stream_read(stream);
  if (c != '(') {
    fprintf(stderr, "expected list, got %c\n", c);
    return 1;
  };
  node_t* node;
  node_t* prev;
  while (1) {
    c = stream_peek(stream);
    if (c == EOF || c == -2 || c == ')') {
      stream_read(stream);
      break;;
    } else {
      if (c == ' ') {
        stream_read(stream);
      } else {
        node = malloc(sizeof(node_t));
        node->next = NULL;
        read_node(stream, node);
        if (list->len == 0) {
          list->fst = node;
        } else {
          prev->next = node;
        };
        prev = node;
        list->len = list->len + 1;
      };
    };
  };
  return  0;
}
int read_node (stream_t* stream, node_t* node) {
  read_empty(stream);
  int c = stream_peek(stream);
  if (c == EOF) {
    fprintf(stderr, "unexpected eof\n");
    return 1;
  };
  switch (c) {
    case ';':
      read_comment(stream);
      return read_node(stream, node);
      break;
    case '(':
      node->type = NODE_LIST;
      node->list = malloc(sizeof(list_t));
      node->list->len = 0;
      return read_list(stream, node->list);
      break;
    default:
      node->type = NODE_ATOM;
      node->atom = malloc(sizeof(atom_t));
      return read_atom(stream, node->atom);
      break;
  };
  return  0;
}
void print_node (node_t* node, int depth) {
  if (node->type == NODE_ATOM) {
    printf("%*s atom: %s\n", depth, "", node->atom->name);
  } else {
    if (node->type == NODE_LIST) {
      printf("%*s list: %d\n", depth, "", node->list->len);
      print_node(node->list->fst, depth + INDENTATION);
    };
  };
  if (node->next != NULL) {
    print_node(node->next, depth);
  };
}
