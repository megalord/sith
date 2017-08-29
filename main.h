typedef struct {
  int rem, size;
  char *data;
} buffer_t;

typedef struct {
  FILE *source;
  int next;
} stream_t;

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

// recursively used functions
int read_node (stream_t *stream, node_t *node);
int print_fn_call (list_t *list, int depth);
