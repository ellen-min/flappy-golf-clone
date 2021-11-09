#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "vector.h"
#include <math.h>

const vector_t VEC_ZERO = {
    .x = 0.0,
    .y = 0.0
};

vector_t vec_init(double x, double y){
    vector_t v = {.x = x, .y = y};
    return v;
}

vector_t *vec_init_ptr(double x, double y) {
    vector_t *res = malloc(sizeof(vector_t));
    *res = vec_init(x, y);;
    return res;
}

vector_t vec_add(vector_t v1, vector_t v2){
    return vec_init(v1.x + v2.x, v1.y + v2.y);
}

vector_t vec_subtract(vector_t v1, vector_t v2){
    return vec_add(v1, vec_negate(v2));
}

vector_t vec_negate(vector_t v){
    return vec_init(-v.x, -v.y);
}

vector_t vec_multiply(double scalar, vector_t v){
    return vec_init(scalar*v.x, scalar*v.y);
}

double vec_dot(vector_t v1, vector_t v2){
    return (v1.x*v2.x) + (v1.y*v2.y);
}

double vec_cross(vector_t v1, vector_t v2){
    return (v1.x*v2.y) - (v1.y*v2.x);
}

vector_t vec_rotate_external(vector_t v, double angle, vector_t pivot) {
    vector_t ret = vec_subtract(v, pivot);
    ret = (vector_t) {.x = ret.x*cos(angle) - ret.y*sin(angle),
            .y = ret.x*sin(angle) + ret.y*cos(angle)};
    return vec_add(pivot, ret);
}

vector_t vec_rotate(vector_t v, double angle){
    return vec_rotate_external(v, angle, VEC_ZERO);
}

vector_t vec_orthogonal(vector_t v){
    return vec_init(-v.y, v.x);
}

double vec_abs_angle(vector_t v){
    return atan2(v.y, v.x);
}

double vec_norm(vector_t v){
    return sqrt(fabs(vec_dot(v, v)));
}

vector_t vec_normalize(vector_t v){
    return vec_multiply(1.0 / vec_norm(v), v);
}

double vec_distance(vector_t v1, vector_t v2){
    return vec_norm(vec_subtract(v2, v1));
}

bool vec_equals(vector_t v1, vector_t v2) {
    return (vec_norm(vec_subtract(v1, v2)) < 1e-4);
}