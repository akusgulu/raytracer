#pragma once

#include "ray.h"
#include "vec3.h"

struct HitRecord {
    double t;
    vec3 normal;
    int mat_id;
};

class Hittable {
  public:
    virtual bool hit(const ray &r, const double &t_min, const double &t_max,
                     HitRecord &rec) const = 0;
};