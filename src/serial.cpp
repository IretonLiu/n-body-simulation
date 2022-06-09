#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "utilities.h"

void BruteForce(std::vector<Body*> bodies) {
  // reset the force acting on each body
  // compute all the forces acting on each body
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
  std::ifstream inFile("../data/" + std::string(argv[1]));

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
  BruteForce(bodies);

  // write all the output and free all the bodies
  std::ofstream outFile("../out/" + std::string(argv[4]));

  outFile << N << std::endl;
  outFile << std::scientific;
  for (Body* body : bodies) {
    outFile << body->mass << " " << body->x << " " << body->y << std::endl;
    delete body;
  }

  outFile.close();

  return 0;
}