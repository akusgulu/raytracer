#include "aabb.h"

bool AABB::hit(const ray &r, const double &t_min, const double &t_max) const {

    double min_diff[3], max_diff[3];
    double min = t_min;
    double max = t_max;

    min_diff[0] = (m_minPoint.x - r.origin().x) / r.direction().x;
    max_diff[0] = (m_maxPoint.x - r.origin().x) / r.direction().x;

    min_diff[1] = (m_minPoint.y - r.origin().y) / r.direction().y;
    max_diff[1] = (m_maxPoint.y - r.origin().y) / r.direction().y;

    min_diff[2] = (m_minPoint.z - r.origin().z) / r.direction().z;
    max_diff[2] = (m_maxPoint.z - r.origin().z) / r.direction().z;

    for (int a = 0; a < 3; a++) {
        min = fmax(fmin(min_diff[a], max_diff[a]), t_min);
        max = fmin(fmax(min_diff[a], max_diff[a]), t_max);
        if (max <= min)
            return false;
    }
    return true;
}