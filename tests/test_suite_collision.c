#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "../include/collision.h"
#include "../include/list.h"
#include "../include/test_util.h"

void test_collision() {
    // // {(-1,-1), (1,-1), (1,1), (-1, 1)}
    // list_t *shape1 = list_init(4, (free_func_t) free);
    // vector_t *p1 = vec_init_ptr(-1,1);
    // list_add(shape1, p1);
    // vector_t *p2 = vec_init_ptr(1,-1);
    // list_add(shape1, p2);
    // vector_t *p3 = vec_init_ptr(1,1);
    // list_add(shape1, p3);
    // vector_t *p4 = vec_init_ptr(0,1);
    // list_add(shape1, p4);

    // // {(0,0), (1,0), (1,1), (0, 1)}
    // list_t *shape2 = list_init(4, (free_func_t) free);
    // vector_t *p21 = vec_init_ptr(0,0);
    // list_add(shape2, p21);
    // vector_t *p22 = vec_init_ptr(1,0);
    // list_add(shape2, p22);
    // vector_t *p23 = vec_init_ptr(1,1);
    // list_add(shape2, p23);
    // vector_t *p24 = vec_init_ptr(0,1);
    // list_add(shape2, p24);
    // assert(find_collision(shape1, shape2)); // Square and overlapping square
    
    // // {(5,5), (5,6), (6,6), (6,5)}
    // list_t *shape3 = list_init(4, (free_func_t) free);
    // vector_t *p31 = vec_init_ptr(5,5);
    // list_add(shape3, p31);
    // vector_t *p32 = vec_init_ptr(5,6);
    // list_add(shape3, p32);
    // vector_t *p33 = vec_init_ptr(6,6);
    // list_add(shape3, p33);
    // vector_t *p34 = vec_init_ptr(6,5);
    // list_add(shape3, p34);
    // assert(!find_collision(shape1, shape3)); // Square and non-overlapping square

    // list_t *shape4 = list_init(3, (free_func_t) free);
    // vector_t *p41 = vec_init_ptr(5,5);
    // list_add(shape4, p41);
    // vector_t *p42 = vec_init_ptr(0,0);
    // list_add(shape4, p42);
    // vector_t *p43 = vec_init_ptr(3,3);
    // list_add(shape4, p43);
    // assert(find_collision(shape1, shape4)); // Square and overlapping triangle

    // list_t *shape5 = list_init(3, (free_func_t) free);
    // vector_t *p51 = vec_init_ptr(4,3);
    // list_add(shape5, p51);
    // vector_t *p52 = vec_init_ptr(7,6);
    // list_add(shape5, p52);
    // vector_t *p53 = vec_init_ptr(3,0);
    // list_add(shape5, p53);
    // assert(!find_collision(shape1, shape5)); // Square and non-overlapping triangle
}

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }
    DO_TEST(test_collision)
    puts("collision_tests PASS");
}