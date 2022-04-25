#ifndef SCENE_DEF_H
#define SCENE_DEF_H
#include "vec3.h"

#include <vector>
#include <map>
#include <string>
using namespace std;

struct camera_st
{
    point3 position;
    vec3 u,v,w;
    double np_l, np_r, np_t, np_b;
    double near_distance;
    int nx, ny;
};

struct pointlight_st
{
    char id[4]; // why do we have this
    point3 position;
    color intensity;
};

struct material_st
{
    color ambient, diffuse, specular, mirror_refl;
    double phong_exp;
};

struct mesh_st
{
    char id[4]; // why do we have this
    int material_id;
    vector<vec3> faces;
};

struct scene_st
{
    unsigned max_depth = 0;
    color background{0, 0, 0};
    camera_st camera;
    color ambient_light{0, 0, 0};
    map<string,material_st> materials;
    vector<pointlight_st> p_lights;
    vector<vec3> verts;
    vector<mesh_st> objects;
};
#endif
