#ifndef __ELEMENTS_H__
#define __ELEMENTS_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "render.h"

/**
 * Creates a colored golf ball with wings and mass.
 *
 * @param radius the radius of the golf ball, wings have 3 times the radius
 * @param color the RGB color of the golf ball, wings are always light grey
 * @param mass the ball's mass, wings have the same mass (total mass = 3* mass)
 * @return a pointer to the newly created golf ball compoundbody
 */
list_t *create_golf_ball(double radius, rgb_color_t color, double mass, vector_t location);

/**
 * Creates a colored golf ball with wings and mass.
 *
 * @param radius the radius of the target hole
 * @param color the RGB color of the hole, normally black
 * @param mass the hole's mass, can simulate a slope towards it
 * @return a pointer to the newly created golf hole compound body
 */
list_t *create_golf_hole(double radius, rgb_color_t color, double mass);

#endif // ifndef __ELEMENTS_H__