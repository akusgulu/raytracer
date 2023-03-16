#include "scene.h"

Material Scene::get_material(std::string id) const {
    for (auto &mat : materials) {
        if (mat.id == id) {
            return mat;
        }
    }
    return Material();
}

bool Scene::hit(const ray &r, const double t_min, const double t_max,
                HitRecord &rec) const {

    HitRecord temp;
    rec.t = t_max;
    bool is_hit = false;
    for (auto &o : hittables) {
        if (o->hit(r, t_min, t_max, temp) && rec.t >= temp.t) {
            rec = temp;
            is_hit = true;
        }
    }

    return is_hit;
}