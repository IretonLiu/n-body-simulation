#pragma once
#include <assert.h>

#include <cmath>
#include <iostream>
#include <stack>
#include <vector>

#include "body.h"
#include "vec3.h"

struct ParallelNode {
    double centreOfMass[3];
    double mass;
    double diagDist;
    int numBodies;
    ParallelNode* deviceChildren[8];
};

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

    ~Node() {
        for (Node* child : children) {
            if (child) delete child;
        }
    }

    inline vec3 GetMinBound() { return minBound; };
    inline vec3 GetMaxBound() { return maxBound; };
    inline vec3 GetCentre() { return (minBound + maxBound) / 2; };

    inline vec3 GetHalfSize() { return (maxBound - minBound) / 2; };

    inline vec3 GetCOM(bool normalise) { return normalise ? centreOfMass / mass : centreOfMass; };
    inline double GetMass() { return mass; };

    inline std::vector<Node*> GetChildren() { return children; };
    inline int GetNumBodies() { return bodies.size(); };

    /** Construct the octree recursively
     */
    void ConstructTree();

    void CalculateForces(Body* body, double r);

    std::vector<ParallelNode*> devicechildren;

    std::string DFS();

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

    double mass;

    /**Set the correct minBound and maxBound for all the children in this node
     */
    void ConstructChildren();
};