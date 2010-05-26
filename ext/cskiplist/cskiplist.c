/* Copyright (c) 2010 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Skip_list_(C)?action=history&offset=20080313195128

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Retrieved from: http://en.literateprograms.org/Skip_list_(C)?oldid=12811
*/

#include <ruby.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include <string.h>

#define P 0.5
#define MAX_LEVEL 6

static VALUE cSkiplist;

struct sn {
    VALUE value;
    struct sn ** forward; /* pointer to array of pointers */
};
typedef struct sn SkipNode;

typedef struct {
    SkipNode* header;
    int level;
    int max_levels;
    double p;
} SkipSet;

float frand() {
    return (float) rand() / RAND_MAX;
}

int random_level(SkipSet *ss) {
    static int first = 1;
    int lvl = 0;

    if(first) {
        srand( (unsigned)time( NULL ) );

        first = 0;
    }

    while(frand() < ss->p && lvl < ss->max_levels)
	lvl++;

    return lvl;
} 

SkipNode* make_node(int level, VALUE value) 
{
    SkipNode* sn = (SkipNode*)malloc(sizeof(SkipNode));

    sn->forward = (SkipNode**)calloc(level + 1, sizeof(SkipNode *));

 
    sn->value = value;
    return sn;
}

//void print_skipset(SkipSet* ss) {
//    SkipNode* x = ss->header->forward[0];
//    printf("{");
//    while(x != NULL) {
//        printf("%d", x->value);
//        x = x->forward[0];
//        if(x != NULL)
//            printf(",");
//    }    
//    printf("}\n");
//}

int less_than(VALUE a, VALUE b) {
    VALUE res;
    ID symSpaceship;
    symSpaceship = rb_intern("<=>");
    res = rb_funcall3(a, symSpaceship, 1, &b);
    if (res == Qnil) {
        return 1;
    } else {
        return FIX2INT(res) == -1 ? 1 : 0;
    }
}

VALUE ss_contains(VALUE self, VALUE search_value) {
    int i;
    SkipSet* ss;
    
    Data_Get_Struct(self, SkipSet, ss);
    
    SkipNode* x = ss->header;
    for(i = ss->level; i >= 0; i--) {
        
        //if (x->forward[i] != NULL) {
        //    printf("%s hey!\n", STR2CSTR(rb_obj_as_string(rb_apply(x->forward[i]->value, symSpaceship, search_value))));
        //    printf("%s hey!\n", STR2CSTR(rb_obj_as_string(rb_apply(x->forward[i]->value, symSpaceship, search_value))));
        //    printf("%s hey!\n", STR2CSTR(rb_obj_as_string(rb_apply(x->forward[i]->value, symSpaceship, search_value))));
        //}
        
        while(x->forward[i] != NULL && less_than(x->forward[i]->value, search_value) == 1) {
            x = x->forward[i];
        }
    }
    x = x->forward[0];

    if(x != NULL && rb_equal(x->value, search_value) == Qtrue)
        return Qtrue;
    return Qfalse;   
}

VALUE ss_insert(VALUE self, VALUE value) {
    int i;
    SkipSet *ss;
    
    Data_Get_Struct(self, SkipSet, ss);
    
    SkipNode* x = ss->header;	
    SkipNode* update[ss->max_levels + 1];
    memset(update, 0, ss->max_levels + 1);
    
    
    for(i = ss->level; i >= 0; i--) {
        
        while(x->forward[i] != NULL && less_than(x->forward[i]->value, value) == 1) {
            x = x->forward[i];
        }
        update[i] = x; 
    }
    x = x->forward[0];
    
    
    if(x == NULL || rb_equal(x->value, value) == Qfalse) {        
        int lvl = random_level(ss);
    
        if(lvl > ss->level) {
    	    for(i = ss->level + 1; i <= lvl; i++) {
    	        update[i] = ss->header;
    	    }
	        ss->level = lvl;
	    }
    
        x = make_node(lvl, value);
    	for(i = 0; i <= lvl; i++) {
    	    x->forward[i] = update[i]->forward[i];
    	    update[i]->forward[i] = x;
    	}
    }
}

VALUE ss_delete(VALUE self, VALUE value) {
    int i;
    SkipSet* ss;
    
    Data_Get_Struct(self, SkipSet, ss);
    SkipNode* x = ss->header;	
    SkipNode* update[ss->max_levels + 1];
    memset(update, 0, ss->max_levels + 1);

    for(i = ss->level; i >= 0; i--) {
        while(x->forward[i] != NULL && less_than(x->forward[i]->value, value) == 1) {
            x = x->forward[i];
        }
        update[i] = x; 
    }
    x = x->forward[0];


    if(rb_equal(x->value, value) == Qtrue) {
        for(i = 0; i <= ss->level; i++) {
	    if(update[i]->forward[i] != x)
	        break;
	    update[i]->forward[i] = x->forward[i];
	}

        free(x);
        while(ss->level > 0 && ss->header->forward[ss->level] == NULL) {
	    ss->level--;
	}

    }
}

static VALUE ss_new(VALUE self, VALUE max_levels, VALUE prob) {
    SkipSet *ss;
    VALUE obj;
    obj = Data_Make_Struct(self, SkipSet, NULL, xfree, ss);
    ss->max_levels = FIX2INT(max_levels);
    ss->p = NUM2DBL(prob);
    ss->header = make_node(ss->max_levels, 0);
    ss->level = 0;
    return obj;
}

void Init_cskiplist(void) {
    cSkiplist = rb_define_class("CSkipList", rb_cObject);
    rb_define_singleton_method(cSkiplist, "new", ss_new, 2);
    rb_define_method(cSkiplist, "insert", ss_insert, 1);
    rb_define_method(cSkiplist, "contains", ss_contains, 1);
    rb_define_method(cSkiplist, "delete", ss_delete, 1);
}
