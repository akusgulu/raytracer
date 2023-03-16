#pragma once

#include "ray.h"
#include "vec3.h"
#include <string>

struct HitRecord {
    double t;
    vec3 normal;
    std::string mat_id;
};

class Hittable {
  public:
    virtual bool hit(const ray &r, const double &t_min, const double &t_max,
                     HitRecord &rec) const = 0;

    virtual bool initBoundingBox() = 0;
};