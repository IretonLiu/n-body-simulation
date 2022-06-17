#include "body.h"

Body::Body(double mass, double x, double y, double z) : mass(mass) {
    position.x = x;
    position.y = y;
    position.z = z;
}

Body::Body(double mass, const vec3 &position) : mass(mass) {
    this->position = position;
}

void Body::accumulateForce(const vec3 &force) {
    // update the force
    this->force += force;
}

void Body::resetForce() {
    // set the componenets to zero
    force.x = 0;
    force.y = 0;
    force.z = 0;
}

void Body::update() {
    // work out the acceleration to apply from the force
    // use the acc to change the vel
    // F = ma

    // use the force to update vel
    velocity += force / mass;

    // use the vel to change the pos
    position += velocity;
}

vec3 Body::calculateForce(Body *other) {
    // use Newton's Gravitational Law for this
    double rSquared = DistSquared(this->position, other->position);
    double magnitude = NewtonsGravitationalLaw(this->mass, other->mass, rSquared);

    // determine the direction of the force (will be towards the other body)
    // double direcX = other->x - this->x;
    // double direcY = other->y - this->y;

    vec3 direction = other->position - this->position;

    // normalise the direction
    double norm = std::sqrt(rSquared);

    // take the component of force acting in each direction
    vec3 force = (direction / norm) * magnitude;

    return force;
}

vec3 Body::calculateForce(vec3 centreOfMass, double mass) {
    // use Newton's Gravitational Law for this
    double rSquared = DistSquared(this->position, centreOfMass);
    double magnitude = NewtonsGravitationalLaw(this->mass, mass, rSquared);

    // determine the direction of the force (will be towards the other body)
    // double direcX = other->x - this->x;
    // double direcY = other->y - this->y;

    vec3 direction = centreOfMass - this->position;

    // normalise the direction
    double norm = std::sqrt(rSquared);

    // take the component of force acting in each direction
    vec3 force = (direction / norm) * magnitude;

    return force;
}