#ifndef RAY_H
#define RAY_H
#include "vec3.h"
#include <iostream>
class ray
{
public:
    ray(const point3 &origin, const vec3 &direction)
    {
        this->o = origin;
        this->d = direction;
    }

    point3 origin() const { return o; }
    vec3 direction() const { return d; }

    point3 at(const double t)
    {
        return o + t * d;
    }

private:
    point3 o;
    vec3 d;
};

std::ostream &operator<<(std::ostream &out, const ray& r)
{
    return out << r.origin() << " + " << r.direction();
}

#endif