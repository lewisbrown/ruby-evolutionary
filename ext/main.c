#include "ruby.h"
#include <stdlib.h>
#include <time.h>
#include "main.h"

VALUE Module1 = Qnil;
VALUE Module2 = Qnil;
VALUE phenome = Qnil;


/*
    Initialise modules and classes
*/
void Init_evolutionary() {
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
    
    // create a new population with random genotypes
    Population *pop = new_population(self, pop_size, g_len, TRUE);
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
        c->rq_update = FALSE;
        
        if(randomize) 
            c->genome = random_genome(g_len);
    }
        
    pop->candidates = candidates;
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
