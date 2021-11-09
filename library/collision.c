#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "list.h"
#include "collision.h"
#include "vector.h"
#include "polygon.h"
#include "assert.h"

double min(double a, double b) {
    if (a < b) return a;
    return b;
}

list_t *orthogonal_edges_of(list_t *shape) {
    size_t shape_size = list_size(shape);
    list_t *edges = list_init(shape_size - 1, (free_func_t) free);
    for(size_t i = 0; i < shape_size; i++) {
        vector_t *p1 = list_get(shape, (i + 1) % shape_size);
        vector_t *p2 = list_get(shape, i);
        vector_t *edge = malloc(sizeof(vector_t));
        // Get orthogonal transformation of edge
        *edge = vec_orthogonal(vec_subtract(*p1, *p2));
        list_add(edges, edge);
    }
    return edges;
}

collision_info_t is_separating(vector_t *axis, list_t *shape1, list_t *shape2) {
    double min_s1 = 1.0e10;
    double max_s1 = -1.0e10;
    double min_s2 = 1.0e10;
    double max_s2 = -1.0e10;
    // Update the bounds of the projection for each shape
    for (size_t i = 0; i < list_size(shape1); i++) {
        vector_t v = *(vector_t *) list_get(shape1, i);
        // Projection of v onto the separating axis
        double p = vec_dot(v, *axis);
        if(p < min_s1) {
            min_s1 = p;
        }
        if(p > max_s1) {
            max_s1 = p;
        }
    }
    for (size_t i = 0; i < list_size(shape2); i++) {
        vector_t v = *(vector_t *) list_get(shape2, i);
        // Projection of v onto the separating axis
        double p = vec_dot(v, *axis);
        if (p < min_s2) {
            min_s2 = p;
        }
        if (p > max_s2) {
            max_s2 = p;
        }
    }
    collision_info_t res;
    if (max_s1 >= min_s2 && max_s2 >= min_s1) {
        double overlap = min(max_s2 - min_s1, max_s1 - min_s2);
        vector_t v = vec_multiply(overlap, vec_normalize(*axis)); // axis / norm(axis) * overlap
        res.collided = false;
        res.axis = v;
    }
    else {
        res.collided = true;
    }
    return res;
}

collision_info_t find_collision(list_t *shape1, list_t *shape2) {
    collision_info_t res;
    list_t *push_vectors = list_init(5, (free_func_t) free);
    list_t *all_orthogonal_edges = list_init(5, (free_func_t) free);
    list_t *shape1_edges = orthogonal_edges_of(shape1);
    list_t *shape2_edges = orthogonal_edges_of(shape2);
    list_append(all_orthogonal_edges, shape1_edges);
    list_append(all_orthogonal_edges, shape2_edges);
    for (size_t i = 0; i < list_size(all_orthogonal_edges); i++) {
        collision_info_t axis_result = is_separating((vector_t *) list_get(all_orthogonal_edges, i), shape1, shape2);
        if (axis_result.collided) {
            res.collided = false;
            return res;
        }
        else {
            vector_t *pv = malloc(sizeof(vector_t));
            *pv = axis_result.axis;
            list_add(push_vectors, pv);
        }
    }

    // Get minimum push vector by overlap
    double min_norm = 1.0e10;
    vector_t min_vector;
    for (size_t i = 0; i < list_size(push_vectors); i++) {
        vector_t *cur_vec = list_get(push_vectors, i);
        double cur_norm = vec_norm(*cur_vec);
        if (cur_norm < min_norm) {
            min_norm = cur_norm;
            min_vector = *cur_vec;
        }
    }
    
    vector_t normalized_axis = (vec_multiply(1/vec_norm(min_vector), min_vector));
    res.collided = true;
    res.axis = normalized_axis;
    return res;
}