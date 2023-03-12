#pragma once

#include "camera.h"
#include "hittable.h"
#include "vec3.h"
#include <vector>

struct Material {
    int id;
    color ambient, diffuse, specular, mirror_refl;
    double phong_exp{0};
};

struct Pointlight {
    point3 position;
    color intensity;
};

struct Scene {
    Camera camera;
    color background, ambient;
    std::vector<Pointlight> lights;
    std::vector<Material> materials;
    std::vector<Hittable *> hittables;
    std::vector<point3> vertices;

    Material get_material(int id) const {
        for (auto &mat : materials) {
            if (mat.id == id) {
                return mat;
            }
        }
        return Material();
    }
};