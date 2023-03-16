#pragma once

#include "camera.h"
#include "hittable.h"
#include "vec3.h"
#include <vector>
#include <string>

struct Material {
    std::string id;
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

    Material get_material(std::string id) const;
    bool hit(const ray &r, const double t_min, const double t_max,
             HitRecord &rec) const;
};
bool scene_from_xml_file(Scene &scene, const char *path);
