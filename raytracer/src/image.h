#pragma once

#include "vec3.h"

class Image {
  public:
    Image(const int width, const int height);
    ~Image();
    void set_pixel(const int i, const int j, const color &c);
    color get_pixel(const int i, const int j) const;
    void export_ppm(std::ostream &out) const;

  private:
    int m_width, m_height;
    color *data;
};