#define INSPECT(rObj) printf("%s\n", STR2CSTR(rb_funcall(rObj, rb_intern("inspect"), 0)))
#define PRINT(str) printf("%s\n", str);
#define TRUE 1
#define FALSE 0

typedef char BOOL;

typedef struct candidate {
    VALUE phenome;
    char *genome;           
    int fitness;
    char rq_update;     // 1 if requires fitness updating
} Candidate;

typedef struct population {
    Candidate *candidates;
    int pop_size;           // population size
    int g_len;             // genome size
    float average_fitness;
} Population;

void Init_evolutionary();

static VALUE cMethod_module_included(VALUE self, VALUE in_class);
static VALUE method_evolve(VALUE self, VALUE vN_gens, VALUE vPop_size);
static Population* new_population(VALUE phenome, int pop_size, int g_len, BOOL randomize);
static char* random_genome(int g_len);


// Candidate* tournament_select(VALUE phenome, Candidate* pop, int pop_size);
// void populate(VALUE phenome, Candidate *pop, int g_len, int pop_size);
// void free_pop(Candidate *pop, int pop_size);
// void update_fitness(VALUE phenome, Candidate *pop);
// void two_point_crossover(char *buf, Candidate *c1, Candidate *c2, int g_len);
// Candidate* weakest(VALUE phenome, Candidate* pop, int pop_size);
// Candidate* strongest(VALUE phenome, Candidate* pop, int pop_size);
