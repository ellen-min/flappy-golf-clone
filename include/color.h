#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdbool.h>

/**
 * A color to display on the screen.
 * The color is represented by its red, green, and blue components.
 * Each component must be between 0 (black) and 1 (white).
 */
typedef struct {
    float r;
    float g;
    float b;
} rgb_color_t;

/**
 * Returns a random color.
 */
rgb_color_t rgb_color_rand();

rgb_color_t rgb_color_init(double r, double g, double b);

rgb_color_t *rgb_color_list_init(double r, double g, double b);

bool rgb_color_equal(rgb_color_t color1, rgb_color_t color2);

rgb_color_t rgb_color_gray();

rgb_color_t rgb_color_neon_green();

rgb_color_t rgb_color_rainbows(int i);

rgb_color_t rgb_color_yellow();

rgb_color_t rgb_color_red();

rgb_color_t rgb_color_pastel();

#endif // #ifndef __COLOR_H__
