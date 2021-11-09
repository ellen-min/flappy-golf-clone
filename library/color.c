#include "../include/color.h"
#include "math.h"
#include <stdio.h>
#include "list.h"
#include <stdlib.h>
#include <assert.h>

rgb_color_t rgb_color_rand(){
    rgb_color_t random_color = { 
        .r = drand48(),
        .g = drand48(),
        .b = drand48()
    };
    return random_color;
}

rgb_color_t rgb_color_init(double r, double g, double b){
    rgb_color_t color = {.r = r / 255.0, .g = g / 255.0, .b = b / 255.0};
    return color;
}

rgb_color_t *rgb_color_list_init(double r, double g, double b){
    rgb_color_t *color = malloc(sizeof(rgb_color_t));
    color->r = r / 255.0;
    color->g = g / 255.0;
    color->b = b / 255.0;
    return color;
}

bool rgb_color_equal(rgb_color_t color1, rgb_color_t color2) {
    bool r = color1.r == color2.r;
    bool g = color1.g == color2.g;
    bool b = color1.b == color2.b;
    return (r && g && b);
}

rgb_color_t rgb_color_red() {
    rgb_color_t red = rgb_color_init(255, 0, 0);
    return red;
}

rgb_color_t rgb_color_yellow() {
    rgb_color_t yellow = rgb_color_init(255, 255, 0);
    return yellow;
}

rgb_color_t rgb_color_gray() { 
    rgb_color_t gray = rgb_color_init(105, 105, 105);
    return gray;
}

rgb_color_t rgb_color_neon_green() { 
    rgb_color_t neon_green = rgb_color_init(57, 255, 20);
    return neon_green;
}

rgb_color_t rgb_color_rainbows(int i) {
    list_t *rainbow = list_init(6, (free_func_t) free);
    assert (rainbow != NULL);

    list_add(rainbow, rgb_color_list_init(0, 51, 102));
    list_add(rainbow, rgb_color_list_init(240, 110, 0));
    list_add(rainbow, rgb_color_list_init(130, 130, 130));
    list_add(rainbow, rgb_color_list_init(110, 110, 110));
    list_add(rainbow, rgb_color_list_init(168, 0, 255));

    list_add(rainbow, rgb_color_list_init(226, 240, 203));
    list_add(rainbow, rgb_color_list_init(181, 234, 215));
    list_add(rainbow, rgb_color_list_init(199, 214, 236));
    list_add(rainbow, rgb_color_list_init(199, 206, 234));
    list_add(rainbow, rgb_color_list_init(204, 201, 236));

    rgb_color_t *my_pointer = list_get(rainbow, i);    
    rgb_color_t my_color = {my_pointer->r, my_pointer->g, my_pointer->b};

    list_free(rainbow);
    return my_color;
}

rgb_color_t rgb_color_pastel(){
    list_t *pastels = list_init(3, (free_func_t) free);
    assert (pastels != NULL);

    list_add(pastels, rgb_color_list_init(100, 149, 237)); // blue
    list_add(pastels, rgb_color_list_init(255, 127, 80)); // orange
    list_add(pastels, rgb_color_list_init(222, 49, 99)); // pink

    int index = rand() % 3;
    rgb_color_t *my_pointer = list_get(pastels, index);
    rgb_color_t my_color = *my_pointer;

    list_free(pastels);
    return my_color;
}
