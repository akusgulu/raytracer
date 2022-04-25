#ifndef VEC3_H
#define VEC3_H

#include <iostream>
class vec3
{
public:
    vec3() : vec3(0, 0, 0) {}
    vec3(double e0, double e1, double e2) : x{e0}, y{e1}, z{e2} {}

    vec3 operator-() const { return vec3(-x, -y, -z); }

    vec3 &operator=(const vec3 &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

public:
    double x, y, z;
};

std::ostream &operator<<(std::ostream &out, const vec3 &v)
{
    return out << v.x << ',' << v.y << ',' << v.z;
}

vec3 operator+(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

vec3 operator-(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

vec3 operator*(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

vec3 operator*(const double d, const vec3 &v)
{
    return vec3(d * v.x, d * v.y, d * v.z);
}
vec3 operator*(const vec3 &v, const double d)
{
    return d * v;
}

vec3 operator/(const double d, const vec3 &v)
{
    return 1 / d * v;
}

double dot(const vec3 &v1, const vec3 &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vec3 cross(const vec3 &u, const vec3 &v)
{
    return vec3(u.y * v.z - u.z * v.y,
                u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x);
}

// Type aliases for vec3
using point3 = vec3; // 3D point
using color = vec3;  // RGB color

#endif