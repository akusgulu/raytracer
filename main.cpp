#include <iostream>
#include <limits>
#include "ray.h"
#include "scene_def.h"
using namespace std;

#define EPS 0.000001
#define INF numeric_limits<double>::infinity()

double max(const double a, const double b)
{
    return a > b ? a : b;
}

int clamp(double a)
{
    int x = static_cast<int>(a);
    return max(0, x > 255 ? 255 : x);
}

void write_color(std::ostream &out, color pixel_color)
{
    out << clamp(pixel_color.x) << ' '
        << clamp(pixel_color.y) << ' '
        << clamp(pixel_color.z) << '\n';
}

bool intersect(const ray &r, const point3 &v0, const point3 &v1, const point3 &v2, double &t,const double t_min, const double t_max)
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

    return t>t_min && t<t_max;
}

ray eye_ray_to_pixel(const camera_st &camera, const int i, const int j)
{
    point3 m = camera.position - camera.w * camera.near_distance;
    point3 q = m + camera.np[LEFT] * camera.u + camera.np[RIGHT] * camera.v;
    double s_u = (i + 0.5) * (camera.np[RIGHT] - camera.np[LEFT]) / camera.nx;
    double s_v = (j + 0.5) * (camera.np[TOP] - camera.np[BOTTOM]) / camera.ny;
    vec3 s = q + s_u * camera.u - s_v * camera.v;

    return ray(camera.position, s - camera.position);
}

color ray_color(const scene_st &scene, const ray &r)
{
    double t_min = numeric_limits<double>::infinity();
    double t;
    string mat_id = "";
    material_st mat;
    point3 face;

    for (auto o : scene.objects)
    {
        for (auto f : o.faces)
        {
            if (intersect(r, scene.verts[f.x - 1], scene.verts[f.y - 1], scene.verts[f.z - 1], t,0,INF) &&
                t_min > t)
            {
                t_min = t;
                face = f;
                mat_id = o.material_id;
            }
        }
    }

    vec3 n;
    if (t_min < numeric_limits<double>::infinity())
        n = unit_vec(cross(scene.verts[face.y - 1] - scene.verts[face.x - 1], scene.verts[face.z - 1] - scene.verts[face.x - 1]));
    

    color c = scene.background;

    if (mat_id != "")
    {
        mat = scene.materials.at(mat_id);
        c = mat.ambient * scene.ambient_light;
        for (auto l : scene.p_lights)
        {
            point3 x = r.at(t_min);
            vec3 l_to_x = l.position - x;
            vec3 w_i = unit_vec(l_to_x);
            double dist = l_to_x.len();
            bool shadow = false;
            ray s = ray(x + EPS * w_i, w_i);
            for (auto o : scene.objects)
            {
                for (auto f : o.faces)
                {
                    double t_x;
                    if (intersect(s, scene.verts[f.x - 1], scene.verts[f.y - 1], scene.verts[f.z - 1], t_x,0,INF) && t_x > 0)
                    {
                        if (s.at(t_x).len() < dist) // probably wrong
                        {
                            cerr << fixed << s.at(t_x).len() << " | " << dist << " > " << t_x << endl;
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
                color E_i = l.intensity / pow(dist, 2);
                double cos_t = max(0, dot(n, w_i));

                c += mat.diffuse * cos_t * E_i;

                vec3 w_o = unit_vec(scene.camera.position - r.at(t));
                vec3 h = unit_vec(w_i + w_o);

                double cos_a = max(0, dot(n, h));
                c += mat.specular * pow(cos_a, mat.phong_exp) * E_i;
            }
        }
    }
    return c;
}

int main(void)
{
    scene_st scene = scene_from_file("scenes/scene3.xml");

    std::cout << "P3\n"
              << scene.camera.nx << " " << scene.camera.ny << "\n255\n";
    for (int j = 0; j < scene.camera.ny; ++j)
    {
        for (int i = 0; i < scene.camera.nx; ++i)
        {
            ray r = eye_ray_to_pixel(scene.camera, i, j);
            write_color(cout, ray_color(scene, r));
        }
    }

    return 0;
}