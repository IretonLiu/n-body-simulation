#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "utilities.h"

void BruteForce(std::vector<Body*>& bodies) {
  // reset the force acting on each body
  for (Body* body : bodies) {
    body->resetForce();
  }

  // compute all the forces acting on each body
  // loop until the second last body
  for (int i = 0; i < bodies.size() - 1; i++) {
    // compare this body to all that come after it
    for (int j = i + 1; j < bodies.size(); j++) {
      // get the force acting between the bodies
      auto forcePair = bodies[i]->calculateForce(bodies[j]);

      // add the forst to each body
      bodies[i]->accumulateForce(forcePair.first, forcePair.second);

      // negate the force for the second body (opp direc -> Newton 3)
      bodies[j]->accumulateForce(-forcePair.first, -forcePair.second);
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
  inFile >> N;
  inFile.ignore();

  // helper variables for reading input
  std::string line;
  long double mass;
  long double x;
  long double y;

  // create the bodies
  std::vector<Body*> bodies;
  bodies.reserve(N);
  for (int i = 0; i < N; i++) {
    // read each body
    std::getline(inFile, line);

    // create a string stream
    std::stringstream ss(line);

    ss >> mass >> x >> y;

    bodies.push_back(new Body(mass, x, y));
  }

  // close the file
  inFile.close();

  // call the brute force
  // perform a number of iterations
  for (int iters = 0; iters < ITERATIONS; iters++) {
    BruteForce(bodies);
  }

  // write all the output and free all the bodies
  std::ofstream outFile("../out/" + filename);

  outFile << N << std::endl;
  outFile << std::scientific;
  for (Body* body : bodies) {
    outFile << body->mass << " " << body->x << " " << body->y << std::endl;
    delete body;
  }

  outFile.close();

  return 0;
}