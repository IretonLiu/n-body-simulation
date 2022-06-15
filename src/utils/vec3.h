#pragma once

#include <cmath>
class vec3 {
   public:
    long double x;
    long double y;
    long double z;
    vec3() : x(0), y(0), z(0){};
    vec3(long double x, long double y, long double z) : x(x), y(y), z(z){};

    vec3 &operator+=(const vec3 &other);
    vec3 &operator=(const vec3 &other);
    vec3 operator/(long double d);
    vec3 operator*(long double d);
    vec3 operator-();
    vec3 operator-(const vec3 &other);
    vec3 operator+(const vec3 &other);
    bool operator>(const vec3 &other);
    bool operator<(const vec3 &other);

    void rot90z();
};
