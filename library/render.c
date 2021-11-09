#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "render.h"
#include "scene.h" 
#include "color.h"
#include <stdbool.h>
#include "vector.h"

const int CIRCLE_APPROX = 50;

list_t *create_triangle_shape(double a) {
    list_t *triangle = list_init(3, (free_func_t) free);
    list_add(triangle, vec_init_ptr(0, 0));
    list_add(triangle, vec_init_ptr(a, 0));
    list_add(triangle, vec_init_ptr(a/2.0, a));
    return triangle;
}

list_t *create_oval_shape(double a, double b){
    list_t *oval = list_init(CIRCLE_APPROX, (free_func_t) free);
    vector_t *extern_points = malloc(sizeof(vector_t));

    for(int i = 0; i < CIRCLE_APPROX; i++) {
        double angle = 2.0 * M_PI / CIRCLE_APPROX * i;
        extern_points->x = a * cos(angle);
        extern_points->y = b * sin(angle);
        vector_t *temp_v = malloc(sizeof(vector_t));
        *temp_v = vec_rotate(*extern_points, 0.0);
        list_add(oval, temp_v);
    }

    free(extern_points);
    return oval;
}

body_t *create_oval(double a, double b, double mass) {
    list_t *shape = create_oval_shape(a, b);
    rgb_color_t color = rgb_color_neon_green();
    body_t *oval = body_init(shape, mass, color); // No mass for this week
    return oval;
}

list_t *create_rectangle_shape(double x, double y){
    list_t *rectangle = list_init(4, (free_func_t) free);

    vector_t *top_right = malloc(sizeof(vector_t));
    top_right->x = x;
    top_right->y = y;
    list_add(rectangle, top_right);

    vector_t *bottom_right = malloc(sizeof(vector_t));
    bottom_right->x = x;
    bottom_right->y = 0;
    list_add(rectangle, bottom_right);

    vector_t *bottom_left = malloc(sizeof(vector_t));
    bottom_left->x = 0;
    bottom_left->y = 0;
    list_add(rectangle, bottom_left);

    vector_t *top_left = malloc(sizeof(vector_t));
    top_left->x = 0;
    top_left->y = y;
    list_add(rectangle, top_left);

    return rectangle;
}

body_t *create_rectangle(double x, double y, double mass) {
    list_t *shape = create_rectangle_shape(x, y);
    rgb_color_t color = rgb_color_init(255, 255, 255);
    body_t *rect = body_init(shape, mass, color);
    return rect;
}

body_t *create_rectangle_colored(double x, double y, rgb_color_t color, double mass) {
    list_t *shape = create_rectangle_shape(x, y);
    body_t *rect = body_init(shape, mass, color);
    body_set_centroid(rect, VEC_ZERO);
    return rect;
}

body_t *create_wall(double x, double y, double mass) {
    list_t *shape = create_rectangle_shape(x, y);
    rgb_color_t color = rgb_color_init(255, 255, 255);
    body_t *rect = body_init(shape, mass, color);
    return rect;
}

body_t *create_bullet(double x, double y, double mass, bool is_enemy){
    list_t *shape = create_rectangle_shape(x, y);
    rgb_color_t color = rgb_color_neon_green();

    if (is_enemy) {
        color = rgb_color_gray();
    }

    body_t *bullet = body_init(shape, mass, color);
    body_set_centroid(bullet, VEC_ZERO);

    return bullet;
}

list_t *create_enemy_shape(double radius) {
    list_t *semicircle = create_semicircle_shape(radius);
    vector_t *first_point = list_remove(semicircle, 0);

    vector_t *bottom_point = malloc(sizeof(vector_t)); // sharp point at bottom
    vector_t *last_v = list_get(semicircle, list_size(semicircle) - 1);

    bottom_point->x = last_v->x + radius;
    bottom_point->y = last_v->y - radius/2;
    
    list_add(semicircle, bottom_point);
    list_add(semicircle, first_point);

    return semicircle;
}

body_t *create_enemy(double radius, double mass) {
    list_t *shape = create_enemy_shape(radius);

    rgb_color_t color = rgb_color_pastel();
    body_t *enemy = body_init(shape, mass, color);

    return enemy;
}

list_t *create_semicircle_shape(double radius) {
    list_t *semicircle = list_init(CIRCLE_APPROX, (free_func_t) free);
    vector_t *extern_points = malloc(sizeof(vector_t));
    
    for(int i = CIRCLE_APPROX / 4; i < 3 * CIRCLE_APPROX / 4; i++) {
        double angle = (2.0 * M_PI / CIRCLE_APPROX) * i + M_PI;
        extern_points->x = 0;
        extern_points->y = radius;
        vector_t *temp_v = malloc(sizeof(vector_t));
        *temp_v = vec_rotate(*extern_points, angle);
        list_add(semicircle, temp_v);
    }

    return semicircle;
}

list_t *create_circle_shape(double radius) {
    list_t *circle = list_init(CIRCLE_APPROX, (free_func_t) free);
    vector_t *extern_points = malloc(sizeof(vector_t));
    for(int i = 0; i < CIRCLE_APPROX; i++) {
        double angle = 2.0 * M_PI / CIRCLE_APPROX * i;
        extern_points->x = 0;
        extern_points->y = radius;
        vector_t *temp_v = malloc(sizeof(vector_t));
        *temp_v = vec_rotate(*extern_points, angle);
        list_add(circle, temp_v);
    }
    free(extern_points);
    return circle;
}

body_t *create_circle(double radius, double mass) {
    list_t *shape = create_circle_shape(radius);
    rgb_color_t color = rgb_color_pastel();
    body_t *circle = body_init(shape, mass, color);
    return circle;
}

body_t *create_lemniscate(double rad, double min_angle, double max_angle, double mass) {
    list_t *shape = list_init(CIRCLE_APPROX, free);
    for (int i = 0; i < CIRCLE_APPROX; i++) {
        vector_t *point = malloc(sizeof(vector_t));
        double angle = (max_angle - min_angle) / CIRCLE_APPROX * i;
        point->x = (rad * cos(angle))/(1 + pow(sin(angle), 2));
        point->y = (rad * sin(angle) * cos(angle))/(1 + pow(sin(angle), 2));
    }
    rgb_color_t color = rgb_color_gray();
    body_t *lem = body_init(shape, mass, color); // centroid is the center of the infinity sign
    return lem;
}

list_t *create_nstar_shape(int n, double size) {
    list_t *star = list_init(2 * n, free);
    vector_t *vex_point = malloc(sizeof(vector_t));
    vector_t *cave_point = malloc(sizeof(vector_t));
    for(int i = 0; i < n; i++) {
        double angle = 2 * M_PI / n * i;
        vex_point->x = 0;
        vex_point->y = size;
        vector_t *temp_v = malloc(sizeof(vector_t));
        *temp_v = vec_rotate(*vex_point, angle);

        cave_point->x = 0;
        cave_point->y = size / 2; // Make concave points half the length
        vector_t *temp_c = malloc(sizeof(vector_t));
        *temp_c = vec_rotate(*cave_point, angle + M_PI/n);

        list_add(star, temp_v);
        list_add(star, temp_c);
    }
    return star;
}

body_t *create_nstar(int n, double radius, double mass) {
    list_t *shape = create_nstar_shape(n, radius);
    rgb_color_t color = rgb_color_pastel();
    body_t *star = body_init(shape, mass, color);
    return star;
}

vector_t random_vector(vector_t min, vector_t max) {
    double x = (double)rand()/(double)(RAND_MAX/(max.x)) + min.x;
    double y = (double)rand()/(double)(RAND_MAX/(max.y)) + min.y;
    return (vector_t) {.x = x, .y = y};
}
