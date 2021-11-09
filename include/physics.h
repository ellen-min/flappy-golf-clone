#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "list.h"
#include "color.h"
#include "body.h"

/**
 * Updates velocity of star depending on collision.
 *
 * @param star The colliding body.
 * @param min The vector defining minimum x, y bounds.
 * @param max The vector defining maximum x, y bounds.
 * @param elasticity The ratio of final to initial speed after collision.
 * @return The updated body after a collision.
 */
body_t *collide(body_t *star, vector_t min, vector_t max, double elasticity);

/**
 * Simulates gravity by updating the velocity of a shape.
 *
 * @param star The body under the effect of gravity.
 * @param gravity The acceleration due to gravity.
 * @param dt The change in time from the previous velocity update.
 * @return The updated body after the acceleration of gravity.
 */
body_t *do_gravity(body_t *star, double gravity, double dt);

/**
 * Checks if a body is out of the window.
 *
 * @param star The colliding polygon body.
 * @param min The vector defining minimum x, y bounds.
 * @param max The vector defining maximum x, y bounds.
 * @return True if the shape is off screen. False otherwise.
 */
bool check_offscreen(list_t *star, vector_t min, vector_t max);

#endif // #ifndef __PHYSICS_H__