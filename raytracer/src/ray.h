#ifndef RAY_H
#define RAY_H
#include "vec3.h"
#include <iostream>
class ray {
  public:
    ray(const point3 &origin, const vec3 &direction) {
        this->o = origin;
        this->d = direction;
    }

    inline point3 origin() const {
        return o;
    }
    inline vec3 direction() const {
        return d;
    }

    inline point3 at(const double t) const {
        return o + t * d;
    }

  private:
    point3 o;
    vec3 d;
};

#endif