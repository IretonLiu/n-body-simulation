#include <cstdlib>
#include <fstream>
#include <iostream>
// #include <string>

#include "utilities.h"

int main(int argc, char* argv[]) {
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
    std::vector<Body*> nBodies = GenerateNBodies(n, P, M);

    // write to file
    std::ofstream dataFile("../data/" + std::string(argv[4]));
    dataFile.precision(30);

    dataFile << n + 1 << " " << P << " " << M << std::endl;
    dataFile << std::scientific;
    for (Body* body : nBodies) {
        dataFile << body->mass
                 << " " << body->position.x
                 << " " << body->position.y
                 << " " << body->position.z
                 << std::endl;
        delete body;
    }

    return 0;
}