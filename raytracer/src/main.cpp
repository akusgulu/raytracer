#include "ray.h"
#include "scene_def.h"
#include "scene.h"
#include "mesh.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <thread>

using namespace std;

#define EPS 0.0000001
#define INF numeric_limits<double>::infinity()

double max(const double a, const double b) {
    return a > b ? a : b;
}

int clamp(double a) {
    int x = static_cast<int>(a);
    return max(0, x > 255 ? 255 : x);
}

void write_color(std::ostream &out, color pixel_color) {
    out << clamp(pixel_color.x) << ' ' << clamp(pixel_color.y) << ' '
        << clamp(pixel_color.z) << '\n';
}

Scene dummy_scene() {
    Scene s;
    s.camera.position = vec3{0, 0, 0};
    s.camera.w = -vec3{0, 0, -1};               // gaze
    s.camera.v = vec3{0, 1, 0};                 // up
    s.camera.u = cross(s.camera.v, s.camera.w); // cross
    s.camera.near_dist = 1;
    s.camera.np_l = -1;
    s.camera.np_r = 1;
    s.camera.np_b = -1;
    s.camera.np_t = 1;
    s.camera.nx = 1280;
    s.camera.ny = 720;

    s.background = color(0, 0, 10);
    s.ambient = color(25.0, 25.0, 25.0);

    Pointlight p;
    p.position = {-0.28, 0.21, -0.22};
    p.intensity = {1000, 1000, 1000};
    s.lights.push_back(p);

    Material m1, m2;

    m1.id = 1;
    m1.ambient = {1, 1, 1};
    m1.diffuse = {1, 1, 1};
    m1.specular = {0.5, 0.5, 0.5};
    m1.mirror_refl = {0, 0, 0};
    m1.phong_exp = 1;
    s.materials.push_back(m1);

    m2.id = 2;
    m2.ambient = {1, 1, 1};
    m2.diffuse = {0, 0, 1};
    m2.specular = {0, 0, 1};
    m2.phong_exp = 1;
    m2.mirror_refl = {0.5, 0.5, 0.5};
    s.materials.push_back(m2);

    for (auto &mat : s.materials) {
        cout << mat.id << endl;
    }

    std::vector<point3> verts{
        {0.138118, 0.100000, -0.278352},   {0.020125, 0.100000, -0.439838},
        {0.138118, -0.100000, -0.278352},  {0.020125, -0.100000, -0.439838},
        {-0.023368, 0.100000, -0.160359},  {-0.141361, 0.100000, -0.321845},
        {-0.023368, -0.100000, -0.160359}, {-0.141361, -0.100000, -0.321845},
        {-1.000000, -0.103624, -1.455763}, {1.000000, -0.123624, -1.455763},
        {-1.000000, -0.123624, 0.544237},  {1.000000, -0.123624, 0.544237}};
    std::vector<int> cube_inds{5, 3, 1, 3, 8, 4, 7, 6, 8, 2, 8, 6,
                               1, 4, 2, 5, 2, 6, 5, 7, 3, 3, 7, 8,
                               7, 5, 6, 2, 4, 8, 1, 3, 4, 5, 1, 2},
        plane_inds{11, 10, 9, 12, 10, 11};

    s.hittables.push_back(new Mesh(verts, cube_inds, 1));
    s.hittables.push_back(new Mesh(verts, plane_inds, 2));

    return s;
}

ray eye_ray_to_pixel(const camera_st &camera, const int i, const int j) {
    // hold these calculations in camera or scene or sth much less calculation
    point3 m = camera.position - camera.w * camera.near_distance;
    point3 q = m + camera.np[LEFT] * camera.u + camera.np[RIGHT] * camera.v;
    double s_u = (i + 0.5) * (camera.np[RIGHT] - camera.np[LEFT]) / camera.nx;
    double s_v = (j + 0.5) * (camera.np[TOP] - camera.np[BOTTOM]) / camera.ny;
    vec3 s = q + s_u * camera.u - s_v * camera.v;

    return ray(camera.position, s - camera.position);
}

color new_ray_color(const Scene &scene, const ray &r, const int depth) {
    HitRecord rec;
    HitRecord closest_hit;
    closest_hit.t = INF;
    for (auto &o : scene.hittables) {
        if (o->hit(r, 0, INF, rec) && closest_hit.t > rec.t) {
            closest_hit = rec;
        }
    }

    color c = scene.background;
    if (closest_hit.t != INF) {
        vec3 n = unit_vec(closest_hit.normal);
        point3 x = r.at(closest_hit.t);
        Material mat = scene.get_material(closest_hit.mat_id);

        c = mat.ambient * scene.ambient;
        vec3 w_o = unit_vec(scene.camera.position - x);

        for (auto &l : scene.lights) {
            vec3 l_to_x = l.position - x;
            vec3 w_i = unit_vec(l_to_x);
            double dist_l = l_to_x.len();
            ray s = ray(x + EPS * w_i, w_i);

            HitRecord shadow_rec;
            bool shadow = false;
            for (auto &o : scene.hittables) {
                if (o->hit(s, 0, INF, shadow_rec)) {
                    double dist_obj = (s.at(shadow_rec.t) - s.origin()).len();
                    if (dist_obj < dist_l) {
                        shadow = true;
                        break;
                    }
                }

                if (shadow)
                    break;
            }

            if (!shadow) {
                color E_i = l.intensity / (dist_l * dist_l);
                double cos_t = max(0, dot(n, w_i));

                c += mat.diffuse * cos_t * E_i;

                vec3 w_o =
                    unit_vec(scene.camera.position - r.at(closest_hit.t));
                vec3 h = unit_vec(w_i + w_o);

                double cos_a = max(0, dot(n, h));

                c += mat.specular * pow(cos_a, mat.phong_exp) * E_i;
            }
        }
        if (mat.mirror_refl.len() > 0 && depth > 0) {
            vec3 w_r = -w_o + 2 * n * dot(n, w_o);
            c += mat.mirror_refl *
                 new_ray_color(scene, ray(x + w_r * EPS, w_r), depth - 1);
        }
    }

    return c;
}

color ray_color(const scene_st &scene, const ray &r, const double depth) {
    double t_min = numeric_limits<double>::infinity();
    double t{0};
    string mat_id = "";
    vec3 n;
    // scene.hit, calculate
    for (auto &o : scene.objects) {
        for (auto &f : o.faces) {
            if (f.intersect(r, 0, INF, t) && t_min > t) {
                t_min = t;
                n = f.normal();
                mat_id = o.material_id;
            }
        }
    }

    color c = scene.background;

    if (mat_id != "") {
        n = unit_vec(n);
        point3 x = r.at(t_min);
        const material_st &mat = scene.materials.at(mat_id);
        c = mat.ambient * scene.ambient_light;
        vec3 w_o = unit_vec(scene.camera.position - r.at(t_min));

        for (auto &l : scene.p_lights) {
            vec3 l_to_x = l.position - x;
            vec3 w_i = unit_vec(l_to_x);
            double dist_l = l_to_x.len();
            bool shadow = false;
            ray s = ray(x + EPS * w_i, w_i);
            for (auto &o : scene.objects) {
                for (auto &f : o.faces) {
                    double t_x;
                    if (f.intersect(s, 0, INF, t_x)) {
                        double dist_obj = (s.at(t_x) - s.origin()).len();
                        if (dist_obj < dist_l) {
                            shadow = true;
                            break;
                        }
                    }
                }
                if (shadow)
                    break;
            }
            if (!shadow) {
                color E_i = l.intensity / pow(dist_l, 2);
                double cos_t = max(0, dot(n, w_i));

                c += mat.diffuse * cos_t * E_i;

                vec3 w_o = unit_vec(scene.camera.position - r.at(t));
                vec3 h = unit_vec(w_i + w_o);

                double cos_a = max(0, dot(n, h));

                c += mat.specular * pow(cos_a, mat.phong_exp) * E_i;
            }
        }
        if (mat.mirror_refl.len() > 0 && depth > 0) {
            vec3 w_r = -w_o + 2 * n * dot(n, w_o);
            c += mat.mirror_refl *
                 ray_color(scene, ray(x + w_r * EPS, w_r), depth - 1);
        }
    }
    return c;
}

struct thread_info {
    int start_pixel = 0, n_pixel = 0;
    vector<color> pixels;
};

void thread_job(thread_info &info, const Scene &scene,
                const scene_st &old_scene) {
    fprintf(stdout, "Calculating pixels [%d,%d]...\n", info.start_pixel,
            info.start_pixel + info.n_pixel);
    int nx = scene.camera.nx;

    for (int i = info.start_pixel, j = info.start_pixel / nx;
         i < info.start_pixel + info.n_pixel; ++i) {
        if (i % nx == 0)
            ++j;
        ray r = scene.camera.ray_to_pixel(i % nx, j);
        // ray r = eye_ray_to_pixel(old_scene.camera, i % nx, j);

        info.pixels.push_back(new_ray_color(scene, r, 6));
    }

    fprintf(stdout, "Pixels [%d,%d] are calculated\n", info.start_pixel,
            info.start_pixel + info.n_pixel);
}

void raytracing_threaded(Scene &scene, scene_st &old_scene, ostream &out) {
    const int nThreads = thread::hardware_concurrency();

    thread_info info[nThreads];
    int total_pixel = scene.camera.nx * scene.camera.ny;
    int n_pixel = total_pixel / nThreads;
    int leftover = total_pixel % nThreads;

    for (int i = 0; i < nThreads; ++i) {
        info[i].start_pixel = i * n_pixel;
        info[i].n_pixel = n_pixel;
    }
    info[nThreads - 1].n_pixel += leftover;

    auto start = chrono::high_resolution_clock::now();
    thread th[nThreads];
    for (int i = 0; i < nThreads; ++i) {
        th[i] = thread(thread_job, ref(info[i]), cref(scene), cref(old_scene));
    }
    for (int i = 0; i < nThreads; ++i) {
        th[i].join();
    }
    auto duration = chrono::duration_cast<chrono::milliseconds>(
        chrono::high_resolution_clock::now() - start);
    cout << "Rendering is completed in " << duration.count() / 1000.0
         << " seconds.\n";

    out << "P3\n" << scene.camera.nx << " " << scene.camera.ny << "\n255\n";
    for (int i = 0; i < nThreads; ++i) {
        for (color &c : info[i].pixels) {
            write_color(out, c);
        }
    }
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        cerr << "No scene specified!" << endl;
        cerr << "Usage: ./rtrace <path_to_scene> <output_path>(optional)"
             << endl;
    }
    scene_st scene;
    if (!scene_from_file(scene, argv[1])) {
        cerr << "PARSING ERROR, TERMINATING." << endl;
        return -1;
    }

    string path = "rtrace_out.ppm";
    if (argc > 2)
        path = argv[2];

    ofstream out{path, ios::out};
    if (!out.is_open())
        cerr << "Error: Output file" << path << "cannot be opened." << endl;

    Scene s = dummy_scene();
    raytracing_threaded(s, scene, out);
    return 0;
}
