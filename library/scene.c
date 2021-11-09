#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "forces.h"
#include "scene.h"
#include "math.h"
#include "list.h"
#include "collision.h"
#include "level_handlers.h"
#include "body.h"
#include "SDL2/SDL_mixer.h"
#include <SDL2/SDL_image.h>

const size_t INIT_CAPACITY = 100;
const double PADDING = 0.05;

typedef struct scene {
    list_t *bodies;
    list_t *background_elements;
    list_t *force_bundles;
    size_t points;
    size_t level;
    int state;
    bool first_try;
    vector_t bound;
    list_t *sounds;
    SDL_Texture *image;
} scene_t;

typedef struct force {
    force_creator_t forcer;
    void *aux;
    list_t *bodies;
    free_func_t freer;
} force_bundle_t;

force_bundle_t *force_bundle_init(force_creator_t forcer, void *aux, list_t *bodies, free_func_t freer) {
    force_bundle_t *new_force_bundle = malloc(sizeof(force_bundle_t));
    new_force_bundle->forcer = forcer;
    new_force_bundle->aux = aux;
    new_force_bundle->bodies = bodies;
    new_force_bundle->freer = freer;
    return new_force_bundle;
}

void force_bundle_free(force_bundle_t *force_bundle) {
    force_bundle->freer(force_bundle->aux);
    free(force_bundle->bodies);
    free(force_bundle);
}

list_t *get_force_bundle_bodies(force_bundle_t *force_bundle) {
    return force_bundle->bodies;
}

force_creator_t get_force_bundle_forcer(force_bundle_t *force_bundle) {
    return force_bundle->forcer;
}

scene_t *scene_init(void) {
    scene_t *scene = malloc(sizeof(scene_t));
    assert(scene != NULL);
    scene->bodies = list_init(INIT_CAPACITY, (free_func_t) body_free);
    scene->background_elements = list_init(INIT_CAPACITY, (free_func_t) body_free);
    scene->force_bundles = list_init(INIT_CAPACITY, (free_func_t) force_bundle_free);
    scene->points = 0;
    scene->state = -5;
    scene->bound = (vector_t) {.x = 2000, .y = 1000};
    scene->level = 1;
    scene->first_try = true;
    scene->sounds = list_init(INIT_CAPACITY, (free_func_t) sdl_free_sound);

    char *filepath = "../resources/intro.wav";
    sdl_load_sound(scene, filepath, 8, 7);
    
    return scene;
}

void scene_free(scene_t *scene) {
    list_free(scene->bodies);
    list_free(scene->force_bundles);
    list_free(scene->sounds);
    SDL_DestroyTexture(scene->image);
    IMG_Quit();
    SDL_Quit();
    Mix_Quit();
    free(scene);
}

void scene_add_sound(scene_t *scene, Mix_Chunk *sound) {
    list_add(scene->sounds, sound);
}

size_t scene_bodies(scene_t *scene) {
    return list_size(scene->bodies);
}

size_t scene_background_elements(scene_t *scene) {
    return list_size(scene->background_elements);
}

body_t *scene_get_body(scene_t *scene, size_t index) {
    assert(index < scene_bodies(scene));
    return list_get(scene->bodies, index);
}

body_t *scene_get_background_element(scene_t *scene, size_t index) {
    assert(index < list_size(scene->background_elements));
    return list_get(scene->background_elements, index);
}

list_t *scene_get_force_bundles(scene_t *scene) {
    return scene->force_bundles;
}

size_t scene_get_points(scene_t *scene) {
    return scene->points;
}

size_t scene_get_level(scene_t *scene) {
    return scene->level;
}

int scene_get_state(scene_t *scene) {
    return scene->state;
}

vector_t scene_get_bound(scene_t *scene) {
    return scene->bound;
}

void scene_add_body(scene_t *scene, body_t *body) {
    list_add(scene->bodies, body);
}

void scene_set_img(scene_t *scene, SDL_Texture *img){
    scene->image = img;
}

SDL_Texture *scene_get_img(scene_t *scene) {
    return scene->image;
}

void scene_add_background_element(scene_t *scene, body_t *body) {
    list_add(scene->background_elements, body);
}

size_t scene_add_point(scene_t *scene) {
    scene->points++;
    return scene->points;
}

bool scene_set_first_try(scene_t *scene, bool retry_first_level)
{
    scene->first_try = false;
    return scene->first_try;
}

bool scene_get_first_try(scene_t *scene)
{
    return scene->first_try;
}

size_t scene_set_points(scene_t *scene, size_t point) {
    scene->points = point;
    return scene->points;
}

void reset_scene(scene_t *scene) {
    body_t *player = list_remove(scene->bodies, 0);
    list_free(scene->bodies);
    list_free(scene->force_bundles);
    scene->bodies = list_init(INIT_CAPACITY, (free_func_t) body_free);
    list_add(scene->bodies, player);
    scene->force_bundles = list_init(INIT_CAPACITY, (free_func_t) force_bundle_free);
    scene->points = 0;
}

size_t scene_add_level(scene_t *scene) {
    reset_scene(scene);
    scene->level++;
    build_level(scene);
    return scene->level;
}

int scene_set_state(scene_t *scene, int state) {
    scene->state = state;
    return scene->state;
}

size_t scene_set_level(scene_t *scene, size_t level) {
    scene->level = level;
    return scene->level;
}

void scene_remove_body(scene_t *scene, size_t index) {
    assert(index <= list_size(scene->bodies));
    body_t *removed = list_remove(scene->bodies, index);
    body_free(removed);
}

void scene_add_force_creator(
    scene_t *scene,
    force_creator_t forcer,
    void *aux,
    free_func_t freer
) {
    scene_add_bodies_force_creator(scene, forcer, aux, NULL, freer);
}

void scene_add_bodies_force_creator(
    scene_t *scene,
    force_creator_t forcer,
    void *aux,
    list_t *bodies,
    free_func_t freer
) {
    force_bundle_t *new_force_bundle = force_bundle_init(forcer, aux, bodies, freer);
    list_add(scene->force_bundles, new_force_bundle);
}

void scene_tick(scene_t *scene, double dt) { // Adding the force creators
    for (size_t i = 0; i < list_size(scene->force_bundles); i++) {
        force_bundle_t *curr_force_bundle = list_get(scene->force_bundles, i);
        force_creator_t curr_force_creator = curr_force_bundle->forcer;
        
        for (size_t j = 0; j < list_size(curr_force_bundle->bodies); j++) {
            // Single Bodies
            if(curr_force_creator == (force_creator_t) drag_force_creator) {
                list_t *body_single = list_init(2, free);
                list_add(body_single, list_get(curr_force_bundle->bodies, j));
                curr_force_creator(curr_force_bundle->aux, body_single);
            }
            // Multiple bodies
            else{
                list_t *body_pair = list_init(2, free);
                list_add(body_pair, list_get(curr_force_bundle->bodies, j));
                list_add(body_pair, list_get(curr_force_bundle->bodies, (j+1) % list_size(curr_force_bundle->bodies)));
                curr_force_creator(curr_force_bundle->aux, body_pair);
            }

        }
    }
    body_t *ball = scene_get_body(scene, 0);
    vector_t center = vec_multiply(0.5, scene_get_bound(scene));
    vector_t ball_disp;
    
    for (size_t i = 0; i < scene_bodies(scene); i++) { // Remove bodies & forces if necessary
        body_t *curr_body = scene_get_body(scene, i);
        if (body_is_removed(curr_body)) {
            for (size_t j = 0; j < list_size(scene->force_bundles); j++) {
                force_bundle_t *force_bundle = list_get(scene->force_bundles, j);
                list_t *bodies = force_bundle->bodies;
                if (list_contains(bodies, curr_body)) {
                    force_bundle_free(list_remove(scene->force_bundles, j)); 
                    j--;
                }
            }
            list_remove(scene->bodies, i);
            i--;
            continue;
        }
        body_tick(curr_body, dt);
        if (i == 0) {
            ball_disp = vec_subtract(center, body_get_centroid(ball));
        }
        body_translate(curr_body, vec_multiply(PADDING, ball_disp));
        body_set_collided(curr_body, false);
    }

    for (size_t i = 0; i < list_size(scene->background_elements); i++) {
        body_t *curr_body = scene_get_background_element(scene, i);
        body_translate(curr_body, vec_multiply(PADDING / 5, ball_disp)); // Parallax effect
    }
}