#ifndef SCENE_DEF_H
#define SCENE_DEF_H
#include "vec3.h"

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
    vec3 u,v,w;
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

struct mesh_st
{
    string material_id;
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

// void print_scene(const scene_st& scene){
//     cout << scene.max_depth << endl;
//     cout << scene.background << endl;

//     cout << scene.camera.position << endl;
//     cout << scene.camera.u << " " << scene.camera.v << " " << scene.camera.w << endl;
//     for(int i=LEFT;i<=BOTTOM;++i){
//         cout << scene.camera.np[i] << " ";
//     }
//     cout << endl;
//     cout << scene.camera.near_distance << endl;
//     cout << scene.camera.nx << " " << scene.camera.ny << endl;

//     cout << scene.ambient_light << endl;
//     for(auto m:scene.materials){
//         cout << m.first << endl;
//         cout << m.second.ambient << " " << m.second.diffuse << " " << m.second.specular;
//         cout << m.second.mirror_refl << " " << m.second.phong_exp << endl;
//     }

//     for(auto v:scene.verts){
//         cout << v << endl;
//     }

//     for(auto o:scene.objects){
//         cout << o.material_id << endl;
//         for(auto f:o.faces){
//             cout << f << endl;
//         }
//     }

// }

scene_st scene_from_file(const char* path);

#endif
