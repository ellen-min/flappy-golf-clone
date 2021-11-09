#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "render.h"
#include "collision.h"
#include "forces.h"
#include "color.h"
#include "level_handlers.h"
#include "elements.h"
#include "cJSON.h"

#define G 6.67E-11 // N m^2 / kg^2
#define M 6E24 // kg
#define ga 9.8 // m / s^2
#define R (sqrt(G * M / ga)) // m

const rgb_color_t WALL_COLOR = {.r = 0.0, .g = 0.0, .b = 0.0};

const int SAND_LENGTH = 5;
const int SAND_HEIGHT = 20;
const rgb_color_t SAND_COLOR = {.r = .761, .g = .698, .b = .502};

const double BUFFER = 1000;
const double WATER_LENGTH = 10;
const double WATER_HEIGHT = 50;
const double HOLE_RADIUS = 30;
const rgb_color_t WATER_COLOR = {.r = .196, .g = 0.666, .b = 0.8117};
const rgb_color_t GRASS_COLOR = {.r = 0.278, .g = .722, .b = 0.408};
const rgb_color_t SKY_COLOR = {.r = 0.651, .g = 0.914, .b = 0.953};
const rgb_color_t MOUNTAIN_COLOR = {.r = 0.294, .g = 0.431, .b = 0.318};
const rgb_color_t SNOW_COLOR = {.r = 0.717, .g = 0.968, .b = 0.960};
const rgb_color_t T_IN_COLOR = {.r = .1, .g = .913, .b = 0.886};
const rgb_color_t T_OUT_COLOR = {.r = 1.0, .g = .6, .b = 0.2};

const double SAND_ELAS = 0.0;
const double GRASS_ELAS = 0.7;

const double LV1_HOLE_X = 1975;
const double LV1_HOLE_Y= 13;

list_t *generate_grass_shape(){
    list_t *vertices = list_init(3, free);

    list_add(vertices, vec_init_ptr(0, 0));
    list_add(vertices, vec_init_ptr(0, 500));
    list_add(vertices, vec_init_ptr(2000, 0));
    return vertices;
}

body_t *generate_grass(scene_t *scene, body_t *ball, list_t *shape) {
    body_t *grass = body_init_with_info(shape, INFINITY, GRASS_COLOR, make_type_info(GRASS), free);
    create_physics_collision(scene, GRASS_ELAS, ball, grass);
    return grass;
}

body_t *generate_water(scene_t *scene, body_t *ball, list_t *shape) {
    body_t *water = body_init_with_info(shape, INFINITY, WATER_COLOR, make_type_info(WATER), free);
    create_collision(scene, ball, water, level_end, scene, NULL);
    return water;
}

body_t *generate_sand(scene_t *scene, body_t *ball, list_t *shape) {
    body_t *sand = body_init_with_info(shape, INFINITY, SAND_COLOR, make_type_info(SAND), free);
    create_collision(scene, ball, sand, sanded, NULL, NULL);
    return sand;
}

body_t *generate_boost(scene_t *scene, body_t *ball, list_t *shape) {
    body_t *boost = body_init_with_info(shape, INFINITY, rgb_color_pastel(), make_type_info(BOOST), free);
    create_collision(scene, ball, boost, power_up, NULL, NULL);
    return boost;
}

body_t *generate_portals(scene_t *scene, body_t *ball, list_t *shape, list_t *out_shape, vector_t dir) {
    body_t *in = body_init_with_info(shape, INFINITY, T_IN_COLOR, make_type_info(PORTAL), free);
    body_t *out = body_init_with_info(out_shape, INFINITY, T_OUT_COLOR, make_type_info(PORTAL), free);
    scene_add_body(scene, out);
    teleport_aux_t *aux = make_teleport_aux(out, dir);
    create_collision(scene, ball, in, teleport, aux, free);
    return in;
}

body_t *get_gravity_body(scene_t *scene) {
    // Will be offscreen, so shape is irrelevant
    list_t *gravity_ball = create_rectangle_shape(1, 1);
    body_t *body = body_init_with_info(
        gravity_ball,
        M,
        WALL_COLOR,
        make_type_info(GRAVITY),
        free
    );

    // Move a distnace R below the scene
    vector_t gravity_center = {.x = 500, .y = -R};
    body_set_centroid(body, gravity_center);
    return body;
}

void set_frame(scene_t *scene, body_t *ball, vector_t size) {
    body_t *left = generate_grass(scene, ball, create_rectangle_shape(BUFFER, size.y + 2 * BUFFER));
    body_set_centroid(left, vec_init(-BUFFER, 0));
    body_t *right = generate_grass(scene, ball, create_rectangle_shape(BUFFER, size.y + 2 * BUFFER));
    body_set_centroid(right, vec_init(size.x, 0));

    body_t *top = generate_grass(scene, ball, create_rectangle_shape(size.x, BUFFER));
    body_set_centroid(top, vec_init(0, size.y + BUFFER));
    body_t *bottom = generate_grass(scene, ball, create_rectangle_shape(size.x + 2 * BUFFER, BUFFER));
    body_set_centroid(bottom, vec_init(-BUFFER, -BUFFER));

    scene_add_body(scene, left);
    scene_add_body(scene, right);
    scene_add_body(scene, top);
    scene_add_body(scene, bottom);
}

char* read_file(char *path) {
    FILE *f = fopen(path, "r");
    
    if (f == NULL) {
        return NULL;
    }

    char *result = malloc(sizeof(char));
    char *res = malloc(100);
    res[0] = '\0'; // initialize longest word
    size_t current_length = 0;

    int successful = fread(result, 1, 1, f); // reads first character
    // fread in while as before
    while (successful == 1) {
        res[current_length] = result[0];
        res = realloc(res, current_length + 100);
        current_length++;
        successful = fread(result, 1, 1, f);
    }
    res[current_length] = '\0';
    free(result);
    fclose(f);
    return res;
}

void generate_level(scene_t *scene, body_t *ball, char* level) {
    char *data = read_file(level);
    const cJSON *bounds = NULL;
    const cJSON *objects = NULL;
    const cJSON *object = NULL;
    int status = 0;
    cJSON *monitor_json = cJSON_Parse(data);
    if (monitor_json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
        goto end;
    }
    bounds = cJSON_GetObjectItemCaseSensitive(monitor_json, "bounds");
    cJSON *width = cJSON_GetObjectItemCaseSensitive(bounds, "width");
    cJSON *height = cJSON_GetObjectItemCaseSensitive(bounds, "height");
    vector_t bound_val = vec_init(width->valuedouble, height->valuedouble);

    set_frame(scene, ball, bound_val);

    objects = cJSON_GetObjectItemCaseSensitive(monitor_json, "objects");
    cJSON_ArrayForEach(object, objects)
    {
        cJSON *type_p = cJSON_GetObjectItemCaseSensitive(object, "type");
        cJSON *pos_x_p = cJSON_GetObjectItemCaseSensitive(object, "pos_x");
        cJSON *pos_y_p = cJSON_GetObjectItemCaseSensitive(object, "pos_y");
        cJSON *shape_p = NULL;
        list_t *shape = NULL;
        char* type = type_p->valuestring;
        double pos_x = pos_x_p->valuedouble;
        double pos_y = pos_y_p->valuedouble;
        
        if(strcmp(type, "GRASS") == 0 || strcmp(type, "WATER") == 0 ||
           strcmp(type, "SAND") == 0 || strcmp(type, "TELEPORT") == 0) {
            shape_p = cJSON_GetObjectItemCaseSensitive(object, "shape");
            cJSON *vertex_p = NULL;
            shape = list_init(5, free);
            cJSON_ArrayForEach(vertex_p, shape_p)
            {
                cJSON *x_p = cJSON_GetObjectItemCaseSensitive(vertex_p, "x");
                cJSON *y_p = cJSON_GetObjectItemCaseSensitive(vertex_p, "y");
                vector_t *vertex = vec_init_ptr(x_p->valuedouble, y_p->valuedouble);
                list_add(shape, vertex);
            }
        }
        // BALL, GRASS, CIRCLE_GRASS, POWER, WATER, SAND, HOLE, TELEPORT_IN, TELEPORT_OUT
        if(strcmp(type, "BALL") == 0) {
            body_set_centroid(ball, vec_init(pos_x, pos_y));
        }
        else if(strcmp(type, "HOLE") == 0) {
            list_t *hole_elements = create_golf_hole(HOLE_RADIUS, rgb_color_gray(), INFINITY);
            body_t *hole_bound = list_get(hole_elements, 0);
            body_set_centroid(hole_bound, vec_init(pos_x, pos_y));
            create_collision(scene, ball, hole_bound, level_end, scene, NULL);
            for (size_t j = 0; j < list_size(hole_elements); j++) {
                scene_add_body(scene, list_get(hole_elements, j));
            }
            free(hole_elements);
        }
        else if(strcmp(type, "GRASS") == 0) {
            body_t *grass = generate_grass(scene, ball, shape);
            scene_add_body(scene, grass);
        }
        else if(strcmp(type, "CIRCLE_GRASS") == 0) {
            cJSON *radius_p = cJSON_GetObjectItemCaseSensitive(object, "radius");
            double radius = radius_p->valuedouble;
            body_t *grass = generate_grass(scene, ball, create_circle_shape(radius));
            body_set_centroid(grass, vec_init(pos_x, pos_y));
            scene_add_body(scene, grass);
        }
        else if(strcmp(type, "WATER") == 0) {
            body_t *water = generate_water(scene, ball, shape);
            scene_add_body(scene, water);
        }
        else if(strcmp(type, "SAND") == 0) {
            body_t *sand = generate_sand(scene, ball, shape);
            scene_add_body(scene, sand);
        }
        else if(strcmp(type, "POWER") == 0) {
            body_t* powerup = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
            body_set_centroid(powerup, vec_init(pos_x, pos_y));
            scene_add_body(scene, powerup);
        }
        else if(strcmp(type, "TELEPORT") == 0) {
            cJSON *out_p = cJSON_GetObjectItemCaseSensitive(object, "out");
            cJSON *vertex_p = NULL;
            list_t *out_shape = list_init(5, free);
            cJSON_ArrayForEach(vertex_p, out_p)
            {
                cJSON *x_p = cJSON_GetObjectItemCaseSensitive(vertex_p, "x");
                cJSON *y_p = cJSON_GetObjectItemCaseSensitive(vertex_p, "y");
                vector_t *vertex = vec_init_ptr(x_p->valuedouble, y_p->valuedouble);
                list_add(out_shape, vertex);
            }

            cJSON *dir_p = cJSON_GetObjectItemCaseSensitive(object, "direction");
            cJSON *dir_x = cJSON_GetObjectItemCaseSensitive(dir_p, "x");
            cJSON *dir_y = cJSON_GetObjectItemCaseSensitive(dir_p, "y");
            vector_t dir = vec_init(dir_x->valuedouble, dir_y->valuedouble);

            body_t *in = generate_portals(scene, ball, shape, out_shape, dir);
            scene_add_body(scene, in);
        }
    }
    goto end;

    end:
        free(data);
        cJSON_Delete(monitor_json);
}

void generate_background(scene_t *scene) {
    char *data = read_file("resources/background.txt");
    const cJSON *objects = NULL;
    const cJSON *object = NULL;
    int status = 0;
    cJSON *monitor_json = cJSON_Parse(data);
    if (monitor_json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
        goto end;
    }

    objects = cJSON_GetObjectItemCaseSensitive(monitor_json, "objects");

    cJSON_ArrayForEach(object, objects) {
        cJSON *type_p = cJSON_GetObjectItemCaseSensitive(object, "type");
        char* type = type_p->valuestring;
        list_t *shape = list_init(5, free);
        cJSON *shape_p = cJSON_GetObjectItemCaseSensitive(object, "shape");
        cJSON *vertex_p = NULL;
        shape = list_init(5, free);
        cJSON_ArrayForEach(vertex_p, shape_p)
        {
            cJSON *x_p = cJSON_GetObjectItemCaseSensitive(vertex_p, "x");
            cJSON *y_p = cJSON_GetObjectItemCaseSensitive(vertex_p, "y");
            vector_t *vertex = vec_init_ptr(x_p->valuedouble, y_p->valuedouble);
            list_add(shape, vertex);
        }
        rgb_color_t color = rgb_color_init(0, 0, 0);
        if(strcmp(type, "SKY") == 0) {
            color = SKY_COLOR;
        }
        if(strcmp(type, "MOUNTAIN") == 0) {
            color = MOUNTAIN_COLOR;
        }
        if(strcmp(type, "SNOW") == 0) {
            color = SNOW_COLOR;
        }
        body_t *cbody = body_init_with_info(shape, INFINITY, color, make_type_info(BACKGROUND), free);
        scene_add_background_element(scene, cbody);
    }

    end:
        free(data);
        cJSON_Delete(monitor_json);
}
