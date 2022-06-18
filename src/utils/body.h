#pragma once

#include "utilities.h"
#include "vec3.h"

struct ParallelBody {
    double mass;
    double position[3];
    double velocity[3];
};

class Body {
   public:
    double mass;

    // position
    vec3 position;

    // velocity
    vec3 velocity;

    // force
    vec3 force;

    // do NOT use for writing data to file - scientific notation not present
    // inline std::string toString() {
    //     return std::to_string(mass) + " " + std::to_string(.x) + " " + std::to_string(y);
    // }

    Body(double mass, double x, double y, double z);

    Body(double mass, const vec3 &position);

    // changes the force
    void accumulateForce(const vec3 &force);

    // set the force to zero
    void resetForce();

    // updates the position
    // don't need a time step since this will be called at a regular rate
    void update();

    // calculate force exerted on this body by another body
    vec3 calculateForce(Body *other);

    // calcuate force based on centre of mass
    vec3 calculateForce(vec3 centreOfMass, double mass);
};