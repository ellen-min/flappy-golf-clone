#ifndef __RENDER_H__
#define __RENDER_H__

#include "scene.h"
#include "color.h"

list_t *create_triangle_shape(double a);

list_t *create_oval_shape(double a, double b);

list_t *create_enemy_shape(double radius);

list_t *create_rectangle_shape(double x, double y);

body_t *create_rectangle(double x, double y, double mass);

body_t *create_rectangle_colored(double x, double y, rgb_color_t color, double mass) ;

body_t *create_wall(double x, double y, double mass);

body_t *create_enemy(double radius, double mass);

body_t *create_bullet(double x, double y, double mass, bool is_enemy);

list_t *create_circle_shape(double radius);

list_t *create_semicircle_shape(double radius);

list_t *create_nstar_shape(int n, double size);

/**
 * Creates a circle object_t
 * @param radius The radius of the circle.
 * @param mass The mass of the star.
 * @return The body of circle.
 */
body_t *create_circle(double radius, double mass);

/**
 * Creates a lemniscate (https://mathworld.wolfram.com/Lemniscate.html) body
 * Note that the infinity (∞) orientation is considered to be at orientation = 0.
 * 
 * @param a The half-radius of the lemniscate
 * @param mass The mass of the lemniscate
 * @param min_angle the starting angle for the lemniscate (≥ 0)
 *                  *** counterclockwise from the +x-axis ***
 * @param max_angle the ending angle (≤ 2π) from the * +x-axis *
 * @return The lemniscate body
 */
body_t *create_lemniscate(double radius, double min_angle, double max_angle, double mass);

/**
 * Creates a n-pointed star.
 *
 * @param n The number of points of the star.
 * @param size The radius of the circumscribing circle.
 * @param mass The mass of the star.
 * @return The body of circle.
 */
body_t *create_nstar(int n, double radius, double mass);

/**
 * Generates a random vector between two bounds.
 * @param min Minimum vector generated
 * @param max Maximum vector generated
 */
vector_t random_vector(vector_t min, vector_t max);

#endif // #ifndef __SCENE_H__
