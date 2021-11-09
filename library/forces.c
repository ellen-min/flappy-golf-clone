#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

#include "sdl_wrapper.h"
#include "forces.h"
#include "body.h"
#include "scene.h"
#include "math.h"
#include "collision.h"
#include "polygon.h"
#include "level_handlers.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_gfxPrimitives.h>


const double MIN_DISTANCE = 5.0;
    
typedef struct drag_aux {
    double gamma;
} drag_aux_t;

typedef struct newtonian_gravity_aux {
    double G;
} newtonian_gravity_aux_t;

typedef struct spring_aux {
    double k;
} spring_aux_t;

typedef struct two_bodies {
    body_t *body1;
    body_t *body2;
} two_bodies_aux_t;

typedef struct collision_aux {
    void* aux;
    collision_handler_t handler;
    free_func_t freer;
    bool collided;
} collision_aux_t;

typedef struct elas {
    double value;
} elas_aux_t;

two_bodies_aux_t *two_bodies_init(body_t *a, body_t *b) {
    two_bodies_aux_t *two_bodies = malloc(sizeof(two_bodies_aux_t));
    assert (two_bodies != NULL);
    two_bodies->body1 = a;
    two_bodies->body2 = b;
    return two_bodies;
}

void collision_aux_free(collision_aux_t* aux) {
    free_func_t freer = aux->freer;
    if(freer != NULL) {
        freer(aux->aux);
    }
    free(aux);
}

drag_aux_t *drag_aux_init(double gamma) {
    drag_aux_t *drag_body = malloc(sizeof(drag_aux_t));
    assert(drag_body != NULL);
    drag_body->gamma = gamma;
    return drag_body;
}

newtonian_gravity_aux_t *newtonian_gravity_aux_init(double G) {
    newtonian_gravity_aux_t *gravity_bodies = malloc(sizeof(newtonian_gravity_aux_t));
    assert(gravity_bodies != NULL);
    gravity_bodies->G = G;
    return gravity_bodies;
}

spring_aux_t *spring_aux_init(double k) {
    spring_aux_t *spring_bodies = malloc(sizeof(spring_aux_t));
    assert(spring_bodies != NULL);
    spring_bodies->k = k;
    return spring_bodies;
}

collision_aux_t *collision_aux_init() {
    collision_aux_t *collision_bodies = malloc(sizeof(collision_aux_t));
    assert(collision_bodies != NULL);
    return collision_bodies;
}

void newtonian_gravity_force_creator(newtonian_gravity_aux_t *aux, list_t *bodies) {
    body_t *body1 = list_get(bodies, 0);
    body_t *body2 = list_get(bodies, 1);
    vector_t centroid1 = body_get_centroid(body1);
    vector_t centroid2 = body_get_centroid(body2);
    double dist = vec_distance(centroid1, centroid2);
    double mass_one = body_get_mass(body1);
    double mass_two = body_get_mass(body2);
    double force = (aux->G * mass_one * mass_two) / (dist * dist);

    if (dist > MIN_DISTANCE) {
        vector_t normal_dir = vec_normalize(vec_subtract(centroid2, centroid1));
        vector_t gravity_force = vec_multiply(force, normal_dir);
        body_add_force(body1, gravity_force);
        body_add_force(body2, vec_negate(gravity_force));
    }
}

void spring_force_creator(spring_aux_t *aux, list_t *bodies) {
    assert(list_size(bodies) == 2);
    body_t *body1 = list_get(bodies, 0);
    body_t *body2 = list_get(bodies, 1);

    vector_t radius = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
    vector_t spring_force = vec_multiply(aux->k, radius);
    body_add_force(body1, spring_force);
    body_add_force(body2, vec_negate(spring_force));
}

void drag_force_creator(drag_aux_t *aux, list_t *bodies) {
    body_t *curr = list_get(bodies, 0);
    vector_t v = body_get_velocity(curr);
    vector_t force = vec_multiply(-1.0 * (aux->gamma), v);
    body_add_force(curr, force);
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2) {
    list_t *bodies = list_init(2, (free_func_t)body_free);
    list_add(bodies, body1);
    list_add(bodies, body2);
    newtonian_gravity_aux_t *newtonian_auxil = newtonian_gravity_aux_init(G);
    scene_add_bodies_force_creator(scene, (force_creator_t) newtonian_gravity_force_creator, 
                                   newtonian_auxil, bodies, (free_func_t) free);
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2) {
    list_t *bodies = list_init(2, (free_func_t)body_free);
    list_add(bodies, body1);
    list_add(bodies, body2);
    spring_aux_t *spring_auxil = spring_aux_init(k);
    scene_add_bodies_force_creator(scene, (force_creator_t) spring_force_creator, spring_auxil, bodies, (free_func_t) free);
}

void create_drag(scene_t *scene, double gamma, body_t *body) {
    list_t *bodies = list_init(1, (free_func_t)body_free);
    list_add(bodies, body);
    drag_aux_t *drag_auxil = drag_aux_init(gamma);
    scene_add_bodies_force_creator(scene, (force_creator_t) drag_force_creator, drag_auxil, bodies, (free_func_t) free);
}

void destructive_collision_handler(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    if (body_is_removed(body1) && body_is_removed(body2)) return;
    body_remove(body1);
    body_remove(body2);
}

void do_impulse(body_t *target, body_t *other, double coeff, vector_t axis) {
    coeff = fabs(coeff);
    vector_t abs_impulse = vec_multiply(coeff, axis);
    vector_t impulse = abs_impulse;
    vector_t d = vec_subtract(body_get_centroid(target), body_get_centroid(other));
    if(vec_dot(d, impulse) < 0) {
        impulse = vec_negate(impulse);
    }
    body_add_impulse(target, impulse);
}

void physics_collision_handler(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    double Cr = ((elas_aux_t *) aux)->value;
    double m1 = body_get_mass(body1);
    double m2 = body_get_mass(body2);
    double u1 = vec_dot(axis, body_get_velocity(body1));
    double u2 = vec_dot(axis, body_get_velocity(body2));
    double coeff;
    vector_t impulse;
    if (m1 == INFINITY) {
        coeff = m2 * (1.0 + Cr) * (u2 - u1);
        impulse = vec_multiply(-coeff, axis);
        body_add_impulse(body2, impulse);
    }
    else if (m2 == INFINITY) {
        coeff = m1 * (1.0 + Cr) * (u2 - u1);
        impulse = vec_multiply(coeff, axis);
        body_add_impulse(body1, impulse);
    }
    else {
        coeff = (m1 * m2) / (m1 + m2) * (1.0 + Cr) * (u2 - u1) + 0.01;
        impulse = vec_multiply(coeff, axis);
        body_add_impulse(body1, impulse);
        body_add_impulse(body2, vec_negate(impulse));
    }
}

void create_collision(
    scene_t *scene,
    body_t *body1,
    body_t *body2,
    collision_handler_t handler,
    void *aux,
    free_func_t freer
) { 
    list_t *bodies = list_init(2, (free_func_t)body_free);
    list_add(bodies, body1);
    list_add(bodies, body2);
    collision_aux_t *col_aux = collision_aux_init();
    col_aux->aux = aux;
    col_aux->handler = handler;
    col_aux->freer = freer;
    scene_add_bodies_force_creator(scene, (force_creator_t) collision_force_creator,
                                   col_aux, bodies, (free_func_t) collision_aux_free);
}

void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2) {
    create_collision(scene, body1, body2, (collision_handler_t) destructive_collision_handler, NULL, NULL);
}

void create_physics_collision(
    scene_t *scene,
    double elasticity,
    body_t *body1,
    body_t *body2
) {
    elas_aux_t *elas = malloc(sizeof(elas_aux_t));
    elas->value = elasticity;
    create_collision(scene, body1, body2, (collision_handler_t) physics_collision_handler, elas, free);
}

void collision_force_creator(collision_aux_t *auxil, list_t *bodies, scene_t *scene) {
    body_t *body1 = list_get(bodies, 0);
    body_t *body2 = list_get(bodies, 1);
    collision_handler_t handler = auxil->handler;
    void *coaux = auxil->aux;
    // Bounding circles to see if collision needs to be checked at all
    double r = body_get_bounding_radius(body1) + body_get_bounding_radius(body2);
    double centroid_dist = vec_norm(vec_subtract(body_get_centroid(body1), body_get_centroid(body2)));
    if(r < centroid_dist) {
        auxil->collided = false;
        return;
    }
    collision_info_t info = find_collision(body_get_shape(body1), body_get_shape(body2));
    if(info.collided) {
        if((get_type(body2) == GRASS)) {
            if(fabs(vec_dot(body_get_velocity(body1), info.axis)) < 100) {
                body_set_velocity(body1, vec_multiply(vec_dot(body_get_velocity(body1), vec_orthogonal(info.axis)), vec_orthogonal(info.axis)));
            }
        }
        if((get_type(body1) == SAND || get_type(body2) == SAND)) {
            if(get_type(body1) == SAND) handler(body2, body1, info.axis, coaux);
            else handler(body1, body2, info.axis, coaux);
            return;
        }
        else if (!auxil->collided) {
            char *teleportsound = "../resources/teleport.wav";
            if(get_type(body2) == PORTAL) {
                sdl_load_sound(NULL, teleportsound, 8, 5);
            }
            handler(body1, body2, info.axis, coaux);
        }
    }
    auxil->collided = info.collided;
}
