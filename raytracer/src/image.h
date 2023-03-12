#pragma once

#include "vec3.h"

class Image {
  public:
    Image(int width, int height);
    ~Image();
    void set_pixel(int i, int j, color c);
    color get_pixel(int i, int j) const;
    void export_ppm(std::ostream &out) const;

  private:
    int m_width, m_height;
    color *data;
};