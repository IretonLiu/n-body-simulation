#pragma once

#include <signal.h>
#include <stdlib.h>

#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#define ASSERT(x, y) \
    if (!(x > y)) raise(SIGTRAP)

#define G 6.67430e-11

class Body {
   public:
    long double mass;
    long double x;
    long double y;

    inline std::string toString() {
        return std::to_string(mass) + " " + std::to_string(x) + " " + std::to_string(y);
    }

    Body(long double mass, long double x, long double y) : mass(mass), x(x), y(y){};
};

inline float DistSquared(float x1, float y1, float x2, float y2) {
    return std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2);
}

inline float Newtons(float m1, float m2, float r) {
    return G * m1 * m2 * (1 / std::pow(r, 2));
}

/* Generate a n bodies with magnitude P and M
 * @param n number of bodies
 * @param P magnitude of the position
 * @param M magnitude of the mass
 */
std::vector<Body*> GenerateNBodies(int n, int P, int M);
