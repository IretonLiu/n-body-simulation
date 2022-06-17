#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "OpenGL/gl_utilities.h"
#include "utils/node.h"
#include "utils/utilities.h"

// #define MAGNITUDE 5

std::vector<Body*> bodies;
double theta = 0.8;

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

void BruteForce() {
    auto t1 = high_resolution_clock::now();

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
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    std::cout << ms_int.count() << "ms for bruteforce\n";
}

void BarnesHut() {
    // TODO: time this

    Node* octree = new Node(bodies);

    auto t1 = high_resolution_clock::now();
    octree->ConstructTree();
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    std::cout << ms_int.count() << "ms for contructrion of tree\n";

    t1 = high_resolution_clock::now();
    for (unsigned int i = 0; i < bodies.size(); i++) {
        Body* body = bodies[i];

        // reset the force acting on the body
        body->resetForce();

        octree->CalculateForces(body, theta);
    }
    for (Body* body : bodies) body->update();
    t2 = high_resolution_clock::now();
    ms_int = duration_cast<milliseconds>(t2 - t1);
    std::cout << ms_int.count() << "ms for force\n";

    delete octree;
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
    double mass;
    double x;
    double y;
    double z;

    // create the bodies

    // minimum and maximum bounds of the entire system
    vec3 minBound;
    vec3 maxBound;

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

    // close the file
    inFile.close();

    // call the brute force
    // perform a number of iterations

    int glProgramID = initGLProgram("Serial");
    render(bodies, P, M, BarnesHut);
    // for (int i = 0; i < 1; i++)
    // BruteForce();

    // BarnesHut();

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