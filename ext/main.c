#include "ruby.h"
#include <stdlib.h>
#include <time.h>
#include "main.h"

VALUE Module1 = Qnil;
VALUE Module2 = Qnil;
VALUE Class  = Qnil;

/*
    Initialise modules and classes
*/
void Init_evolutionary() {
	Module1 = rb_define_module("Evolutionary");
    Module2 = rb_define_module_under(Module1, "GeneticAlgorithm");
    Class   = rb_define_class_under(Module2, "Base", rb_cObject);

    rb_define_module_function(Module2, "included", cMethod_module_included, 1);

    // rb_define_method(Class, "initialize", method_initialize, 1);
    // rb_define_method(Class, "evolve", method_evolve, 2);
    //     rb_define_attr(Class, "phenome", 1, 0);
    
     // seed random numbers
     srand(clock());
}

VALUE method_initialize(VALUE self, VALUE phenome) {
    rb_iv_set(self, "@phenome", phenome);
    return phenome;
}


/*
    Called when module is included in another class
    Define evolve method.
*/
static VALUE cMethod_module_included(VALUE self, VALUE in_class) {
    rb_define_method(in_class, "evolve", method_evolve, 2);
}


static VALUE method_evolve(VALUE self, VALUE vN_gens, VALUE vPop_size) {
    INSPECT(self);
}
