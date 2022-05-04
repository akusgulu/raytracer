#ifndef SCENE_DEF_H
#define SCENE_DEF_H
#include "vec3.h"
#include "ray.h"

#include <vector>
#include <map>
#include <string>
using namespace std;

#define LEFT 0
#define RIGHT 1
#define TOP 2
#define BOTTOM 3

struct camera_st
{
    point3 position;
    vec3 u, v, w;
    double np[4];
    double near_distance;
    int nx, ny;
};

struct pointlight_st
{
    point3 position;
    color intensity;
};

struct material_st
{
    color ambient, diffuse, specular, mirror_refl;
    double phong_exp;
};

struct triangle
{
    point3 v0, v1, v2;
    bool intersect(const ray &r, const double &t_min, const double &t_max, double &t) const
    {
        const double EPSILON = 0.000001;

        vec3 e1 = v1 - v0;
        vec3 e2 = v2 - v0;

        vec3 pvec = cross(r.direction(), e2);
        double det = dot(e1, pvec);

        if (det > -EPSILON && det < EPSILON)
            return false;
        double inv_det = 1.0 / det;

        vec3 tvec = r.origin() - v0;
        double u = dot(tvec, pvec) * inv_det;
        if (u < 0.0 || u > 1.0)
            return false;

        vec3 qvec = cross(tvec, e1);

        double v = dot(r.direction(), qvec) * inv_det;
        if (v < 0.0 || u + v > 1.0)
            return false;

        t = dot(e2, qvec) * inv_det;

        return t > t_min && t < t_max;
    }
    vec3 normal() const{
        return cross(v1 - v0, v2 - v0);
    }
};

struct mesh_st
{
    string material_id;
    vector<triangle> faces;
};

struct scene_st
{
    unsigned max_depth = 0;
    color background{0, 0, 0};
    camera_st camera;
    color ambient_light{0, 0, 0};
    map<string, material_st> materials;
    vector<pointlight_st> p_lights;
    vector<vec3> verts;
    vector<mesh_st> objects;
};

bool scene_from_file(scene_st &scene, const char *path);

#endif
