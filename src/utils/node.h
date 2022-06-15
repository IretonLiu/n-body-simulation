#pragma once
#include <stack>
#include <vector>

#include "body.h"
#include "vec3.h"

class Node {
   public:
    /** Constructor for the root node of the octree
     * @param bodies a vector of all the bodies in the system
     */
    Node(std::vector<Body*>& bodies);

    /** Constructor for a child node of the octree
     * @param minBound minimum bound of the octree cube
     * @param maxBound maximum bound of the octree cube
     * @param maxBodies maximum number of bodies that can be in this child node
     */
    Node(vec3 minBound, vec3 maxBound, int maxBodies);

    inline vec3 GetMinBound() { return minBound; };
    inline vec3 GetMaxBound() { return maxBound; };
    inline vec3 GetCentre() { return (minBound + maxBound) / 2; };

    inline vec3 GetHalfSize() { return (maxBound - minBound) / 2; };

    inline vec3 GetCOM(bool normalise) { return normalise ? centreOfMass / mass : centreOfMass; };
    inline long double GetMass() { return mass; };

    /** Construct the octree recursively
     */
    void ConstructTree();

    void CalculateForces(Body* body, long double r);

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