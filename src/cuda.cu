#include <cuda_runtime.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "OpenGL/gl_utilities.h"
#include "utils/helper_cuda.h"
#include "utils/node.h"
#include "utils/utilities.h"

std::vector<Body *> bodies;
ParallelBody *cudaBodies;

double theta = 0.8;
size_t maxStackSize = 0;
int numNodes = 0;

std::stringstream data("6 6 15 9.116473579367844e+15 1.701604548859206e+05 1.305680358210422e+05 -2.130698223755301e+04 8.401877171547095e+15 -2.032763069026615e+04 -1.811363407102262e+05 1.140483239660925e+05 3.943829268190930e+15 -1.791981915863382e+05 5.544912736748732e+03 1.241392662871304e+05 7.830992237586059e+15 5.601020441137095e+03 -8.777286769754969e+04 7.708867393003816e+04 7.984400334760733e+15 3.344990699950628e+04 3.541127793886492e+05 6.195353842703000e+04 1.000000000000000e+19 0.000000000000000e+00 0.000000000000000e+00 0.000000000000000e+00");
int NUM_THREADX = 1;
// Building a separate tree for access on cuda
// preprocessing

template <typename T>
void DebugPrint(T info) {
    std::cout << info << std::endl;
}

__host__ std::pair<Node *, ParallelNode *> CopyTreeToDevice() {
    Node *octree = new Node(bodies);
    octree->ConstructTree();
    // ParallelNode *cudaOctree_h;
    ParallelNode *cudaOctree_d;
    checkCudaErrors(cudaMalloc(&cudaOctree_d, sizeof(ParallelNode)));

    std::pair<Node *, ParallelNode *> octree_pair = std::make_pair(octree, cudaOctree_d);

    std::stack<std::pair<Node *, ParallelNode *>> traversalStack;
    traversalStack.push(octree_pair);
    // traverse the original tree
    numNodes++;
    while (!traversalStack.empty()) {
        maxStackSize = std::max(traversalStack.size(), maxStackSize);
        std::pair<Node *, ParallelNode *> currNodePair = traversalStack.top();
        traversalStack.pop();

        if (currNodePair.first->GetNumBodies() < 1) continue;

        ParallelNode *cudaNode_h = new ParallelNode();
        ParallelNode *cudaNode_d = currNodePair.second;

        // copy the bare minimum info needed for barneshut to a Cuda Node

        cudaNode_h->centreOfMass[0] = currNodePair.first->GetCOM(true).x;
        cudaNode_h->centreOfMass[1] = currNodePair.first->GetCOM(true).y;
        cudaNode_h->centreOfMass[2] = currNodePair.first->GetCOM(true).z;
        cudaNode_h->numBodies = currNodePair.first->GetNumBodies();
        cudaNode_h->mass = currNodePair.first->GetMass();
        cudaNode_h->diagDist = std::sqrt(DistSquared(currNodePair.first->GetMaxBound(), currNodePair.first->GetMinBound()));

        if (currNodePair.first->GetNumBodies() != 1) {
            for (int i = 0; i < 8; i++) {
                // allocate each child on the device
                ParallelNode *child_d;
                checkCudaErrors(cudaMalloc(&child_d, sizeof(ParallelNode)));

                // keep track of the address of children from the device on host
                cudaNode_h->deviceChildren[i] = child_d;
                currNodePair.first->devicechildren[i] = child_d;
                traversalStack.push(std::make_pair(currNodePair.first->GetChildren()[i], child_d));
                numNodes++;
            }
        };

        // copy the data over
        if (currNodePair.second == cudaOctree_d)
            checkCudaErrors(cudaMemcpy(cudaOctree_d, cudaNode_h, sizeof(ParallelNode), cudaMemcpyHostToDevice));
        else
            checkCudaErrors(cudaMemcpy(cudaNode_d, cudaNode_h, sizeof(ParallelNode), cudaMemcpyHostToDevice));

        delete cudaNode_h;
    }

    return octree_pair;
}

void FreeTree(Node *octree, ParallelNode *cudaOctree_d) {
    std::stack<Node *> traversalStack;

    traversalStack.push(octree);

    while (!traversalStack.empty()) {
        Node *currNode = traversalStack.top();
        traversalStack.pop();
        if (currNode->GetNumBodies() <= 1) continue;

        for (int i = 0; i < 8; i++) {
            // clean up gpu memory
            ParallelNode *deviceChild = currNode->devicechildren[i];
            checkCudaErrors(cudaFree(deviceChild));

            traversalStack.push(currNode->GetChildren()[i]);
        }
    }

    checkCudaErrors(cudaFree(cudaOctree_d));
    delete octree;
}

void UpdateBodies(ParallelBody *cudaBodies) {
    for (int i = 0; i < bodies.size(); i++) {
        bodies[i]->position.x = cudaBodies[i].position[0];
        bodies[i]->position.y = cudaBodies[i].position[1];
        bodies[i]->position.z = cudaBodies[i].position[2];

        bodies[i]->velocity.x = cudaBodies[i].velocity[0];
        bodies[i]->velocity.y = cudaBodies[i].velocity[1];
        bodies[i]->velocity.z = cudaBodies[i].velocity[2];
    }
}

__device__ double DistSquared_d(double *p1, double *p2) {
    double dist = 0.0;
    for (int i = 0; i < 3; i++) {
        dist += (p1[i] - p2[i]) * (p1[i] - p2[i]);
    }
    return dist;
}

__device__ bool isEqual(double one, double two) {
    double eps = 0.1;
    return (fabs(one - two) < eps);
}

__device__ bool SameBody(ParallelBody *body, ParallelNode *node) {
    if (node->numBodies != 1) return false;
    if (!isEqual(body->mass, node->mass)) return false;
    for (int i = 0; i < 3; i++) {
        if (!isEqual(body->position[i], node->centreOfMass[i])) return false;
    }

    return true;
}

__device__ void CalcuateForce(ParallelBody *body, ParallelNode *node, double *totalForce) {
    // TODO: if no work, check G
    double dist = DistSquared_d(body->position, node->centreOfMass);

    double magnitude = G * body->mass * node->mass * (1 / dist);
    double force[3];

    dist = sqrt(dist);
    for (int i = 0; i < 3; i++) {
        force[i] = magnitude * (node->centreOfMass[i] - body->position[i]) / dist;
    }

    for (int i = 0; i < 3; i++)
        totalForce[i] += force[i];
}

__global__ void BarnesHutKernel(ParallelNode *octree_d, size_t maxStackSize_d, ParallelBody *bodies_d, int N, double theta) {
    int threadID = blockIdx.x * blockDim.x + threadIdx.x;

    if (threadID >= N) return;

    ParallelBody *body = (ParallelBody *)bodies_d + threadID;
    double totalForce[3] = {0, 0, 0};

    extern __shared__ ParallelNode *traversalStack[];
    int stackIndex = threadIdx.x * maxStackSize_d;
    int topIndex = -1;  // index of the top element in the stack

    // pushing to stack
    topIndex += 1;
    traversalStack[topIndex + stackIndex] = octree_d;

    int counter = 0;
    // output_d[1] = 420.0;
    while (topIndex != -1) {
        // peeking and popping
        ParallelNode *currNode = traversalStack[topIndex + stackIndex];
        topIndex--;

        if (SameBody(body, currNode)) continue;
        double d = sqrt(DistSquared_d(body->position, currNode->centreOfMass));
        double s = currNode->diagDist;

        if ((s / d) > theta && currNode->numBodies != 1) {
            for (int i = 0; i < 8; i++) {
                ParallelNode *child = currNode->deviceChildren[i];

                if (child->numBodies != 0) {
                    topIndex++;
                    traversalStack[topIndex + stackIndex] = child;
                }
            }
        } else {
            CalcuateForce(body, currNode, totalForce);
        }
    }

    for (int i = 0; i < 3; i++) {
        body->velocity[i] += totalForce[i] / body->mass;
        body->position[i] += body->velocity[i];
    }
}

int main(int argc, char *argv[]) {
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
    std::string filename(argv[1]);
    std::ifstream inFile("../data/" + filename);

    // the number of particles, magnitude of the position,magnitude of the mass
    int N, P, M;
    inFile >> N >> P >> M;
    inFile.ignore();
    // data >> N >> P >> M;

    // data.ignore();
    // helper variables for reading input
    std::string line;
    double mass;
    double x, y, z;

    bodies.reserve(N);
    cudaBodies = new ParallelBody[N];
    for (int i = 0; i < N; i++) {
        // read each body
        std::getline(inFile, line);

        // create a string stream
        std::stringstream ss(line);

        ss >> mass >> x >> y >> z;

        vec3 position(x, y, z);

        bodies.push_back(new Body(mass, position));
        bodies[i]->velocity = bodies[i]->position / std::pow(10, P - 3);
        bodies[i]->velocity.rot90z();

        cudaBodies[i].position[0] = position.x;
        cudaBodies[i].position[1] = position.y;
        cudaBodies[i].position[2] = position.z;
        cudaBodies[i].velocity[0] = bodies[i]->velocity.x;
        cudaBodies[i].velocity[1] = bodies[i]->velocity.y;
        cudaBodies[i].velocity[2] = bodies[i]->velocity.z;
        cudaBodies[i].mass = mass;
    }

    // ParallelNode *cudaNode_h = new ParallelNode();
    // ParallelNode *cudaNode_d;
    // cudaNode_h->mass = 123.0;

    // checkCudaErrors(cudaMalloc(&cudaNode_d, sizeof(ParallelNode)));
    // checkCudaErrors(cudaMemcpy(cudaNode_d, cudaNode_h, sizeof(ParallelNode), cudaMemcpyHostToDevice));
    std::pair<Node *, ParallelNode *> octreePair;
    ParallelBody *cudaBodies_d;
    checkCudaErrors(cudaMalloc(&cudaBodies_d, sizeof(ParallelBody) * N));

    int numIterations = 100;
    for (int i = 0; i < numIterations; i++) {
        octreePair = CopyTreeToDevice();

        checkCudaErrors(cudaMemcpy(cudaBodies_d, cudaBodies, sizeof(ParallelBody) * N, cudaMemcpyHostToDevice));

        dim3 dimBlock(NUM_THREADX, 1, 1);
        dim3 dimGrid((bodies.size() + NUM_THREADX) / NUM_THREADX, 1, 1);

        BarnesHutKernel<<<dimGrid, dimBlock, maxStackSize * NUM_THREADX * sizeof(ParallelNode *)>>>(octreePair.second, maxStackSize, cudaBodies_d, N, theta);
        checkCudaErrors(cudaDeviceSynchronize());

        checkCudaErrors(cudaMemcpy(cudaBodies, cudaBodies_d, sizeof(ParallelBody) * N, cudaMemcpyDeviceToHost));
        FreeTree(octreePair.first, octreePair.second);
        UpdateBodies(cudaBodies);
				std::cout<<i<<std::endl;
    }

    std::ofstream outFile("../out/cuda-" + filename);
    outFile.precision(15);

    outFile << N << std::endl;
    outFile << std::scientific;
    for (Body *body : bodies) {
        outFile << body->mass
                << " " << body->position.x
                << " " << body->position.y
                << " " << body->position.z
                << std::endl;
        delete body;
    }

    outFile.close();

    checkCudaErrors(cudaFree(cudaBodies_d));
    delete[] cudaBodies;
    checkCudaErrors(cudaDeviceReset());
}