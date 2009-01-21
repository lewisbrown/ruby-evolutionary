#include "ruby.h"
#include <stdlib.h>
#include <time.h>
#include "main.h"

VALUE phenome = Qnil;

/*
    Initialise modules and classes
*/
void Init_evolutionary() {
    VALUE Module1 = Qnil;
    VALUE Module2 = Qnil;

	Module1 = rb_define_module("Evolutionary");
    Module2 = rb_define_module_under(Module1, "GeneticAlgorithm");
    rb_define_module_function(Module2, "included", cMethod_module_included, 1);
    
     // seed random numbers
     srand(clock());
}


/*
    Called when module is included in another class
    Define evolve method.
*/
static VALUE cMethod_module_included(VALUE self, VALUE in_class) {
    rb_define_method(in_class, "evolve", method_evolve, 2);
}


/*
    Main evolution method, called by user and handles
    main loop
*/
static VALUE method_evolve(VALUE self, VALUE vN_gens, VALUE vPop_size) {
    // This is the object to be evolved: store globally
    phenome = self;
    
    // Process method parameters
    int n_gens = FIX2INT(vN_gens);
    int pop_size = FIX2INT(vPop_size);
    
    // We need to call encode once, to obtain genome length
    VALUE ary_genome = rb_funcall(self, rb_intern("encode"), 0);
    int g_len = RARRAY(ary_genome)->len;
    
    // space for new genotype to be created
    char *child_genome = (char *)malloc(g_len * sizeof(char));
    
    // create a new population with random genotypes
    Population *pop = new_population(self, pop_size, g_len, TRUE);
    update_fitness(pop, g_len);
    
    // main loop
    int g;
    for(g=0; g < n_gens; ++g) {
        if(g % 100 == 0)
            printf("Processing generation %d\n", g);
            
        Candidate *c1 = tournament_select(pop);
        Candidate *c2 = tournament_select(pop);
        
        two_point_crossover(child_genome, c1, c2, g_len);
        
        Candidate *low = weakest(pop);
        low->genome = child_genome;
        low->rq_update = TRUE;
        update_fitness(pop, g_len);
        
        Candidate *high = strongest(pop);
        
        printf("weakest is %i\n", low->fitness);
        printf("strongest is %i\n", high->fitness);
    }
}


/*
    Create a new, blank population, and return a pointer to the struct.
*/
static Population* new_population(VALUE phenome, int pop_size, int g_len, BOOL randomize) {
    Population *pop = ALLOC(Population);
    Candidate *candidates = ALLOC_N(Candidate, pop_size);

    int i;
    for(i=0; i < pop_size; ++i) {
        Candidate *c = (candidates + i);
        c->phenome = phenome;
        c->fitness = -INT_MAX;
        c->rq_update = TRUE;
        
        if(randomize) 
            c->genome = random_genome(g_len);       
    }

    pop->g_len = g_len;
    pop->pop_size = pop_size;
    pop->candidates = candidates;

    return pop;
}


/*
    Return a random genome char array of length g_len
*/
static char* random_genome(int g_len) {
    int j;
    char *genome = ALLOC_N(char, g_len);
    
    for(j=0; j < g_len; ++j) {
        sprintf((genome + j), "%i", rand() % 2);
    }
    return genome;
}


/* 
    for each member of population, check if requires updating 
    and update if so.
*/
static void update_fitness(Population *pop, int g_len) {
    int i;
    Candidate *candidates = pop->candidates;

    for(i=0; i < pop->pop_size; ++i) {
        Candidate *c = (candidates + i);
        if(c->rq_update) {
            rb_funcall(phenome, rb_intern("decode"), 1, genome_to_int_array(c->genome, g_len));
            c->fitness = FIX2INT(rb_funcall(phenome, rb_intern("fitness"), 0));
        }
    }
}


/*
    Convert a char *genome to ruby array of integers
*/
static VALUE genome_to_int_array(char *genome, int g_len) {
    int i;
    VALUE arr = rb_ary_new();
    for(i=0; i < g_len; ++i) {
        rb_ary_push(arr, rb_int_new((genome[i]) - '0'));
    }
    return arr;
}


/*
    tournament selection
*/
static Candidate* tournament_select(Population *pop) {
    Candidate *a, *b;

    int pop_size = pop->pop_size;
    Candidate *candidates = pop->candidates;
    
    // random selection
    a = candidates + (rand() % pop_size);
    b = candidates + (rand() % pop_size);
    
    return (a->fitness > b->fitness) ? a : b;
}


/* 
    two-point crossover
*/
static void two_point_crossover(char *buf, Candidate *c1, Candidate *c2, int g_len) {
    int p1 = rand() % g_len;
    int p2 = p1 + (rand() % (g_len - p1));

    int i;
    for(i=0; i<p1; ++i) 
        buf[i] = c1->genome[i];  
    for(;i<p2; ++i)
        buf[i] = c2->genome[i];
    for(;i<g_len; ++i)
        buf[i] = c1->genome[i];
}


/*
    weakest in population
*/
static Candidate* weakest(Population *pop) {
    int i;
    int lowest = INT_MAX;
    Candidate *weak, *current;
    for(i=0; i < pop->pop_size; ++i) {
        current = (pop->candidates + i);
        if(current->fitness < lowest) {
            lowest = current->fitness;
            weak = current;
        }
    }
    return weak;
}


/*
    strongest in population
*/
static Candidate* strongest(Population *pop) {
    int i;
    int highest = INT_MIN;
    Candidate *strong, *current;
    for(i=0; i < pop->pop_size; ++i) {
        current = (pop->candidates + i);
        if(current->fitness > highest) {
            highest = current->fitness;
            strong = current;
        }
    }
    return strong;
}