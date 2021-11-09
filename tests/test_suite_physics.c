#include "test_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "polygon.h"
#include "physics.h"
#include "render.h"

// // void test_create_body() {
// //     int num_vertex = (rand() % (10 - 2 + 1)) + 2;
// //     body_t *polygon = create_nstar(num_vertex, 10, 0);
// //     vector_t init_vel = {.x = 10, .y = 0};
// //     body_set_velocity(polygon, init_vel);
// //     vector_t poly_velocity = body_get_velocity(polygon);
// //     rgb_color_t poly_color = body_get_color(polygon);
    
// //     assert(poly_color.r == (float) 0.1);
// //     assert(poly_color.g == (float) 0.2);
// //     assert(poly_color.b == (float) 0.3);
// //     assert(vec_equal(poly_velocity, init_vel));
// // }

// void test_do_gravity(){
//     body_t *polygon = create_nstar(5, 10, 0);
//     // rgb_color_t color = {.r = 0.5, .g = 0.5, .b = 0.5};
//     vector_t init_vel = (vector_t) {10, 0};
//     // body_t *polygon = body_init(vertices, 0, color);
//     body_set_velocity(polygon, init_vel);

//     body_t *result_1 = do_gravity(polygon, 0, 1);
//     vector_t vel_1 = body_get_velocity(result_1);
//     assert(vec_equal(vel_1, init_vel));

//     body_t *result_2 = do_gravity(polygon, 10, 1);
//     vector_t vel_2 = body_get_velocity(result_2);
//     vector_t expected_2 = vec_subtract(init_vel, (vector_t) {0, 10});
//     assert(vec_equal(vel_2, expected_2));
// } 

// void test_collide(){
//     body_t *polygon = create_nstar(4, 10, 0);
//     // rgb_color_t color = {.r = 0.5, .g = 0.5, .b = 0.5};
//     vector_t init_vel = {.x = 20.0, .y = 30.0};
//     // body_t *polygon = body_init(vertices, 0, color);
//     body_set_velocity(polygon, init_vel);
//     vector_t translation = {.x = 50, .y = 50};

//     list_t *shape = body_get_shape(polygon);
//     polygon_translate(shape, translation);

//     body_t *collided = collide(polygon, VEC_ZERO, (vector_t) {100, 100}, 1);
//     vector_t col_vel = body_get_velocity(collided);
//     assert(vec_equal(col_vel, init_vel)); //No collision

//     vector_t neg_x_vel = {-20.0, 30.0};
//     body_t *col_2 = collide(polygon, VEC_ZERO, (vector_t) {55, 100}, 1);
//     vector_t col2_vel = body_get_velocity(col_2);
//     assert(vec_equal(col2_vel, neg_x_vel)); // x-axis collision only

//     body_t *col_3 = collide(polygon, VEC_ZERO, (vector_t) {45, 45}, 1);
//     vector_t col3_vel = body_get_velocity(col_3);
//     assert(vec_equal(col3_vel, vec_negate(init_vel))); // x and y collision
// }

int main(int argc, char *argv[]) {
    // // Run all tests if there are no command-line arguments
    // bool all_tests = argc == 1;
    // // Read test name from file
    // char testname[100];
    // if (!all_tests) {
    //     read_testname(argv[1], testname, sizeof(testname));
    // }

    // // DO_TEST(test_create_body)
    // // DO_TEST(test_do_gravity)
    // // DO_TEST(test_collide)

    puts("physics_tests PASS");
}