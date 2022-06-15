#pragma once

#include <cmath>
class vec3 {
   public:
    long double *data;
    long double &x;
    long double &y;
    long double &z;
    vec3() : data(new long double[3]), x(data[0]), y(data[1]), z(data[2]){};
    vec3(long double x, long double y, long double z) : data(new long double[3]{x, y, z}), x(data[0]), y(data[1]), z(data[2]){};
    ~vec3() {
        delete[] data;
    };

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
