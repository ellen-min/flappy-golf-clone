#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "sdl_wrapper.h"
#include "terrain.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_mixer.h>

const char WINDOW_TITLE[] = "CS 3";
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 500;
const double MS_PER_S = 1e3;

/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void) {
    int *width = malloc(sizeof(*width)),
        *height = malloc(sizeof(*height));
    assert(width != NULL);
    assert(height != NULL);
    SDL_GetWindowSize(window, width, height);
    vector_t dimensions = {.x = *width, .y = *height};
    free(width);
    free(height);
    return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center) {
    // Scale scene so it fits entirely in the window
    double x_scale = window_center.x / max_diff.x,
           y_scale = window_center.y / max_diff.y;
    return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center) {
    // Scale scene coordinates by the scaling factor
    // and map the center of the scene to the center of the window
    vector_t scene_center_offset = vec_subtract(scene_pos, center);
    double scale = get_scene_scale(window_center);
    vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
    vector_t pixel = {
        .x = round(window_center.x + pixel_center_offset.x),
        // Flip y axis since positive y is down on the screen
        .y = round(window_center.y - pixel_center_offset.y)
    };
    return pixel;
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key) {
    switch (key) {
        case SDLK_LEFT:  return LEFT_ARROW;
        case SDLK_UP:    return UP_ARROW;
        case SDLK_RIGHT: return RIGHT_ARROW;
        case SDLK_DOWN:  return DOWN_ARROW;
        case SDLK_SPACE: return SPACE;
        case SDLK_q: return Q_CHARACTER;
        default:
            // Only process 7-bit ASCII characters
            return key == (SDL_Keycode) (char) key ? key : '\0';
    }
}

void sdl_init(vector_t min, vector_t max) {
    // Check parameters
    assert(min.x < max.x);
    assert(min.y < max.y);

    center = vec_multiply(0.5, vec_add(min, max));
    max_diff = vec_subtract(max, center);
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    TTF_Init();
    window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );
    assert (window != NULL);

    renderer = SDL_CreateRenderer(window, -1, 0);
}

Mix_Chunk *sdl_load_sound(scene_t *scene, char *filepath, int volume, int channel) {
    Mix_Chunk *sound = NULL;
    int success = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    if (success < 0) {
        printf("Unable to open sound.");
    }
    sound = Mix_LoadWAV(filepath);
    Mix_VolumeChunk(sound, volume);
    Mix_PlayChannel(channel, sound, 0);
    return sound;
}

void sdl_free_sound(Mix_Chunk *sound) {
    Mix_FreeChunk(sound);
    //Mix_Quit();
}

SDL_Texture *sdl_load_image(char *filepath){
    SDL_Texture *img = NULL;
    img = IMG_LoadTexture(renderer, filepath);

    int w, h;
    SDL_QueryTexture(img, NULL, NULL, &w, &h);
    SDL_Rect texr; texr.x = 0; texr.y = 0; texr.w = w/2.15; texr.h = h/2.8;
    SDL_RenderCopy(renderer, img, NULL, &texr);

    return img;
}

void center_display(char *message, int text_height, int font_size, int x_pos, int width, int height, rgb_color_t color){
    SDL_Color textColor = {color.r * 255, color.g * 255, color.b * 255, 255};
    TTF_Font* font = TTF_OpenFont("resources/gamefont.ttf", font_size);

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, message, textColor);
    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect *Message_rect = malloc(sizeof(SDL_Rect));
    int w = textSurface->w;
    int h = textSurface->h;
    Message_rect->x = WINDOW_WIDTH/2 - (w/2);  
    Message_rect->y = text_height; 
    Message_rect->w = w; 
    Message_rect->h = h; 

    SDL_RenderCopy(renderer, text, NULL, Message_rect);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}

void point_display(char *score) {
    SDL_Color textColor = {0, 51, 102, 255};
    TTF_Font* font = TTF_OpenFont("resources/gamefont.ttf", 50);
    if(!font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    }
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, score, textColor);
    int w = textSurface->w;
    int h = textSurface->h;

    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect *Message_rect = malloc(sizeof(SDL_Rect));
    Message_rect->x = 30;
    Message_rect->y = 30;
    Message_rect->w = w;
    Message_rect->h = h;
    SDL_RenderCopy(renderer, text, NULL, Message_rect);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}

bool sdl_is_done(scene_t *scene) {
    SDL_Event *event = malloc(sizeof(*event));
    assert(event != NULL);
    while (SDL_PollEvent(event)) {
        switch (event->type) {
            case SDL_QUIT:
                free(event);
                return true;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                // Skip the keypress if no handler is configured
                // or an unrecognized key was pressed
                if (key_handler == NULL) break;
                char key = get_keycode(event->key.keysym.sym);
                if (key == '\0') break;

                uint32_t timestamp = event->key.timestamp;
                if (!event->key.repeat) {
                    key_start_timestamp = timestamp;
                }
                key_event_type_t type =
                    event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
                double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
                key_handler(key, type, held_time, scene);
                break;
        }
    }
    free(event);
    return false;
}

void sdl_clear(void) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

void sdl_draw_polygon(list_t *points, rgb_color_t color) {
    // Check parameters
    size_t n = list_size(points);
    assert(n >= 3);
    assert(0 <= color.r && color.r <= 1);
    assert(0 <= color.g && color.g <= 1);
    assert(0 <= color.b && color.b <= 1);

    vector_t window_center = get_window_center();

    // Convert each vertex to a point on screen
    int16_t *x_points = malloc(sizeof(*x_points) * n),
            *y_points = malloc(sizeof(*y_points) * n);
    assert(x_points != NULL);
    assert(y_points != NULL);
    for (size_t i = 0; i < n; i++) {
        vector_t *vertex = list_get(points, i);
        vector_t pixel = get_window_position(*vertex, window_center);
        x_points[i] = pixel.x;
        y_points[i] = pixel.y;
    }

    // Draw polygon with the given color
    filledPolygonRGBA(
        renderer,
        x_points, y_points, n,
        color.r * 255, color.g * 255, color.b * 255, 255
    );
    free(x_points);
    free(y_points);
}

void sdl_show(void) {
    // Draw boundary lines
    vector_t window_center = get_window_center();
    vector_t max = vec_add(center, max_diff),
             min = vec_subtract(center, max_diff);
    vector_t max_pixel = get_window_position(max, window_center),
             min_pixel = get_window_position(min, window_center);
    SDL_Rect *boundary = malloc(sizeof(*boundary));
    boundary->x = min_pixel.x;
    boundary->y = max_pixel.y;
    boundary->w = max_pixel.x - min_pixel.x;
    boundary->h = min_pixel.y - max_pixel.y;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, boundary);
    free(boundary);

    SDL_RenderPresent(renderer);
}

void sdl_render_scene(scene_t *scene) {
    sdl_clear();
    int state = scene_get_state(scene);
    if (state == -5) {
        char *filepath = "../resources/intro_img.png";
        scene_set_img(scene, sdl_load_image(filepath));

        center_display(("Welcome to Flappy Golf!"), 60, 40, 320, 400, 100, rgb_color_rainbows(3));

        center_display(("Use left & right arrows to control the ball."), 190, 30, 440, 130, 70, rgb_color_rainbows(0));

        center_display("Press space to start.", 280, 30, 370, 250, 50, rgb_color_rainbows(1));
        center_display("Press 'q' at any time to quit.", 320, 30, 450, 200, 80, rgb_color_rainbows(1));
    }
    else if (state == -1) {
        char *filepath = "../resources/intro_img.png";
        scene_set_img(scene, sdl_load_image(filepath));

        center_display(("You Lost this Level!"), 60, 40, 320, 400, 100, rgb_color_rainbows(3));

        char score_str[50];
        sprintf(score_str, "%zu", scene_get_points(scene));
        char str1[100] = "Flaps: ";
        strcat(str1, score_str);
        center_display(str1, 140, 30, 440, 130, 70, rgb_color_rainbows(1));

        char level_str[50];
        sprintf(level_str, "%zu", scene_get_level(scene));
        char str2[100] = "Level: ";
        strcat(str2, level_str);
        center_display(str2, 190, 30, 450, 200, 80, rgb_color_rainbows(1));

        center_display("Press space to retry.", 260, 30, 370, 250, 50, rgb_color_rainbows(0));

    }
    else if (state == 1) {
        char *filepath = "../resources/intro_img.png";
        scene_set_img(scene, sdl_load_image(filepath));

        body_set_velocity(scene_get_body(scene, 0), VEC_ZERO); //necessary to stop the ball from "rolling" even when game is done.
        center_display(("You Win this Level!"), 60, 40, 320, 400, 100, rgb_color_rainbows(3));
        char score_str[50];
        sprintf(score_str, "%zu", scene_get_points(scene));
        char str1[100] = "Flaps: ";
        strcat(str1, score_str);
        center_display(str1, 140, 30, 440, 130, 70, rgb_color_rainbows(1));

        char level_str[50];
        sprintf(level_str, "%zu", scene_get_level(scene));
        char str2[100] = "Level: ";
        strcat(str2, level_str);
        center_display(str2, 190, 30, 450, 200, 80, rgb_color_rainbows(1));

        center_display("Press space to retry.", 260, 30, 370, 250, 50, rgb_color_rainbows(0));
        center_display("Press up arrow to continue.", 310, 30, 330, 280, 45, rgb_color_rainbows(0));
    }
    else if (state == 0) {
        sdl_clear();
        size_t background_element_count = scene_background_elements(scene);
        for (size_t i = 0; i < background_element_count; i++) {
            body_t *body = scene_get_background_element(scene, i);
            list_t *shape = body_get_shape(body);
            sdl_draw_polygon(shape, body_get_color(body));
            list_free(shape);
        }
        size_t body_count = scene_bodies(scene);
        for (size_t i = 0; i < body_count; i++) {
            body_t *body = scene_get_body(scene, i);
            list_t *shape = body_get_shape(body);
            sdl_draw_polygon(shape, body_get_color(body));
            list_free(shape);
        }
        char score_str[10];
        sprintf(score_str, "%zu", scene_get_points(scene));
        char str1[100] = "Flaps: ";
        strcat(str1, score_str);
        point_display(str1);
    }
    else if (state == 2) {
        char *filepath = "../resources/intro_img.png";
        sdl_load_image(filepath);

        center_display(("You've won all the levels. Good job!"), 60, 40, 320, 400, 100, rgb_color_rainbows(3));

        char score_str[50];
        sprintf(score_str, "%zu", scene_get_points(scene));
        char str1[100] = "Flaps: ";
        strcat(str1, score_str);
        center_display(str1, 140, 30, 440, 130, 70, rgb_color_rainbows(1));

        char level_str[50];
        sprintf(level_str, "%zu", scene_get_level(scene));
        char str2[100] = "Level: ";
        strcat(str2, level_str);
        center_display(str2, 190, 30, 400, 200, 80, rgb_color_rainbows(1));

        center_display("Press space to retry this level.", 260, 30, 370, 250, 50, rgb_color_rainbows(0));
        center_display("Press 'q' to quit.", 310, 30, 330, 280, 45, rgb_color_rainbows(0));
    }
    sdl_show();
}


void sdl_on_key(key_handler_t handler, void *aux) {
    key_handler = handler;
}

double time_since_last_tick(void) {
    clock_t now = clock();
    double difference = last_clock
        ? (double) (now - last_clock) / CLOCKS_PER_SEC
        : 0.0; // return 0 the first time this is called
    last_clock = now;
    return difference;
}
