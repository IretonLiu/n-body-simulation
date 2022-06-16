#include <cuda_runtime.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "OpenGL/gl_utilities.h"
#include "utils/helper_cuda.h"
#include "utils/node.h"
// #include "utils/utilities.h"s

std::vector<Body *> bodies;
double theta = 0.8;

struct cudaNode {
    long double centreOfMass[3];
    long double mass;
    int children[8];
};

// __global__ void BarnesHutKernel(Node *) {
// }

main(int argc, char *argv[]) {
    /* 1 argument
     * FILENAME: The file name to read data from
     */

    if (argc != 2) {
        std::cout << "Incorrect number of parameters given:"
                  << " 1 required, " << argc - 1 << " given" << std::endl
                  << "FILENAME: The file name to read data from" << std::endl;
        return 0;
    }

    // open the file
    std::string filename(argv[1]);
    std::ifstream inFile("../data/" + filename);

    // the number of particles
    int N;

    // magnitude of the position
    int P;

    // magnitude of the mass
    int M;

    inFile >> N >> P >> M;
    inFile.ignore();

    // helper variables for reading input
    std::string line;
    long double mass;
    long double x;
    long double y;
    long double z;

    bodies.reserve(N);
    for (int i = 0; i < N; i++) {
        // read each body
        std::getline(inFile, line);

        // create a string stream
        std::stringstream ss(line);

        ss >> mass >> x >> y >> z;

        vec3 position(x, y, z);

        bodies.push_back(new Body(mass, position));
        bodies[i]->velocity = bodies[i]->position / std::pow(10, P - 3);
        bodies[i]->velocity.rot90z();
    }
}