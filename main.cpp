#include <iostream>
#include <limits>
#include "ray.h"
#include "scene_def.h"
using namespace std;

#define EPS 0.0001

double max(const double a, const double b)
{
    return a > b ? a : b;
}

ray eye_ray_to_pixel(const camera_st &camera, const int i, const int j)
{
    point3 m = camera.position - camera.w * camera.near_distance;
    point3 q = m + camera.np_l * camera.u + camera.np_r * camera.v;
    double s_u = (i + 0.5) * (camera.np_r - camera.np_l) / camera.nx;
    double s_v = (j + 0.5) * (camera.np_t - camera.np_b) / camera.ny;
    vec3 s = q + s_u * camera.u - s_v * camera.v;

    return ray(camera.position, s - camera.position);
}

bool intersect(const ray &r, const point3 &v0, const point3 &v1, const point3 &v2, double &t)
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

    return true;
}

scene_st generate_scene()
{
    scene_st scene;

    scene.camera.u = vec3(1, 0, 0);
    scene.camera.v = vec3(0, 1, 0);
    scene.camera.w = cross(scene.camera.u, scene.camera.v);
    scene.camera.position = point3(0, 0, 0);
    scene.camera.nx = 100;
    scene.camera.ny = 100;
    scene.camera.np_l = -1;
    scene.camera.np_r = 1;
    scene.camera.np_b = -1;
    scene.camera.np_t = 1;
    scene.camera.near_distance = 1;

    scene.background = color(255, 255, 255);
    scene.ambient_light = color(100, 150, 20);

    scene.verts.push_back(vec3(-0.01, 0.33, -1.0));
    scene.verts.push_back(vec3(-0.55, -0.35, -1.0));
    scene.verts.push_back(vec3(0.61, -0.37, -1));

    scene.verts.push_back(vec3(-0.70, 0.20, -1.0));
    scene.verts.push_back(vec3(-0.80, -0.20, -0.5));
    scene.verts.push_back(vec3(-0.60, -0.20, -1.5));

    mesh_st mesh;
    mesh.faces.push_back(vec3(0, 1, 2));
    scene.objects.push_back(mesh);

    pointlight_st light;
    light.position = point3(1.0, -0.1, -0.5);
    light.intensity = color(50, 50, 40);

    pointlight_st l2;
    l2.position = point3(0.3, 0.44, -0.3);
    l2.intensity = color(60, 40, 50);

    scene.p_lights.push_back(light);
    scene.p_lights.push_back(l2);
    return scene;
}

int clamp(double a)
{
    int x = static_cast<int>(a);
    return x > 255 ? 255 : x;
}

void write_color(std::ostream &out, color pixel_color)
{
    // Write the translated [0,255] value of each color component.
    out << clamp(pixel_color.x) << ' '
        << clamp(pixel_color.y) << ' '
        << clamp(pixel_color.z) << '\n';
}

int main(void)
{
    scene_st scene = generate_scene();

    double t_min;
    double t;
    mesh_st *obj = NULL;
    point3 x;
    vec3 n;
    point3 face;
    double cos_t;

    std::cout << "P3\n"
              << scene.camera.nx << " " << scene.camera.ny << "\n255\n";
    for (int j = 0; j < scene.camera.ny; ++j)
    {
        for (int i = 0; i < scene.camera.nx; ++i)
        {
            // tmin = inf, obj || material || face = NULL
            t_min = numeric_limits<double>::infinity();
            obj = NULL;
            ray r = eye_ray_to_pixel(scene.camera, i, j);

            // for each obj o
            // check intersect at point x
            // if t < tmin: tmin = t; obj = o
            for (auto o : scene.objects)
            {
                for (auto f : o.faces)
                {
                    if (intersect(r, scene.verts[f.x], scene.verts[f.y], scene.verts[f.z], t) &&
                        t_min > t)
                    {
                        t_min = t;
                        face = f;
                    }
                }
                if (t_min < numeric_limits<double>::infinity())
                {
                    obj = &o; // change this with id
                    n = cross(scene.verts[face.y] - scene.verts[face.x], scene.verts[face.z] - scene.verts[face.x]);
                    n = n / n.len();
                }
            }

            color c = scene.background; // change for background

            // if obj != NULL
            //  c = ambient light
            //  for each light l
            //  compute shadow ray s from x to l
            //  for each obj p
            //  if s intersects p before l
            //   ignore this light
            //  else: c += diffuse + specular
            if (obj != NULL)
            {
                c = scene.ambient_light;
                // diffuse, specular and shadow calculations should follow
                // diffuse, need to use materials
                for (auto l : scene.p_lights)
                {
                    vec3 w_i = l.position - r.at(t);
                    bool shadow = false;
                    // shadow
                    //      compute shadow ray s from x to l with epsilon
                    //      check if it intersects any object
                    ray s = ray(r.at(t) + EPS * w_i, w_i);
                    for (auto o : scene.objects)
                    {
                        for (auto f : o.faces)
                        {
                            double t_x;
                            if (intersect(s, scene.verts[f.x], scene.verts[f.y], scene.verts[f.z], t_x))
                            {
                                if (t_x < 1)
                                {
                                    if (t_x > 0)
                                        cerr << t_x << endl;
                                    shadow = true;
                                    break;
                                }
                            }
                        }
                        if (shadow)
                            break;
                    }

                    if (!shadow)
                    {
                        // diffuse
                        color E_i = l.intensity / pow(w_i.len(), 2);
                        double cos_t = max(0, dot(n, w_i));
                        if (cos_t > 1)
                            cerr << cos_t << endl;

                        c += cos_t * E_i;

                        // specular
                        vec3 w_o = scene.camera.position - r.at(t);
                        vec3 h = w_i + w_o;
                        h = h / h.len();

                        double cos_a = max(0, dot(n, h));
                        c += pow(cos_a, 0.5) * E_i;
                    }
                }
            }
            write_color(cout, c);
        }
    }

    return 0;
}