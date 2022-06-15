#pragma once
#include <vector>

#include "body.h"
#include "vec3.h"

class Node {
   public:
    Node(vec3 minBound, vec3 maxBound, std::vector<Body*> bodies);
    Node(vec3 minBound, vec3 maxBound, int maxBodies);

    inline vec3 GetMinBound() { return minBound; };
    inline vec3 GetMaxBound() { return maxBound; };
    inline vec3 GetCentre() { return (minBound + maxBound) / 2; };

    inline vec3 GetHalfSize() { return (maxBound - minBound) / 2; };

    inline vec3 GetCOM() { return centreOfMass; };
    inline long double GetMass() { return mass; };

    /** Construct the octree recursively
     */
    void ConstructTree();

   private:
    /*
     * 0 1
     * 2 3
     *
     * 4 5
     * 6 7
     */
    std::vector<Node*> children;

    std::vector<Body*> bodies;

    vec3 centreOfMass;  // not normalized
    vec3 minBound;
    vec3 maxBound;

    long double mass;

    /**Set the correct minBound and maxBound for all the children in this node
     */
    void ConstructChildren();
};