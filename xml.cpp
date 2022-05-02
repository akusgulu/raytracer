#include <iostream>

#include "pugixml-1.12/src/pugixml.hpp"
#include "scene_def.h"
#include <sstream>

using namespace pugi;
using namespace std;

vector<double> tokenize(const char *str)
{
    vector<double> tokens;
    stringstream ss{str};
    double n;
    while (ss >> n)
    {
        tokens.push_back(n);
    }
    return tokens;
}

vec3 v_to_v3(const vector<double> &v)
{
    return vec3(v[0], v[1], v[2]);
}

vector<vec3> str_to_vv3(const char *str)
{
    vector<double> v = tokenize(str);
    vector<vec3> vv3;
    for (int i = 0; i < v.size(); i += 3)
        vv3.push_back(point3(v[i], v[i + 1], v[i + 2]));
    return vv3;
}

void field_error(const char *field_name)
{
    cerr << "Error loading field: " << field_name << endl;
}

bool scene_from_file(scene_st &scene, const char *path)
{

    xml_document doc;
    xml_parse_result res = doc.load_file(path, parse_trim_pcdata);

    // if(res != status_ok){
    //     cerr << res << endl;
    //     cerr << path << endl;
    //     // cerr << res.description() << " " << res.offset << endl;
    //     return false;
    // }

    xml_node sc = doc.child("scene");
    xml_node camera = sc.child("camera");
    xml_node lights = sc.child("lights");
    xml_node materials = sc.child("materials");
    xml_node vdata = sc.child("vertexdata");
    xml_node objs = sc.child("objects");

    vector<double> tok;
    string buf;

    scene.max_depth = stoi(sc.child_value("maxraytracedepth"));
    scene.background = v_to_v3(tokenize(sc.child_value("background")));

    scene.camera.position = v_to_v3(tokenize(camera.child_value("position")));
    scene.camera.w = -v_to_v3(tokenize(camera.child_value("gaze")));
    scene.camera.v = v_to_v3(tokenize(camera.child_value("up")));
    scene.camera.u = cross(scene.camera.v, scene.camera.w);

    tok = tokenize(camera.child_value("nearplane"));
    scene.camera.np[LEFT] = tok[0];
    scene.camera.np[RIGHT] = tok[1];
    scene.camera.np[BOTTOM] = tok[2];
    scene.camera.np[TOP] = tok[3];

    scene.camera.near_distance = stod(camera.child_value("neardistance"));
    tok = tokenize(camera.child_value("imageresolution"));
    scene.camera.nx = tok[0];
    scene.camera.ny = tok[1];

    scene.ambient_light = v_to_v3(tokenize(lights.child_value("ambientlight")));

    pointlight_st p;
    for (auto light : lights.children("pointlight"))
    {
        // cout << light.attribute("id").value() << endl;
        p.position = v_to_v3(tokenize(light.child_value("position")));
        p.intensity = v_to_v3(tokenize(light.child_value("intensity")));
        scene.p_lights.push_back(p);
    }

    material_st m;
    for (auto mat : materials.children("material"))
    {
        m.ambient = v_to_v3(tokenize(mat.child_value("ambient")));
        m.diffuse = v_to_v3(tokenize(mat.child_value("diffuse")));
        m.specular = v_to_v3(tokenize(mat.child_value("specular")));
        m.mirror_refl = v_to_v3(tokenize(mat.child_value("mirrorreflectance")));
        m.phong_exp = stoi(mat.child_value("phongexponent"));
        scene.materials[mat.attribute("id").value()] = m;
    }
    auto verts = str_to_vv3(sc.child_value("vertexdata"));

    mesh_st mesh;
    for (auto o : objs.children("mesh"))
    {
        mesh.material_id = o.child_value("materialid");
        auto faces = str_to_vv3(o.child_value("faces"));
        for (auto f : faces)
        {
            mesh.faces.push_back({verts[f.x - 1], verts[f.y - 1], verts[f.z - 1]});
        }
        scene.objects.push_back(mesh);
    }

    return true;
}