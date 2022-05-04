#include <iostream>
#include <limits>
#include "ray.h"
#include "scene_def.h"
#include <thread>

using namespace std;

#define EPS 0.0001
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

ray eye_ray_to_pixel(const camera_st &camera, const int i, const int j)
{
    // hold these calculations in camera or scene or sth much less calculation
    point3 m = camera.position - camera.w * camera.near_distance;
    point3 q = m + camera.np[LEFT] * camera.u + camera.np[RIGHT] * camera.v;
    double s_u = (i + 0.5) * (camera.np[RIGHT] - camera.np[LEFT]) / camera.nx;
    double s_v = (j + 0.5) * (camera.np[TOP] - camera.np[BOTTOM]) / camera.ny;
    vec3 s = q + s_u * camera.u - s_v * camera.v;

    return ray(camera.position, s - camera.position);
}

color ray_color(const scene_st &scene, const ray &r, const double depth)
{
    double t_min = numeric_limits<double>::infinity();
    double t;
    string mat_id = "";
    vec3 n;
    // scene.hit, calculate
    for (auto &o : scene.objects)
    {
        for (auto &f : o.faces)
        {
            if (f.intersect(r, 0, INF, t) && t_min > t)
            {
                t_min = t;
                n = f.normal();
                mat_id = o.material_id;
            }
        }
    }

    color c = scene.background;

    if (mat_id != "")
    {
        point3 x = r.at(t_min);
        const material_st &mat = scene.materials.at(mat_id);
        c = mat.ambient * scene.ambient_light;
        vec3 w_o = scene.camera.position - r.at(t_min);

        for (auto &l : scene.p_lights)
        {
            vec3 l_to_x = l.position - x;
            vec3 w_i = l_to_x;
            double dist_l = l_to_x.len();
            bool shadow = false;
            ray s = ray(x + EPS * w_i, w_i);
            for (auto &o : scene.objects)
            {
                for (auto &f : o.faces)
                {
                    double t_x;
                    if (f.intersect(s, 0, INF, t_x))
                    {
                        double dist_obj = (s.at(t_x) - s.origin()).len();
                        if (dist_obj < dist_l)
                        {
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
                color E_i = l.intensity / pow(dist_l, 2);
                double cos_t = max(0, dot(n, w_i));

                c += mat.diffuse * cos_t * E_i;

                vec3 w_o = unit_vec(scene.camera.position - r.at(t));
                vec3 h = unit_vec(w_i + w_o);

                double cos_a = max(0, dot(n, h));

                c += mat.specular * pow(cos_a, mat.phong_exp) * E_i;
            }
        }
        if (mat.mirror_refl.len() > 0 && depth > 0)
        {
            vec3 w_r = -w_o + 2 * n * dot(n, w_o);
            c += mat.mirror_refl * ray_color(scene, ray(x + w_r * EPS, w_r), depth - 1);
        }
    }
    return c;
}

struct thread_info
{
    int start_pixel = 0, n_pixel = 0;
    vector<color> pixels;
};

void thread_job(thread_info &info, const scene_st &scene)
{
    int nx = scene.camera.nx;
    cerr << &scene << endl;
    for (int i = info.start_pixel, j = info.start_pixel / nx; i < info.start_pixel + info.n_pixel; ++i)
    {
        if (i % nx == 0)
            ++j;
        ray r = eye_ray_to_pixel(scene.camera, i % nx, j);
        info.pixels.push_back(ray_color(scene, r, scene.max_depth));
    }

    fprintf(stderr, "Pixels [%d,%d] are calculated\n", info.start_pixel, info.start_pixel + info.n_pixel);
}

void raytracing_threaded(scene_st &scene)
{
    const int nThreads = 8;

    thread_info info[nThreads];
    int total_pixel = scene.camera.nx * scene.camera.ny;
    int n_pixel = total_pixel / nThreads;
    int leftover = total_pixel % nThreads;

    cerr << "N: " << n_pixel << " l: " << leftover << endl;

    for (int i = 0; i < nThreads; ++i)
    {
        info[i].start_pixel = i * n_pixel;
        info[i].n_pixel = n_pixel;
    }
    info[nThreads - 1].n_pixel += leftover;

    thread th[nThreads];
    for (int i = 0; i < nThreads; ++i)
    {
        th[i] = thread(thread_job, ref(info[i]), cref(scene));
    }
    void *status;
    for (int i = 0; i < nThreads; ++i)
    {
        th[i].join();
    }

    std::cout << "P3\n"
              << scene.camera.nx << " " << scene.camera.ny << "\n255\n";
    for (int i = 0; i < nThreads; ++i)
    {
        for (color &c : info[i].pixels)
        {
            write_color(cout, c);
        }
    }
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        cerr << "No scene specified" << endl;
    }
    scene_st scene;
    if (scene_from_file(scene, argv[1]))
    {
        cerr << "PARSING ERROR, TERMINATING." << endl;
        return -1;
    }

    raytracing_threaded(scene);
    return 0;
}
