#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "list.h"
#include "vector.h"

double signed_polygon_area(list_t *polygon){
    // Use formula: 1/2 |(sum_(i=1)^(n) v_i x v_(i+1))|
    double result = 0;
    size_t size = list_size(polygon);
    for(size_t i = 0; i < size; i++){
        vector_t *i_poly = (vector_t *)list_get(polygon, i);
        vector_t *iplusone = (vector_t *)list_get(polygon, (i+1) % size);
        result += vec_cross(*i_poly, *iplusone);
    }
    return 0.5 * result;
}

double polygon_area(list_t *polygon){
    return fabs(signed_polygon_area(polygon));
}

vector_t polygon_centroid(list_t *polygon){
    double area = signed_polygon_area(polygon);
    double x = 0;
    double y = 0;
    size_t size = list_size(polygon);
    for(size_t i = 0; i < size; i++){
        size_t j = (i+1) % size;
        vector_t *p_i = (vector_t*) list_get(polygon, i);
        vector_t *p_j = (vector_t*) list_get(polygon, j);
        double cross = vec_cross(*p_i, *p_j);
        x += (p_i->x + p_j->x) * cross;
        y += (p_i->y + p_j->y) * cross;
    }
    
    x = x / (6 * area);
    y = y / (6 * area);

    vector_t result = {.x = x, .y = y};
    return result;
}

void polygon_translate(list_t *polygon, vector_t translation){
    size_t size = list_size(polygon);
    for(size_t i = 0; i < size; i++){
        vector_t translated = vec_add(*(vector_t*) list_get(polygon, i), translation);
        *(vector_t*) list_get(polygon, i) = translated;
    }
}

void polygon_rotate(list_t *polygon, double angle, vector_t point){
    // Algorithm: Shift point of rotation to origin, perform rotation around origin, shift back
    polygon_translate(polygon, vec_negate(point));
    size_t size = list_size(polygon);
    for(size_t i = 0; i < size; i++){
        vector_t rotated = vec_rotate(*(vector_t*) list_get(polygon, i), angle);
        *(vector_t*) list_get(polygon, i) = rotated;
    }
    polygon_translate(polygon, point);
}