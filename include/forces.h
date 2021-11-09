#ifndef __FORCES_H__
#define __FORCES_H__

#include "scene.h"

/**
 * A function called when a collision occurs.
 * @param body1 the first body passed to create_collision()
 * @param body2 the second body passed to create_collision()
 * @param axis a unit vector pointing from body1 towards body2
 *   that defines the direction the two bodies are colliding in
 * @param aux the auxiliary value passed to create_collision()
 */
typedef void (*collision_handler_t)
    (body_t *body1, body_t *body2, vector_t axis, void *aux);

typedef struct drag_aux drag_aux_t;
typedef struct newtonian_gravity_aux newtonian_gravity_aux_t;
typedef struct spring_aux spring_aux_t;
typedef struct two_bodies two_bodies_aux_t;
typedef struct collision_aux collision_aux_t;

two_bodies_aux_t *two_bodies_init(body_t *a, body_t *b);

/**
 * Initializes arguments necessary to call a force_creator_t function on one body.
 * 
 * @param gamma constant used for force calculation (drag)
 * @param body body to call function
*/
drag_aux_t *drag_aux_init(double gamma);

/**
 * Represents arguments necessary to call a force_creator_t function on two bodies.
 * 
 * @param G constant used for force calculation (newtonian gravity)
 * @param body1 the first body
 * @param body2 the second body
*/
newtonian_gravity_aux_t *newtonian_gravity_aux_init(double G);

/**
 * Represents arguments necessary to call a force_creator_t function on two bodies.
 * 
 * @param k constant used for force calculation (spring constant)
 * @param body1 the first body
 * @param body2 the second body
*/
spring_aux_t *spring_aux_init(double k);

/**
 * Represents arguments necessary to call a force_creator_t function on two bodies.
 * @deprecated
*/
collision_aux_t *collision_aux_init();

/**
 * Applies the gravitational force on each body 
 * This is the force_creator_t needed to add force to scene in create_newtonian_gravity.
 * 
 * @param bodies two bodies to apply gravity between
 */
void newtonian_gravity_force_creator(newtonian_gravity_aux_t *aux, list_t *bodies);

/**
 * Applies the spring force on each body 
 * This is the force_creator_t needed to add force to scene in create_spring.
 * 
 * @param bodies two bodies to apply spring force between
 */
void spring_force_creator(spring_aux_t *aux, list_t *bodies);

/**
 * Applies the drag force on the body
 * This is the force_creator_t needed to add force to scene in create_drag.
 * 
 * @param body the body to apply drag force to
 */
void drag_force_creator(drag_aux_t *aux, list_t *bodies);


/**
 * Applies the collision force on the body
 * This is the force_creator_t needed to add force to scene in create_collision.
 * 
 * @param bodies the body to apply collision force to
 */
void collision_force_creator(collision_aux_t *aux, list_t *bodies, scene_t *scene);

/**
 * Adds a force creator to a scene that applies gravity between two bodies.
 * The force creator will be called each tick
 * to compute the Newtonian gravitational force between the bodies.
 * See https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form.
 * The force should not be applied when the bodies are very close,
 * because its magnitude blows up as the distance between the bodies goes to 0.
 *
 * @param scene the scene containing the bodies
 * @param G the gravitational proportionality constant
 * @param body1 the first body
 * @param body2 the second body
 */
void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2);

/**
 * Adds a force creator to a scene that acts like a spring between two bodies.
 * The force creator will be called each tick
 * to compute the Hooke's-Law spring force between the bodies.
 * See https://en.wikipedia.org/wiki/Hooke%27s_law.
 *
 * @param scene the scene containing the bodies
 * @param k the Hooke's constant for the spring
 * @param body1 the first body
 * @param body2 the second body
 */
void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2);

/**
 * Adds a force creator to a scene that applies a drag force on a body.
 * The force creator will be called each tick
 * to compute the drag force on the body proportional to its velocity.
 * The force points opposite the body's velocity.
 *
 * @param scene the scene containing the bodies
 * @param gamma the proportionality constant between force and velocity
 *   (higher gamma means more drag)
 * @param body the body to slow down
 */
void create_drag(scene_t *scene, double gamma, body_t *body);

/**
 * Adds a force creator to a scene that calls a given collision handler
 * function each time two bodies collide.
 * This generalizes create_destructive_collision() from last week,
 * allowing different things to happen on a collision.
 * The handler is passed the bodies, the collision axis, and an auxiliary value.
 * It should only be called once while the bodies are still colliding.
 *
 * @param scene the scene containing the bodies
 * @param body1 the first body
 * @param body2 the second body
 * @param handler a function to call whenever the bodies collide
 * @param aux an auxiliary value to pass to the handler
 * @param freer if non-NULL, a function to call in order to free aux
 */
void create_collision(
    scene_t *scene,
    body_t *body1,
    body_t *body2,
    collision_handler_t handler,
    void *aux,
    free_func_t freer
);

/**
 * Adds a force creator to a scene that destroys two bodies when they collide.
 * The bodies should be destroyed by calling body_remove().
 * This should be represented as an on-collision callback
 * registered with create_collision().
 *
 * @param scene the scene containing the bodies
 * @param body1 the first body
 * @param body2 the second body
 */
void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2);

/**
 * Adds a force creator to a scene that applies impulses
 * to resolve collisions between two bodies in the scene.
 * This should be represented as an on-collision callback
 * registered with create_collision().
 *
 * You may remember from project01 that you should avoid applying impulses
 * multiple times while the bodies are still colliding.
 * You should also have a special case that allows either body1 or body2
 * to have mass INFINITY, as this is useful for simulating walls.
 *
 * @param scene the scene containing the bodies
 * @param elasticity the "coefficient of restitution" of the collision;
 * 0 is a perfectly inelastic collision and 1 is a perfectly elastic collision
 * @param body1 the first body
 * @param body2 the second body
 */
void create_physics_collision(
    scene_t *scene,
    double elasticity,
    body_t *body1,
    body_t *body2
);

#endif // #ifndef __FORCES_H__
