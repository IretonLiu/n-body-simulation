#include "utilities.h"

std::vector<Body *> GenerateNBodies(int n, int P, int M) {
    std::vector<Body *> nbodies(n + 1);

    std::default_random_engine generator;
    std::normal_distribution<long double> distribution(0.5, 1.0 / 6.0);

    for (int i = 0; i < n; i++) {
        long double x = distribution(generator);
        if (x < 0)
            x = 0.0;
        else if (x > 1.0)
            x = 1.0;

        long double y = distribution(generator);
        if (y < 0)
            y = 0.0;
        else if (y > 1.0)
            y = 1.0;

        long double z = distribution(generator);
        if (z < 0)
            z = 0.0;
        else if (z > 1.0)
            z = 1.0;

        long double mass = 10.0 * rand() / RAND_MAX;
        Body *body = new Body(mass * std::pow(10, M), x * std::pow(10, P), y * std::pow(10, P), z * std::pow(10, P));

        nbodies[i] = body;
    }

    nbodies[n] = new Body(1 * std::pow(10, (M + 4)), 0.5 * std::pow(10, P), 0.5 * std::pow(10, P), 0.5 * std::pow(10, P));
    return nbodies;
}
