#pragma once

#include <vector>
#include "hittable.h"
#include "vec3.h"
#include <string>

class Mesh : public Hittable {
  public:
    Mesh(const std::vector<point3> &vertices, const std::vector<int> &indices,
         const std::string &material_id)
        : m_vertices{vertices}
        , m_indices{indices}
        , mat_id{material_id} {};
    virtual bool hit(const ray &r, const double &t_min, const double &t_max,
                     HitRecord &rec) const;

  private:
    const std::vector<point3> &m_vertices;
    std::vector<int> m_indices;
    std::string mat_id;
};