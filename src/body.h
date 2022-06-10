#pragma once

#include "utilities.h"
#include "vec3.h"

class Body {
   public:
    long double mass;

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

    Body(long double mass, long double x, long double y, long double z);

    // changes the force
    void accumulateForce(const vec3 &force);

    // set the force to zero
    void resetForce();

    // updates the position
    // don't need a time step since this will be called at a regular rate
    void update();

    // calculate force exerted on this body by another body
    vec3 calculateForce(Body *other);
};