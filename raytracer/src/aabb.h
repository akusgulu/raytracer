#pragma once

#include "hittable.h"
#include "vec3.h"

class AABB {
  public:
    AABB() = default;

    AABB(const point3 &minPoint, const point3 &maxPoint)
        : m_minPoint{minPoint}
        , m_maxPoint{maxPoint} {
    }

    virtual bool hit(const ray &r, const double &t_min,
                     const double &t_max) const;

    point3 m_minPoint;
    point3 m_maxPoint;
};