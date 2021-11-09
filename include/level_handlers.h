#ifndef __LEVEL_HANDLERS_H__
#define __LEVEL_HANDLERS_H__

#include <stdio.h>
#include <stdlib.h>
#include "sdl_wrapper.h"
typedef enum {
    BALL,
    HOLE,
    GRAVITY,
    SAND,
    GRASS,
    WATER,
    BOOST,
    PORTAL,
    BACKGROUND,
} body_type_t;

typedef struct score {
    size_t points;
} score_t;

typedef struct teleport_aux {
    body_t *out;
    vector_t direction;
} teleport_aux_t;

body_type_t *make_type_info(body_type_t type);

body_type_t get_type(body_t *body);

teleport_aux_t *make_teleport_aux(body_t *out, vector_t dir);

void teleport(body_t *ball, body_t *portal, vector_t axis, void *aux);

void level_end(body_t *ball, body_t *hole, vector_t axis, void *aux);

void power_up(body_t *ball, body_t *target, vector_t axis, void *aux);

void sanded(body_t *ball, body_t *target, vector_t axis, void *aux);


/**
 * A function called when a collision occurs.
 * @param scene the game scene
 * @param new_level the level to be generated
 * @return a body pointer to the player object
 */

body_t *build_level(scene_t *scene);

#endif // ifndef __LEVEL_HANDLERS_H__