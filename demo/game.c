#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include "assert.h"
#include "list.h"
#include "vector.h"
#include "sdl_wrapper.h"
#include "scene.h"
#include "forces.h"
#include "body.h"
#include "color.h"
#include "render.h"
#include "collision.h"
#include "polygon.h"
#include "elements.h"
#include "physics.h"
#include "terrain.h"
#include "level_handlers.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_gfxPrimitives.h>

const int NUM_LEVELS = 7;

const vector_t SCREEN_SIZE = {.x = 2000, .y = 1000};
const vector_t PLAYER_SPEED = {.x = 500, .y = 700};

const double MASS = 10;
const double RADIUS = 10;
const double PLAYER_POS = 20;
const double LARGE_MASS = INFINITY;
const double WALL_THICKNESS = 10;
const double TRAIL_SIZE = 6;
const double GRAV_VAL = 1800;
size_t LEVEL = 1;

void handler(char key, key_event_type_t type, double held_time, scene_t *scene) {
    body_t *golfball = scene_get_body(scene, 0);
    char *filepath = "../resources/popsound.wav";
    if (type == KEY_PRESSED) {
        if (key == RIGHT_ARROW) {
            if (scene_get_state(scene) == 0) {
                vector_t right_v = PLAYER_SPEED;
                body_translate(golfball, vec_init(0, 10));
                body_set_velocity(golfball, right_v);
                scene_add_point(scene);
                sdl_load_sound(scene, filepath, 50, 4);
            }
        }
        else if (key == LEFT_ARROW) {
            if (scene_get_state(scene) == 0) {
                vector_t left_v = {.x = -1.0*PLAYER_SPEED.x, .y = PLAYER_SPEED.y};
                body_translate(golfball, vec_init(0, 10));
                body_set_velocity(golfball, left_v);
                scene_add_point(scene);
                sdl_load_sound(scene, filepath, 50, 4);
            }
        }
        else if (key == UP_ARROW) {
            if (scene_get_state(scene) == -1) {
                scene_set_state(scene, -1);
            }
            else {
                if (scene_get_level(scene) == NUM_LEVELS && scene_get_state(scene) == 1) {
                    scene_set_state(scene, 2);
                }
                else if (scene_get_state(scene) == 1) {
                    scene_add_level(scene);
                    scene_set_points(scene, 0);
                    body_set_velocity(golfball, VEC_ZERO);
                    scene_set_state(scene, 0);
                }
            }  
        }
        else if (key == SPACE) {
            Mix_HaltChannel(7);
            if (scene_get_state(scene) == -5) {
                scene_set_state(scene, 0);
            }
            else {
                if(scene_get_state(scene) == -1 || 1) {
                    body_set_velocity(golfball, VEC_ZERO);
                    if(scene_get_level(scene) == 1) {
                        scene_set_level(scene, 1);
                        reset_scene(scene);
                        build_level(scene);
                    }
                    else {
                        scene_set_level(scene, scene_get_level(scene) - 1);
                        scene_add_level(scene);
                    }
                    scene_set_points(scene, 0);
                    scene_set_state(scene, 0);
                }  
            }
        }
        else if (key == Q_CHARACTER) {
            scene_free(scene);
            SDL_Quit();
            exit(0);
        }
    }
}

int main(int argc, char *argv[]) {
    sdl_init(VEC_ZERO, SCREEN_SIZE);

    scene_t *scene = scene_init();

    body_t *player = build_level(scene);
    sdl_on_key(handler, scene);
    double clock = 0.0;
    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        clock += dt;
        if (scene_get_state(scene) == 0) {
            do_gravity(player, GRAV_VAL, dt);
        }
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
}
