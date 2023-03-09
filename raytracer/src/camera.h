#pragma once

#include "vec3.h"
#include "ray.h"

struct Camera {
    point3 position;
    vec3 u, v, w;
    double np_l, np_r, np_t, np_b;
    double near_dist;
    int nx, ny;

    ray ray_to_pixel(const int i, const int j) {
        point3 m = position - w * near_dist;
        point3 q = m + np_l * u + np_r * v;
        double s_u = (i + 0.5) * (np_r - np_l) / nx;
        double s_v = (j + 0.5) * (np_t - np_b) / ny;
        vec3 s = q + s_u * u - s_v * v;

        return ray(position, s - position);
    }
};