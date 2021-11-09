#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "polygon.h"
#include "physics.h"
#include "color.h"

body_t *collide(body_t *star, vector_t min, vector_t max, double elasticity) {
    vector_t new_v = body_get_velocity(star);
    bool collided = false;
    // Check each point to see if it crosses the border
    for (size_t i = 0; i < list_size(body_get_shape(star)); i++) {
        vector_t *point = list_get(body_get_shape(star), i);
        if (point->x > max.x) { // Crosses right
            new_v.x = fabs(body_get_velocity(star).x) * -elasticity;
            collided = true;
        }
        if (point->x < min.x) { // Crosses Left
            new_v.x = fabs(body_get_velocity(star).x) * elasticity;
            collided = true;
        }
        if (point->y > max.y) { // Crosses Up
            new_v.y = fabs(body_get_velocity(star).y) * -elasticity;
            collided = true;
        }
        if (point->y < min.y) { // Crosses Down
            new_v.y = fabs(body_get_velocity(star).y) * elasticity;
            collided = true;
        }
        if (collided) {
            break;
        }
    }
    body_set_velocity(star, new_v);
    return star;
}

body_t *do_gravity(body_t *star, double gravity, double dt) {
    vector_t new_v = {.x = body_get_velocity(star).x, .y = body_get_velocity(star).y - gravity * dt};
    body_set_velocity(star, new_v);
    return star;
}

bool check_offscreen(list_t *star, vector_t min, vector_t max){
    for(size_t i = 0; i < list_size(star); i++){
        vector_t *point = list_get(star, i);
        if(point->x < max.x && point->x > min.x && point->y < max.y && point->y > min.y){
            return false;
        }
    }
    return true;
}