#include "utilities.h"

#define DELTA 1

inline int RandomRange() {
  return rand() % (2 * DELTA + 1) - DELTA;
}

static Body *GenerateBody(int P, int M) {
  int p = RandomRange() + P;  // magnitude of position
  int m = RandomRange() + M;  // magnitude of mass

  // mantissa of position
  long double x = 10.0 * rand() / RAND_MAX;
  long double y = 10.0 * rand() / RAND_MAX;

  // mantissa of the mass
  long double mass = 10.0 * rand() / RAND_MAX;

  Body *body = new Body(mass * std::pow(10, m), x * std::pow(10, p), y * std::pow(10, p));

  return body;
}

void Body::accumulateForce(long double forceX, long double forceY) {
  // update the force
  fx += forceX;
  fy += forceY;
}

void Body::resetForce() {
  // set the componenets to zero
  fx = 0;
  fy = 0;
}

void Body::update() {
  // work out the acceleration to apply from the force
  // use the acc to change the vel
  // F = ma

  // use the force to update vel
  vx += fx / mass;
  vy += fy / mass;

  // use the vel to change the pos
  x += vx;
  y += vy;
}

std::pair<long double, long double> Body::calculateForce(Body *other) {
  // use Newton's Gravitational Law for this
  double rSquared = DistSquared(this->x, this->y, other->x, other->y);
  double magnitude = NewtonsGravitationalLaw(this->mass, other->mass, rSquared);

  // determine the direction of the force (will be towards the other body)
  long double direcX = other->x - this->x;
  long double direcY = other->y - this->y;

  // normalise the direction
  long double norm = std::sqrt(rSquared);

  // take the component of force acting in each direction
  long double forceX = magnitude * (direcX / norm);
  long double forceY = magnitude * (direcY / norm);

  return std::pair<long double, long double>(forceX, forceY);
}

std::vector<Body *> GenerateNBodies(int n, int P, int M) {
  assert(P > DELTA);
  assert(M > DELTA);

  std::vector<Body *>
      nbodies(n);
  for (int i = 0; i < n; i++) {
    nbodies[i] = GenerateBody(P, M);
  }

  return nbodies;
}