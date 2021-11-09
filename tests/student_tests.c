#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "../include/forces.h"
#include "../include/test_util.h"

list_t *make_shape() {
    list_t *shape = list_init(4, free);
    vector_t *v = malloc(sizeof(vector_t));
    *v = (vector_t){-1, -1};
    list_add(shape, v);
    v = malloc(sizeof(vector_t));
    *v = (vector_t){+1, -1};
    list_add(shape, v);
    v = malloc(sizeof(vector_t));
    *v = (vector_t){+1, +1};
    list_add(shape, v);
    v = malloc(sizeof(vector_t));
    *v = (vector_t){-1, +1};
    list_add(shape, v);
    return shape;
}

// Tests that a mass on a spring oscillates like A cos(sqrt(K / M) * t)
bool helper_spring_sinusoid(double m, double k, double a, double dt) {
    const int STEPS = 1000000;
    scene_t *scene = scene_init();
    body_t *mass = body_init(make_shape(), m, (rgb_color_t){0, 0, 0});
    body_set_centroid(mass, (vector_t){a, 0});
    scene_add_body(scene, mass);
    body_t *anchor = body_init(make_shape(), INFINITY, (rgb_color_t){0, 0, 0});
    scene_add_body(scene, anchor);
    create_spring(scene, k, mass, anchor);
    for (int i = 0; i < STEPS; i++) {
        bool test_pos = vec_isclose(body_get_centroid(mass), 
                        (vector_t){a * cos(sqrt(k / m) * i * dt), 0});
        bool test_anchor = vec_equal(body_get_centroid(anchor), VEC_ZERO);
        
        if (!test_pos || !test_anchor) {
                scene_free(scene);
                return false;
        }
        scene_tick(scene, dt);
    }
    scene_free(scene);
    return true;
}

void test_spring_sinusoid() {
    assert(helper_spring_sinusoid(5, 2, 3, 1e-6));
    assert(helper_spring_sinusoid(15, 1, 10, 1e-6));
}

double gravity_potential(double G, body_t *body1, body_t *body2) {
    vector_t r = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
    return -G * body_get_mass(body1) * body_get_mass(body2) / sqrt(vec_dot(r, r));
}

double kinetic_energy(body_t *body) {
    vector_t v = body_get_velocity(body);
    return body_get_mass(body) * vec_dot(v, v) / 2;
}

// Tests that drag force does not increase energy
bool helper_drag_decreases_energy(double m1, double dt) {
    const int STEPS = 1000000;

    scene_t *scene = scene_init();
    body_t *mass1 = body_init(make_shape(), m1, (rgb_color_t){0, 0, 0});
    scene_add_body(scene, mass1);

    double gamma = 50.0;
    create_drag(scene, gamma, mass1);

    double initial_energy = kinetic_energy(mass1);

    for (int i = 0; i < STEPS; i++) {
        double final_energy = kinetic_energy(mass1);
        if (initial_energy < final_energy) {
            scene_free(scene);
            return false;
        }
        initial_energy = final_energy;
        scene_tick(scene, dt);
    }
    scene_free(scene);
    return true;
}

void test_drag_decreases_energy() {
    assert(helper_drag_decreases_energy(4.5, 1e-6));
    assert(helper_drag_decreases_energy(1, 1e-6));
    assert(helper_drag_decreases_energy(0.5, 1e-6));
    assert(helper_drag_decreases_energy(1, 1e-6));
}

// Tests that a conservative force (gravity) conserves K + U

bool helper_newtonian_gravity_conservation(double m1, double m2, double g, double dt) {
    const int STEPS = 1000000;
    scene_t *scene = scene_init();
    body_t *mass1 = body_init(make_shape(), m1, (rgb_color_t){0, 0, 0});
    scene_add_body(scene, mass1);
    body_t *mass2 = body_init(make_shape(), m2, (rgb_color_t){0, 0, 0});
    body_set_centroid(mass2, (vector_t){10, 20});
    scene_add_body(scene, mass2);
    create_newtonian_gravity(scene, g, mass1, mass2);
    double initial_energy = gravity_potential(g, mass1, mass2);
    for (int i = 0; i < STEPS; i++) {
        if(body_get_centroid(mass1).x >= body_get_centroid(mass2).x) {
            scene_free(scene);
            return false;
        }
        double energy = gravity_potential(g, mass1, mass2) + kinetic_energy(mass1) +
                        kinetic_energy(mass2);
        if (!within(1e-4, energy / initial_energy, 1)) {
            scene_free(scene);
            return false;
        }
        scene_tick(scene, dt);
    }
    scene_free(scene);
    return true;
}

void test_newtonian_gravity_conservation() {
    assert(helper_newtonian_gravity_conservation(3.0, 2.0, 1.0, 1e-6));
    assert(helper_newtonian_gravity_conservation(3.0, 2.0, 9.0, 1e-6));
    assert(helper_newtonian_gravity_conservation(10.0, 2.0, 5.0, 1e-6));
}

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_spring_sinusoid)
    DO_TEST(test_newtonian_gravity_conservation)
    DO_TEST(test_drag_decreases_energy)
    puts("student_tests PASS");
}