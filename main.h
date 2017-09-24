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

// TODO: implement set datatype
typedef enum {
  //TYPE_BOOL,
  TYPE_CHAR,
  //TYPE_LIST,
  TYPE_INT,
  TYPE_WILD
} type_t;

struct symbol_t;
typedef struct {
  int arity;
  struct symbol_t* args;
  struct symbol_t* ret;
} func_t;

typedef enum {
  SYMBOL_VAR,
  SYMBOL_FUNC
} symbol_type_t;

typedef struct symbol_t {
  char* name;
  symbol_type_t type;
  //int is_extern;
  union {
    char* var;
    func_t* fn;
  } def;
} symbol_t;

typedef struct {
  int len;
  symbol_t* symbols;
} symbol_table_t;

struct module_t;
typedef struct module_t {
  char* name;
  int num_deps;
  struct module_t* deps;
  symbol_table_t table;
  list_t code;
} module_t;

int INDENTATION = 2;

// recursively used functions
int read_node (stream_t *stream, node_t *node);
int print_fn_call (FILE* fp, list_t *list, int depth);
int parse_filename (char*, char**);
int parse (char*, module_t*);
int compile_front (module_t*, char*);

//symbol_table_t global_table = { .len = 0 };
//symbol_t* lookup_symbol(char*);
