#include "image.h"

Image::Image(int width, int height)
    : m_width{width}
    , m_height{height} {

    data = new color[width * height];
}

Image::~Image() {
    delete[] data;
}

void Image::set_pixel(int i, int j, color c) {
    if (i < 0 || i > m_width || j < 0 || j > m_height)
        return;

    data[j * m_width + i] = c;
}

color Image::get_pixel(int i, int j) const {
    if (i < 0 || i > m_width || j < 0 || j > m_height)
        return color(0, 0, 0);

    return data[j * m_width + i];
}

static double max(const double a, const double b) {
    return a > b ? a : b;
}

static int clamp(double a) {
    int x = static_cast<int>(a);
    return max(0, x > 255 ? 255 : x);
}

void Image::export_ppm(std::ostream &out) const {
    out << "P3\n" << m_width << " " << m_height << "\n255\n";
    for (int j = 0; j < m_height; ++j) {
        for (int i = 0; i < m_width; ++i) {
            out << clamp(data[j * m_width + i].x) << ' '
                << clamp(data[j * m_width + i].y) << ' '
                << clamp(data[j * m_width + i].z) << '\n';
        }
    }
}
