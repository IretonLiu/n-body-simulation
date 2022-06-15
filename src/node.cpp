#include "node.h"

void Node::ConstructChildren() {
    vec3 centre = GetCentre();
    vec3 halfsize = GetHalfSize();

    std::vector<vec3> mins{
        // top half
        vec3(centre.x - halfsize.x, centre.y, centre.z),
        vec3(centre.x, centre.y, centre.z),
        vec3(centre.x - halfsize.x, centre.y, centre.z - halfsize.z),
        vec3(centre.x, centre.y, centre.z - halfsize.z),

        // bottom half
        vec3(centre.x - halfsize.x, centre.y - halfsize.y, centre.z),
        vec3(centre.x, centre.y - halfsize.y, centre.z),
        vec3(centre.x - halfsize.x, centre.y - halfsize.y, centre.z - halfsize.z),
        vec3(centre.x, centre.y - halfsize.y, centre.z - halfsize.z),
    };

    int maxBodies = bodies.size();
    for (int i = 0; i < 8; i++) {
        children[i] = new Node(mins[i], mins[i] + halfsize, maxBodies);
    }
}

Node::Node(vec3 minBound, vec3 maxBound, std::vector<Body*> bodies) {
    this->minBound = minBound;
    this->maxBound = maxBound;
    this->bodies = bodies;
    this->children = std::vector<Node*>(8);
    centreOfMass = vec3(0, 0, 0);
};

Node::Node(vec3 minBound, vec3 maxBound, int maxBodies) {
    this->minBound = minBound;
    this->maxBound = maxBound;
    this->bodies.reserve(maxBodies);
    this->children = std::vector<Node*>(8);
    centreOfMass = vec3(0, 0, 0);
};

void Node::ConstructTree() {
    if (bodies.size() == 1) {
        centreOfMass = bodies[0]->position * bodies[0]->mass;
        mass = bodies[0]->mass;
        return;
    }

    if (bodies.size() < 1) return;

    // if there is more than one body in this node, then construct the subtree of this node
    vec3 centre = (minBound + maxBound) / 2;
    ConstructChildren();

    int index = 0;

    for (int i = 0; i < bodies.size(); i++) {
        Body* body = bodies[i];
        vec3 pos = body->position;
        if (pos.y >= centre.y) {  // top half
            index += 0;
        } else {  // bottom half
            index += 4;
        }

        if (pos.x >= centre.x) {  // rigth half
            index += 1;
        } else {  // left half
            index += 0;
        }

        if (pos.z >= centre.z) {  // front half
            index += 0;
        } else {  // back half
            index += 2;
        }

        children[index]->bodies.push_back(body);

        mass += body->mass;
        centreOfMass += body->position * body->mass;
    }

    for (int i = 0; i < 8; i++) {
        children[i]->bodies.shrink_to_fit();
        children[i]->ConstructTree();
    }

    return;
}
