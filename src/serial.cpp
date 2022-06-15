#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "OpenGL/gl_utilities.h"
#include "node.h"
#include "utilities.h"

// #define MAGNITUDE 5

std::vector<Body*> bodies;

void BruteForce(std::vector<Body*>& bodies) {
    // reset the force acting on each body
    for (Body* body : bodies) {
        body->resetForce();
    }

    // compute all the forces acting on each body
    // loop until the second last body
    for (unsigned int i = 0; i < bodies.size() - 1; i++) {
        // compare this body to all that come after it
        for (unsigned int j = i + 1; j < bodies.size(); j++) {
            // get the force acting between the bodies
            auto force = bodies[i]->calculateForce(bodies[j]);

            // add the forst to each body
            bodies[i]->accumulateForce(force);

            // negate the force for the second body (opp direc -> Newton 3)
            bodies[j]->accumulateForce(-force);
        }

        // body "i" has done all force comparisons
        // update it
        bodies[i]->update();
    }

    // update the last body
    bodies[bodies.size() - 1]->update();
}

void BarnesHut() {
}

int main(int argc, char* argv[]) {
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

    // create the bodies
    // std::vector<Body*> bodies;
    bodies.reserve(N);
    for (int i = 0; i < N; i++) {
        // read each body
        std::getline(inFile, line);

        // create a string stream
        std::stringstream ss(line);

        ss >> mass >> x >> y >> z;

        bodies.push_back(new Body(mass, x, y, z));
        bodies[i]->velocity.x = x / std::pow(10, P - 3);
        bodies[i]->velocity.y = y / std::pow(10, P - 3);
        bodies[i]->velocity.z = z / std::pow(10, P - 3);
    }

    // close the file
    inFile.close();

    // call the brute force
    // perform a number of iterations

    int glProgramID = initGLProgram("Serial");
    render(bodies, P, M, BruteForce);

    // write all the output and free all the bodies
    std::ofstream outFile("../out/" + filename);
    outFile.precision(30);

    outFile << N << std::endl;
    outFile << std::scientific;
    for (Body* body : bodies) {
        outFile << body->mass
                << " " << body->position.x
                << " " << body->position.y
                << " " << body->position.z
                << std::endl;
        delete body;
    }

    outFile.close();

    return 0;
}