#pragma once

#include <assert.h>
#include <signal.h>
#include <stdlib.h>

#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#define G 6.67430e-11

#define ITERATIONS 1

class Body {
   public:
    long double mass;

    // position
    long double x;
    long double y;

    // velocity
    long double vx = 0;
    long double vy = 0;

    // force
    long double fx = 0;
    long double fy = 0;

    // do NOT use for writing data to file - scientific notation not present
    inline std::string toString() {
        return std::to_string(mass) + " " + std::to_string(x) + " " + std::to_string(y);
    }

    Body(long double mass, long double x, long double y) : mass(mass), x(x), y(y){};

    // changes the force
    void accumulateForce(long double forceX, long double forceY);

    // set the force to zero
    void resetForce();

    // updates the position
    // don't need a time step since this will be called at a regular rate
    void update();

    // calculate force exerted on this body by another body
    std::pair<long double, long double> calculateForce(Body *other);
};

inline long double DistSquared(long double x1, long double y1, long double x2, long double y2) {
    return std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2);
}

inline long double NewtonsGravitationalLaw(long double m1, long double m2, long double rSquared) {
    return G * m1 * m2 * (1 / rSquared);
}

/* Generate a n bodies with magnitude P and M
 * @param n number of bodies
 * @param P magnitude of the position
 * @param M magnitude of the mass
 */
std::vector<Body *> GenerateNBodies(int n, int P, int M);
