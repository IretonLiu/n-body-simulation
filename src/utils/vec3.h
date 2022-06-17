#pragma once

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#else
#define CUDA_CALLABLE_MEMBER
#endif

#include <cmath>
class vec3 {
   public:
    double x;
    double y;
    double z;
    CUDA_CALLABLE_MEMBER vec3() : x(0), y(0), z(0){};
    CUDA_CALLABLE_MEMBER vec3(double x, double y, double z) : x(x), y(y), z(z){};

    CUDA_CALLABLE_MEMBER vec3 &operator+=(const vec3 &other);
    CUDA_CALLABLE_MEMBER vec3 &operator=(const vec3 &other);
    CUDA_CALLABLE_MEMBER vec3 operator/(double d);
    CUDA_CALLABLE_MEMBER vec3 operator*(double d);
    CUDA_CALLABLE_MEMBER vec3 operator-();
    CUDA_CALLABLE_MEMBER vec3 operator-(const vec3 &other);
    CUDA_CALLABLE_MEMBER vec3 operator+(const vec3 &other);
    CUDA_CALLABLE_MEMBER bool operator>(const vec3 &other);
    CUDA_CALLABLE_MEMBER bool operator<(const vec3 &other);

    CUDA_CALLABLE_MEMBER void rot90z();
};
