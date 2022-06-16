#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>

// #include <string>

#include "utils/utilities.h"

/**
 * @brief Generate a n bodies with magnitude P and M
 *
 * @param n number of bodies
 * @param P magnitude of the position
 * @param M magnitude of the mass
 */
std::vector<Body *> GenerateNBodies(int n, int P, int M) {
    std::vector<Body *> nbodies(n + 1);

    std::default_random_engine generator;
    std::normal_distribution<long double> distribution(0.0, 1.0 / 6.0);

    for (int i = 0; i < n; i++) {
        double x = distribution(generator);
        x = std::min(std::max(x, -0.5), 0.5);

        double y = distribution(generator);
        y = std::min(std::max(y, -0.5), 0.5);

        double z = distribution(generator);
        z = std::min(std::max(z, -0.5), 0.5);

        long double mass = 10.0 * rand() / RAND_MAX;
        Body *body = new Body(mass * std::pow(10, M), x * std::pow(10, P), y * std::pow(10, P), z * std::pow(10, P));

        nbodies[i] = body;
    }

    nbodies[n] = new Body(1 * std::pow(10, (M + 4)), 0.0, 0.0, 0.0);
    return nbodies;
}

int main(int argc, char *argv[]) {
    /* 4 argument
     * N: The number of particles
     * P: Order of magnitude for positions
     * M: Order of magnitude for mass
     * FILENAME: The file name to write data to, or read data from
     */

    if (argc != 5) {
        std::cout << "Incorrect number of parameters given:"
                  << " 4 required, " << argc - 1 << " given" << std::endl
                  << "N: The number of particles" << std::endl
                  << "P: Order of magnitude for positions" << std::endl
                  << "M: Order of magnitude for mass" << std::endl
                  << "FILENAME: The file name to write data to, or read data from" << std::endl;
        return 0;
    }

    int n = atoi(argv[1]);
    int P = atoi(argv[2]);
    int M = atoi(argv[3]);
    std::vector<Body *> nBodies = GenerateNBodies(n, P, M);

    // write to file
    std::ofstream dataFile("../data/" + std::string(argv[4]));
    dataFile.precision(30);

    dataFile << n + 1 << " " << P << " " << M << std::endl;
    dataFile << std::scientific;
    for (Body *body : nBodies) {
        dataFile << body->mass
                 << " " << body->position.x
                 << " " << body->position.y
                 << " " << body->position.z
                 << std::endl;
        delete body;
    }

    return 0;
}