#include <iostream>
#include <fstream>
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

bool is_valid(const char_t *p, const string &error_msg,bool& err)
{
    if (p == "")
    {
        cerr << "XML error: " << error_msg << " not found" << endl;
        err &= false;
        return err |= false;
    }
    err &= true;
    return true;
}

inline bool is_valid(const char_t *p, const string &id, const string &error_msg,bool& err)
{
    return is_valid(p, id + error_msg,err);
}

bool scene_from_file(scene_st &scene, const char *path)
{
    bool err = true;
    
    xml_document doc;
    xml_parse_result res = doc.load_file(path, parse_trim_pcdata);

    if(!doc.first_child()){
        cerr << "Error: Can't open input file!" << endl;
        return false;
    }
    xml_node sc = doc.child("scene");
    xml_node camera = sc.child("camera");
    xml_node lights = sc.child("lights");
    xml_node materials = sc.child("materials");
    xml_node vdata = sc.child("vertexdata");
    xml_node objs = sc.child("objects");

    vector<double> tok;

    if (is_valid(sc.child_value("maxraytracedepth"), "-maxraytracedepth-",err))
        scene.max_depth = stoi(sc.child_value("maxraytracedepth"));

    if (is_valid(sc.child_value("background"), "-background-",err))
        scene.background = v_to_v3(tokenize(sc.child_value("background")));

    if (is_valid(camera.child_value("position"), "-camera.position-",err))
        scene.camera.position = v_to_v3(tokenize(camera.child_value("position")));

    if (is_valid(camera.child_value("gaze"), "-camera.gaze-",err))
        scene.camera.w = -v_to_v3(tokenize(camera.child_value("gaze")));

    if (is_valid(camera.child_value("position"), "-camera.position-",err))
        scene.camera.v = v_to_v3(tokenize(camera.child_value("up")));

    scene.camera.u = cross(scene.camera.v, scene.camera.w);

    if (is_valid(camera.child_value("nearplane"), "-camera.nearplane-",err))
        tok = tokenize(camera.child_value("nearplane"));
    scene.camera.np[LEFT] = tok[0];
    scene.camera.np[RIGHT] = tok[1];
    scene.camera.np[BOTTOM] = tok[2];
    scene.camera.np[TOP] = tok[3];

    if (is_valid(camera.child_value("neardistance"), "-camera.neardistance-",err))
        scene.camera.near_distance = stod(camera.child_value("neardistance"));

    if (is_valid(camera.child_value("imageresolution"), "camera.imageresolution-",err))
        tok = tokenize(camera.child_value("imageresolution"));
    scene.camera.nx = tok[0];
    scene.camera.ny = tok[1];

    if (is_valid(lights.child_value("ambientlight"), "-ambientlight-",err))
        scene.ambient_light = v_to_v3(tokenize(lights.child_value("ambientlight")));

    pointlight_st p;
    for (auto light : lights.children("pointlight"))
    {
        string id = light.attribute("id").value();
        if (is_valid(light.child_value("position"), id, ".position",err))
            p.position = v_to_v3(tokenize(light.child_value("position")));

        if (is_valid(light.child_value("intensity"), id, ".intensity",err))
            p.intensity = v_to_v3(tokenize(light.child_value("intensity")));

        scene.p_lights.push_back(p);
    }

    material_st m;
    for (auto mat : materials.children("material"))
    {
        string id = mat.attribute("id").value();
        if (is_valid(mat.child_value("ambient"), id, ".ambient",err))
            m.ambient = v_to_v3(tokenize(mat.child_value("ambient")));

        if (is_valid(mat.child_value("diffuse"), id, ".diffuse",err))
            m.diffuse = v_to_v3(tokenize(mat.child_value("diffuse")));

        if (is_valid(mat.child_value("specular"), id, ".specular",err))
            m.specular = v_to_v3(tokenize(mat.child_value("specular")));

        if (is_valid(mat.child_value("mirrorreflectance"), id, ".mirrorreflectance",err))
            m.mirror_refl = v_to_v3(tokenize(mat.child_value("mirrorreflectance")));

        if (is_valid(mat.child_value("phongexponent"), id, ".phongexponent",err))
            m.phong_exp = stoi(mat.child_value("phongexponent"));

        scene.materials[id] = m;
    }
    vector<vec3> verts;
    if (is_valid(sc.child_value("vertexdata"),".vertexdata",err))
        verts = str_to_vv3(sc.child_value("vertexdata"));

    mesh_st mesh;
    for (auto o : objs.children("mesh"))
    {
        mesh.faces.clear();
        string id = o.attribute("id").value();
        if (is_valid(o.child_value("materialid"), id, ".materialid",err))
            mesh.material_id = o.child_value("materialid");
        if (is_valid(o.child_value("faces"), id, ".faces",err))
        {
            auto faces = str_to_vv3(o.child_value("faces"));
            for (auto f : faces)
            {
                mesh.faces.push_back({verts[f.x - 1], verts[f.y - 1], verts[f.z - 1]});
            }
        }
        scene.objects.push_back(mesh);
    }

    return err;
}