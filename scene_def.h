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
    double phong_exp{0};
};

struct triangle
{
public:
    point3 v0, v1, v2;

    vec3 normal() const
    {
        return cross(v1 - v0, v2 - v0);
    }

    bool intersect(const ray &r, const double &t_min, const double &t_max, double &t) const
    {
        const double EPSILON = 0.000001;

        vec3 a_b = v0 - v1;
        vec3 a_c = v0 - v2;
        double detA = determinant(a_b, a_c, r.direction());
        if (detA > -EPSILON && detA < EPSILON)
            return false;

        vec3 a_o = v0 - r.origin();
        double inv_detA = 1.0 / detA;

        double beta = determinant(a_o, a_c, r.direction()) * inv_detA;
        if (beta < 0.0 || beta > 1.0)
            return false;

        double gamma = determinant(a_b, a_o, r.direction()) * inv_detA;
        if (gamma < 0.0 || beta + gamma > 1.0)
            return false;

        t = determinant(a_b, a_c, a_o) * inv_detA;

        return t > t_min && t < t_max;
    }

private:
    inline double determinant(const vec3 &col1, const vec3 &col2, const vec3 &col3) const
    {
        return col1.x * (col2.y * col3.z - col3.y * col2.z) + col1.y * (col3.x * col2.z - col3.z * col2.x) + col1.z * (col2.x * col3.y - col2.y * col3.x);
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
