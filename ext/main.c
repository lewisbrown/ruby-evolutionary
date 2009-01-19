#include "ruby.h"
#include <stdlib.h>

VALUE Module = Qnil;
VALUE Class  = Qnil;

struct candidate {
    char *genome;
    int fitness;
};

void Init_evolutionary();
VALUE method_initialize(VALUE self, VALUE phenome);
VALUE method_evolve(VALUE self, VALUE vN_gens, VALUE vPop_size);
char* tournament_select(VALUE phenome, char **pop, int pop_size);
void populate(char** pop, int g_size, int pop_size);
void free_pop(char **pop, int pop_size);
void two_point_crossover(char *buf, char* g1, char* g2, int g_size);

void Init_evolutionary() {
	Module = rb_define_module("Evolutionary");
    Class  = rb_define_class_under(Module, "GeneticAlgorithm", rb_cObject);
	
	rb_define_method(Class, "initialize", method_initialize, 1);	
	rb_define_method(Class, "evolve", method_evolve, 2);
    rb_define_attr(Class, "phenome", 1, 0);
}

void populate(char **pop, int g_size, int pop_size) {
    int i=0, j;
    while(i < pop_size) {
        pop[i] = (char *)malloc(sizeof(char) * g_size);
        for(j=0; j < g_size; ++j) {
            char r = random() % 2;
            sprintf(&pop[i][j], "%i", r);
        }
        i++;
    }
}

void free_pop(char **pop, int pop_size) {
    int i;
    for(i=0; i < pop_size; ++i)
        free(pop[i]);
    free(pop);
}

char* tournament_select(VALUE phenome, char **pop, int pop_size) {
    // TODO seed
    char *a, *b;
    int fa, fb;
    

    a = pop[random() % pop_size];
    b = pop[random() % pop_size];
        
    rb_funcall(phenome, rb_intern("decode"), 1, rb_str_new2(a));
    fa = FIX2INT(rb_funcall(phenome, rb_intern("fitness"), 0));

    rb_funcall(phenome, rb_intern("decode"), 1, rb_str_new2(b));
    fb = FIX2INT(rb_funcall(phenome, rb_intern("fitness"), 0));

    return fa > fb ? a : b;
}

void two_point_crossover(char *buf, char *g1, char *g2, int g_size) {
    int p1 = (random() % (g_size - 2)) + 1;
    int p2 = p1 + (random() % (g_size - p1));
    
    int i;
    for(i=0; i<p1; ++i)
        buf[i] = g1[i];
    for(;i<p2; ++i)
        buf[i] = g2[i];
    for(;i<g_size; ++i)
        buf[i] = g1[i];
}

char* weakest(VALUE phoneme, char** pop, int pop_size) {
    int i=0, fitness=0, low_fitness=0;
    char *weakest;
    while(i < pop_size) {
        //fitness = FIX2INT(rb_funcall);
    }
}

VALUE method_initialize(VALUE self, VALUE phenome) {
    rb_iv_set(self, "@phenome", phenome);
    return phenome;
}

VALUE method_evolve(VALUE self, VALUE vN_gens, VALUE vPop_size) {
    int g, pop_size, g_size;
    char **pop;

    char *child = (char *)malloc(sizeof(char) * g_size);
    
    VALUE phenome = rb_ivar_get(self, rb_intern("@phenome"));
    VALUE genotype = rb_funcall(phenome, rb_intern("encode"), 0);

    pop_size = FIX2INT(vPop_size);
    g_size = FIX2INT(rb_funcall(genotype, rb_intern("size"), 0));
    
    pop = (char **)malloc(pop_size * sizeof(char *));
    populate(pop, g_size, pop_size);
    
    for(g=0; g < FIX2INT(vN_gens); ++g) {
        if(g % 250 == 0)
            printf("Processing generation %d\n", g);
        
        char* c1 = tournament_select(phenome, pop, pop_size);
        char* c2 = tournament_select(phenome, pop, pop_size);
        
        two_point_crossover(child, c1, c2, g_size);
    }
    
    free_pop(pop, pop_size);        
    free(child);
    return Qnil;
}
