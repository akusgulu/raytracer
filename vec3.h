#ifndef VEC3_H
#define VEC3_H
#include <cmath>
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

    vec3& operator+=(const vec3 &v){
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    inline double len() const{
        return sqrt(pow(x,2)+pow(y,2)+pow(z,2));
    }

public:
    double x, y, z;
};

std::ostream &operator<<(std::ostream &out, const vec3 &v);
vec3 operator+(const vec3 &v1, const vec3 &v2);
vec3 operator-(const vec3 &v1, const vec3 &v2);
vec3 operator*(const vec3 &v1, const vec3 &v2);
vec3 operator*(const double d, const vec3 &v);
vec3 operator*(const vec3 &v, const double d);
vec3 operator/(const vec3 &v,const double d);
double dot(const vec3 &v1, const vec3 &v2);
vec3 cross(const vec3 &u, const vec3 &v);
vec3 unit_vec(const vec3 &v);

// Type aliases for vec3
using point3 = vec3; // 3D point
using color = vec3;  // RGB color

#endif