#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "render.h"


/**
 * Generates sand that will have no elasticity and bring the speed of the ball to zero.
 * Use with scene_add_body to add to scene.
 *
 * @param scene the scene in which the sand should be placed
 * @param ball the golf ball
 * @param shape list of vectors where the sand particles belong
 * @return a compound body of the sand terrain that can be added to the scene
 */
list_t *generate_sand(scene_t *scene, body_t *ball, list_t *shape);

list_t *generate_water(scene_t *scene, body_t *ball, list_t *shape);

body_t *generate_boost(scene_t *scene, body_t *ball, list_t *shape);

void generate_level(scene_t *scene, body_t *ball, char* level);

void generate_background(scene_t *scene);

#endif // ifndef __TERRAIN_H__