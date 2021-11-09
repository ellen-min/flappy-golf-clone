#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "body.h"
#include "color.h"
#include "list.h"
#include "vector.h"
#include "polygon.h"

typedef struct body {
    list_t *shape;
    double mass;
    rgb_color_t color;
    vector_t centroid;
    vector_t velocity;
    double orientation;
    double bounding_radius;
    list_t *anchors;
    bool collided;

    vector_t force;
    vector_t impulse;
    void *info;
    free_func_t info_freer;
    bool remove;
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
    return body_init_with_info(shape, mass, color, NULL, NULL);
}

body_t *body_init_with_info(
    list_t *shape,
    double mass,
    rgb_color_t color,
    void *info,
    free_func_t info_freer
) {
    assert(mass >= 0);
    body_t *object = malloc(sizeof(body_t));
    assert(object != NULL);
    object->shape = shape;
    object->mass = mass;
    object->color = color;
    object->info = info;
    object->info_freer = info_freer;
    object->anchors = NULL;
    object->collided = false;

    double max_radius = 0;
    for(size_t i = 0; i < list_size(shape); i++) {
        vector_t *vertex = list_get(shape, i);
        double r = vec_norm(*vertex);
        if (r > max_radius) max_radius = r;
    }
    object->bounding_radius = max_radius;

    object->centroid = VEC_ZERO;
    object->velocity = VEC_ZERO;
    object->orientation = (double) 0.0;
    object->remove = false;

    object->force = VEC_ZERO;
    object->impulse = VEC_ZERO;

    return object;
}

void body_free(body_t *body) {
    if (body == NULL){
        return;
    }
    list_free(body->shape);
    free(body->info);
    free(body->anchors);
    free(body);
}

list_t *body_get_shape(body_t *body) {
    list_t *shape_cpy = list_init(10, NULL);
    for(size_t i = 0; i < list_size(body->shape); i++) {
        vector_t *temp = malloc(sizeof(vector_t));
        *temp = *(vector_t*) list_get(body->shape, i);
        list_add(shape_cpy, temp);
    }
    return shape_cpy;
}

bool body_collided(body_t *body) {
    return body->collided;
}

list_t *get_anchors(body_t *body) {
    return body->anchors;
}

vector_t body_get_centroid(body_t *body) {
    return body->centroid;
}

vector_t body_get_velocity(body_t *body) {
    return body->velocity;
}

double body_get_mass(body_t *body) {
    return body->mass;
}

double body_get_bounding_radius(body_t *body) {
    return body->bounding_radius;
}

void body_translate(body_t *body, vector_t v)
{
    body->centroid = vec_add(v, body->centroid);
    polygon_translate(body->shape, v);
}

rgb_color_t body_get_color(body_t *body) {
    return body->color;
}

void *body_get_info(body_t *body) {
    return body->info;
}

void body_set_info(body_t *body, void* info) {
    body->info = info;
}

void body_set_collided(body_t *body, bool c) {
    body->collided = c;
}

void body_set_centroid(body_t *body, vector_t x) {
    vector_t cur_centroid = body->centroid;
    vector_t displacement = vec_subtract(x, cur_centroid);
    polygon_translate(body->shape, displacement);
    body->centroid = x;

    if (body->anchors != NULL) {
        for (size_t i = 0; i < list_size(body->anchors); i++) {
            body_t *curr = list_get(body->anchors, i);
            body_translate(curr, displacement);
        }
    }
}

void body_set_velocity(body_t *body, vector_t v) {
    body->velocity = v;
}

void body_set_rotation(body_t *body, double angle) {
    polygon_rotate(body->shape, -1 * (body->orientation), body->centroid);
    polygon_rotate(body->shape, angle, body->centroid);
    body->orientation = angle;
}

double body_get_rotation(body_t *body) {
    return body->orientation;
}

void body_add_force(body_t *body, vector_t force) { 
    body->force = vec_add(body->force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
    body->impulse = vec_add(body->impulse, impulse);
}

void body_add_anchor(body_t *main_body, body_t *anchored_body) {
    if (main_body->anchors == NULL) {
        main_body->anchors = list_init(1, (free_func_t) body_free);
    }
    list_add(main_body->anchors, anchored_body);
}

list_t *body_get_anchors(body_t *body) {
    return body->anchors;
}

vector_t body_get_impulse(body_t *body) {
    return body->impulse;
}

void body_set_color(body_t *body, rgb_color_t color) {
    body->color = color;
}

void body_tick(body_t *body, double dt) {
    vector_t acceleration = vec_multiply((1.0 / body->mass), body->force);

    vector_t dv = vec_multiply((1.0 / body->mass), body->impulse);
    vector_t old_v = body->velocity;
    vector_t new_v = vec_add(dv, vec_add(body->velocity, vec_multiply(dt, acceleration)));
    
    vector_t avg_v = vec_multiply((0.5), vec_add(old_v, new_v));
    body_set_velocity(body, new_v);

    vector_t dx = vec_multiply(dt, avg_v);
    body_translate(body, dx);

    if(body->anchors != NULL) {
        for(size_t i = 0; i < list_size(body->anchors); i++) {
            body_translate(list_get(body->anchors, i), dx);
            //FIXME: check ball rotations
        }
    }

    body->force = VEC_ZERO;
    body->impulse = VEC_ZERO;
}

void body_remove(body_t *body) {
    body->remove = true;
}

bool body_is_removed(body_t *body) {
    return body->remove;
}
