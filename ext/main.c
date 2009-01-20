#include "ruby.h"
#include <stdlib.h>
#include <time.h>

VALUE Module = Qnil;
VALUE Class  = Qnil;

typedef struct candidate {
    char *genome;
    int fitness;
} Candidate;

void Init_evolutionary();
VALUE method_initialize(VALUE self, VALUE phenome);
VALUE method_evolve(VALUE self, VALUE vN_gens, VALUE vPop_size);
Candidate* tournament_select(VALUE phenome, Candidate* pop, int pop_size);
void populate(VALUE phenome, Candidate *pop, int g_size, int pop_size);
void free_pop(Candidate *pop, int pop_size);
void update_fitness(VALUE phenome, Candidate *pop);
void two_point_crossover(char *buf, Candidate *c1, Candidate *c2, int g_size);
Candidate* weakest(VALUE phenome, Candidate* pop, int pop_size);
// Candidate* strongest(VALUE phenome, Candidate* pop, int pop_size);

void Init_evolutionary() {
	Module = rb_define_module("Evolutionary");
    Class  = rb_define_class_under(Module, "GeneticAlgorithm", rb_cObject);

	rb_define_method(Class, "initialize", method_initialize, 1);	
	rb_define_method(Class, "evolve", method_evolve, 2);
    rb_define_attr(Class, "phenome", 1, 0);
    
     // seed random numbers  
     srand((unsigned)clock());
}


void update_fitness(VALUE phenome, Candidate *pop) {
    rb_funcall(phenome, rb_intern("decode"), 1, rb_str_new2(pop->genome));
    pop->fitness = FIX2INT(rb_funcall(phenome, rb_intern("fitness"), 0));
}

void populate(VALUE phenome, Candidate *pop, int g_size, int pop_size) {
    int i=0, j;    
    Candidate* pop_p = pop;
        
    while(i < pop_size) {
        // TODO use ALLOC
        char *genome = (char *)malloc(g_size * sizeof(char));

        for(j=0; j < g_size; ++j) {
            sprintf((genome + j), "%i", rand() % 2);
        }
        
        pop_p->genome = genome;
        pop_p->fitness = -10000;
        
        update_fitness(phenome, pop_p);

        pop_p++;
        i++;
    }
}

void free_pop(Candidate* pop, int pop_size) {
    // TODO free everything
    
    // int i;
    // for(i=0; i < pop_size; ++i) {
    //     if((pop+i)->genome != NULL) {
    //         free((pop+i)->genome);
    //         printf("freed genome");
    //     }
    //     free(pop+i);
    // };
    // free(pop);
}

Candidate* tournament_select(VALUE phenome, Candidate* pop, int pop_size) {
    Candidate *a, *b;
    int fa, fb;
    
    // random selection
    a = pop + (rand() % pop_size);
    b = pop + (rand() % pop_size);
                        
    rb_funcall(phenome, rb_intern("decode"), 1, rb_str_new2(a->genome));
    fa = FIX2INT(rb_funcall(phenome, rb_intern("fitness"), 0));

    rb_funcall(phenome, rb_intern("decode"), 1, rb_str_new2(b->genome));
    fb = FIX2INT(rb_funcall(phenome, rb_intern("fitness"), 0));
    
    return fa > fb ? a : b;
}

void two_point_crossover(char *buf, Candidate *c1, Candidate *c2, int g_size) {
    int p1 = random() % g_size;
    int p2 = p1 + (random() % (g_size - p1));


    int i;
    for(i=0; i<p1; ++i) {
        buf[i] = c1->genome[i];  
    }
    for(;i<p2; ++i)
        buf[i] = c2->genome[i];
    for(;i<g_size; ++i)
        buf[i] = c1->genome[i];
}

Candidate* weakest(VALUE phenome, Candidate* pop, int pop_size) {
    int i=0, fitness=0, lowest=0;
    Candidate *weakest_p;
        
    while(i < pop_size) {
        if((pop+i)->fitness < lowest) {
            weakest_p = (pop+i);
            lowest = (pop+i)->fitness;
        }
        ++i;
    }
    return weakest_p;
}

// Candidate* strongest(VALUE phenome, Candidate* pop, int pop_size) {
//     int i=0, fitness=0, highest=0;
//     Candidate *strongest_p;
//        
//     while(i < pop_size) {
//         if((pop+i)->fitness > highest) {
//             strongest_p = (pop+i);
//             highest = (pop+i)->fitness;
//         }
//         ++i;
//     }
//     
//     printf("highest was %i\n",highest);
//     return strongest_p;
// }

VALUE method_initialize(VALUE self, VALUE phenome) {
    rb_iv_set(self, "@phenome", phenome);
    return phenome;
}

VALUE method_evolve(VALUE self, VALUE vN_gens, VALUE vPop_size) {
    int g, pop_size, g_size;

    // get the requested population size
    pop_size = FIX2INT(vPop_size);
    
    // allocate space. TODO use ALLOC throughout
    Candidate *pop = ALLOC_N(Candidate, pop_size);
    char *child = (char *)malloc(g_size * sizeof(char));

    // obtain a pointer to the actual object to evolve.
    // This should respond to the messages encode, decode and fitness.
    VALUE phenome = rb_iv_get(self, "@phenome");

    // encode the phenome to get the lengthy of the encoded genome
    VALUE vGenotype = rb_funcall(phenome, rb_intern("encode"), 0);
    g_size = RSTRING(vGenotype)->len;
    
    //printf("Fitness level pre-evolution: %d\n", FIX2INT(rb_funcall(phenome, rb_intern("fitness"), 0)));

    // populate with randomly generated candidates
    populate(phenome, pop, g_size, pop_size);
    
    // for each generation evolve a new set of candidates.
    // Select two fitter individuals, recombine and replace the weakest.
    for(g=0; g < FIX2INT(vN_gens); ++g) {
        // if(g % 10 == 0)
        //     printf("Processing generation %d\n", g);

        Candidate* c1 = tournament_select(phenome, pop, pop_size);
        Candidate* c2 = tournament_select(phenome, pop, pop_size);
        
        two_point_crossover(child, c1, c2, g_size);
        Candidate *weak = weakest(phenome, pop, pop_size);
        weak->genome = child;

        update_fitness(phenome, weak);
    }
        // 
        // Candidate *strong = strongest(phenome, pop, pop_size);
        // printf("%s\n",strong->genome);
        // rb_funcall(phenome, rb_intern("decode"), 1, rb_str_new2(strong->genome));
        // 
    //printf("Evolved so that fitness is now %d\n", strong->fitness);
    
    
    
    // TODO tidy everything up
    //free_pop(pop, pop_size);        
    free(child);
    return Qnil;
}
