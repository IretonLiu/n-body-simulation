#include "utilities.h"

// std::vector<Body *> GenerateNBodies(int n, int P, int M) {
//     std::vector<Body *> nbodies(n + 1);

//     std::default_random_engine generator;
//     std::normal_distribution<double> distribution(0.0, 1.0 / 6.0);

//     for (int i = 0; i < n; i++) {
//         double x = distribution(generator);
//         x = std::min(std::max(x, -0.5), 0.5);

//         double y = distribution(generator);
//         y = std::min(std::max(y, -0.5), 0.5);

//         double z = distribution(generator);
//         z = std::min(std::max(z, -0.5), 0.5);

//         double mass = 10.0 * rand() / RAND_MAX;
//         Body *body = new Body(mass * std::pow(10, M), x * std::pow(10, P), y * std::pow(10, P), z * std::pow(10, P));

//         nbodies[i] = body;
//     }

//     nbodies[n] = new Body(1 * std::pow(10, (M + 4)), 0.0, 0.0, 0.0);
//     return nbodies;
// }
