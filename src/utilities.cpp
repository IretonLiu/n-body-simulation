#include "utilities.h"

#define DELTA 1

inline int RandomRange() {
    return rand() % (2 * DELTA + 1) - DELTA;
}

static Body* GenerateBody(int P, int M) {
    int p = RandomRange() + P;  // magnitude of position
    int m = RandomRange() + M;  // magnitude of mass

    // mantissa of position
    long double x = 10.0 * rand() / RAND_MAX;
    long double y = 10.0 * rand() / RAND_MAX;

    // mantissa of the mass
    long double mass = 10.0 * rand() / RAND_MAX;

    Body* body = new Body(mass * std::pow(10, m), x * std::pow(10, p), y * std::pow(10, p));

    return body;
}

std::vector<Body*> GenerateNBodies(int n, int P, int M) {
    assert(P > DELTA);
    assert(M > DELTA);

    std::vector<Body*>
        nbodies(n);
    for (int i = 0; i < n; i++) {
        nbodies[i] = GenerateBody(P, M);
    }

    return nbodies;
}