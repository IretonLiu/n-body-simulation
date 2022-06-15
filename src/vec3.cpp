#include "vec3.h"

vec3 &vec3::operator+=(const vec3 &other) {
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;

    return *this;
}

vec3 &vec3::operator=(const vec3 &other) {
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;

    return *this;
}

vec3 vec3::operator/(long double d) {
    vec3 vec;
    vec.x = this->x / d;
    vec.y = this->y / d;
    vec.z = this->z / d;
    return vec;
}

vec3 vec3::operator*(long double d) {
    vec3 vec;
    vec.x = this->x * d;
    vec.y = this->y * d;
    vec.z = this->z * d;
    return vec;
}

vec3 vec3::operator-() {
    vec3 vec;
    vec.x = -this->x;
    vec.y = -this->y;
    vec.z = -this->z;

    return vec;
}

vec3 vec3::operator-(const vec3 &other) {
    vec3 vec;
    vec.x = this->x - other.x;
    vec.y = this->y - other.y;
    vec.z = this->z - other.z;
    return vec;
}

vec3 vec3::operator+(const vec3 &other) {
    vec3 vec;
    vec.x = this->x + other.x;
    vec.y = this->y + other.y;
    vec.z = this->z + other.z;
    return vec;
}