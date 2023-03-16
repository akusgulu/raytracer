#include "ray.h"
#include "scene.h"
#include "mesh.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include "image.h"
using namespace std;

#define EPS 0.0000001
#define INF numeric_limits<double>::infinity()
double max(const double a, const double b) {
    return a > b ? a : b;
}
color ray_color(const Scene &scene, const ray &r, const int depth) {
    HitRecord closest_hit;

    if (scene.hit(r, 0, INF, closest_hit)) {
        color c = scene.background;
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
                 ray_color(scene, ray(x + w_r * EPS, w_r), depth - 1);
        }
        return c;
    }

    return scene.background;
}

struct thread_info {
    int start_pixel = 0, n_pixel = 0;
    vector<color> pixels;
};

void thread_job(thread_info &info, const Scene &scene) {
    fprintf(stdout, "Calculating pixels [%d,%d]...\n", info.start_pixel,
            info.start_pixel + info.n_pixel);
    int nx = scene.camera.nx;

    for (int i = info.start_pixel, j = info.start_pixel / nx;
         i < info.start_pixel + info.n_pixel; ++i) {
        if (i % nx == 0)
            ++j;
        ray r = scene.camera.ray_to_pixel(i % nx, j);
        info.pixels.push_back(ray_color(scene, r, 6));
    }

    fprintf(stdout, "Pixels [%d,%d] are calculated\n", info.start_pixel,
            info.start_pixel + info.n_pixel);
}

void raytracing_threaded(Scene &scene, Image &img) {
    const unsigned int nThreads = thread::hardware_concurrency();

    std::vector<thread_info> info{nThreads};
    int total_pixel = scene.camera.nx * scene.camera.ny;
    int n_pixel = total_pixel / nThreads;
    int leftover = total_pixel % nThreads;

    for (unsigned int i = 0; i < nThreads; ++i) {
        info[i].start_pixel = i * n_pixel;
        info[i].n_pixel = n_pixel;
    }
    info[nThreads - 1].n_pixel += leftover;

    auto start = chrono::high_resolution_clock::now();
    std::vector<thread> th{nThreads};
    for (unsigned int i = 0; i < nThreads; ++i) {
        th[i] = thread(thread_job, ref(info[i]), cref(scene));
    }
    for (unsigned int i = 0; i < nThreads; ++i) {
        th[i].join();
    }
    auto duration = chrono::duration_cast<chrono::milliseconds>(
        chrono::high_resolution_clock::now() - start);
    cout << "Rendering is completed in " << duration.count() / 1000.0
         << " seconds.\n";

    int i = 0;
    int j = 0;
    for (auto &th : info) {
        for (color &c : th.pixels) {
            img.set_pixel(i, j, c);
            ++i;
            if (i == scene.camera.nx) {
                ++j;
                i = 0;
            }
        }
    }
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        cerr << "No scene specified!" << endl;
        cerr << "Usage: ./rtrace <path_to_scene> <output_path>(optional)"
             << endl;
    }
    Scene scene;
    if (!scene_from_xml_file(scene, argv[1])) {
        cerr << "PARSING ERROR, TERMINATING." << endl;
        return -1;
    }

    string path = "rtrace_out.ppm";
    if (argc > 2)
        path = argv[2];

    ofstream out{path, ios::out};
    if (!out.is_open())
        cerr << "Error: Output file" << path << "cannot be opened." << endl;

    Image img(scene.camera.nx, scene.camera.ny);

    raytracing_threaded(scene, img);
    img.export_ppm(out);
    return 0;
}
