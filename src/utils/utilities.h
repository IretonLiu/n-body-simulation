#pragma once

#include <assert.h>
#include <signal.h>
#include <stdlib.h>

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "body.h"
#include "vec3.h"

#define G 6.67430e-11

#define ITERATIONS 2000

class Body;

inline double DistSquared(double x1, double y1, double x2, double y2) {
    return std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2);
}

inline double DistSquared(const vec3 &p1, const vec3 &p2) {
    return std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2) + std::pow(p1.z - p2.z, 2);
}

inline double NewtonsGravitationalLaw(double m1, double m2, double rSquared) {
    return G * m1 * m2 * (1 / (rSquared));
}

/**
 * @brief Get the maximum possible position a body can initialise at
 *
 * @param P the magitude of the positions
 * @return the maximum possible position
 */
double GetMaxPosition(int P);
