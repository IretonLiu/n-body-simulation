#include "node.h"

void Node::ConstructChildren() {
    vec3 centre = GetCentre();
    vec3 halfsize = GetHalfSize();

    std::vector<vec3> mins(8);
    // top half
    mins[0] = vec3(centre.x - halfsize.x, centre.y, centre.z);
    mins[1] = vec3(centre.x, centre.y, centre.z);
    mins[2] = vec3(centre.x - halfsize.x, centre.y, centre.z - halfsize.z);
    mins[3] = vec3(centre.x, centre.y, centre.z - halfsize.z);
    // bottom half
    mins[4] = vec3(centre.x - halfsize.x, centre.y - halfsize.y, centre.z);
    mins[5] = vec3(centre.x, centre.y - halfsize.y, centre.z);
    mins[6] = vec3(centre.x - halfsize.x, centre.y - halfsize.y, centre.z - halfsize.z);
    mins[7] = vec3(centre.x, centre.y - halfsize.y, centre.z - halfsize.z);

    int maxBodies = bodies.size();
    for (int i = 0; i < 8; i++) {
        children[i] = new Node(mins[i] + vec3(0, 0, 0), mins[i] + halfsize, maxBodies);
    }
}

Node::Node(std::vector<Body*>& bodies) {
    this->bodies = bodies;
    this->children = std::vector<Node*>(8);
    centreOfMass = vec3(0, 0, 0);
    minBound = bodies[0]->position;
    maxBound = bodies[0]->position;

    for (Body* body : this->bodies) {
        vec3 position = body->position;
        minBound.x = std::min(minBound.x, position.x);
        minBound.y = std::min(minBound.y, position.y);
        minBound.z = std::min(minBound.z, position.z);

        maxBound.x = std::max(maxBound.x, position.x);
        maxBound.y = std::max(maxBound.y, position.y);
        maxBound.z = std::max(maxBound.z, position.z);
    }
};

Node::Node(vec3 minBound, vec3 maxBound, int maxBodies) {
    this->minBound = minBound;
    this->maxBound = maxBound;
    this->bodies.reserve(maxBodies);  // reserve enough space for the children
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

void Node::CalculateForces(Body* body, long double r) {
    // square for distance comparison
    long double rSquared = r * r;

    std::stack<Node*> traversalStack;
    traversalStack.push(this);

    while (traversalStack.size() > 0) {
        Node* currentNode = traversalStack.top();
        int numBodies = currentNode->bodies.size();
        traversalStack.pop();

        if (numBodies == 1 && currentNode->bodies[0] == body) continue;

        long double dSquared = DistSquared(body->position, currentNode->GetCOM(true));

        if (dSquared < rSquared && numBodies != 1) {
            for (int i = 0; i < 8; i++) {
                Node* child = currentNode->children[i];

                if (child->bodies.size() != 0)
                    traversalStack.push(child);
            }
        } else {
            vec3 force = body->calculateForce(currentNode->GetCOM(true), currentNode->mass);
            body->accumulateForce(force);
        }
    }
}