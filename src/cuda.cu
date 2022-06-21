#include <cuda_runtime.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "utils/helper_cuda.h"
#include "utils/node.h"
#include "utils/utilities.h"

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

std::vector<Body *> bodies;
ParallelBody *cudaBodies;
ParallelBody *cudaBodies_d;

double theta = 0.8;
size_t maxStackSize = 0;
int numNodes = 0;

int NUM_THREADX = 64;
// Building a separate tree for access on cuda
// preprocessing

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
        // std::cout<< cudaBodies[i].position[0]<<" "<<std::endl;
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
    double eps = 1.0;
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

void BarnesHutCuda(int N, int numIterations) {
    std::pair<Node *, ParallelNode *> octreePair;

    checkCudaErrors(cudaMalloc(&cudaBodies_d, sizeof(ParallelBody) * N));

    for (int i = 0; i < numIterations; i++) {
        std::pair<Node *, ParallelNode *> octreePair = CopyTreeToDevice();
        checkCudaErrors(cudaMemcpy(cudaBodies_d, cudaBodies, sizeof(ParallelBody) * N, cudaMemcpyHostToDevice));

        dim3 dimBlock(NUM_THREADX, 1, 1);
        dim3 dimGrid((bodies.size() + NUM_THREADX) / NUM_THREADX, 1, 1);

        BarnesHutKernel<<<dimGrid, dimBlock, maxStackSize * NUM_THREADX * sizeof(ParallelNode *)>>>(octreePair.second, maxStackSize, cudaBodies_d, N, theta);
        checkCudaErrors(cudaDeviceSynchronize());

        checkCudaErrors(cudaMemcpy(cudaBodies, cudaBodies_d, sizeof(ParallelBody) * N, cudaMemcpyDeviceToHost));
        // FreeTree(octreePair.first, octreePair.second);
        delete octreePair.first;
        UpdateBodies(cudaBodies);
    }
}

int main(int argc, char *argv[]) {
    /* 1 argument
     * FILENAME: The file name to read data from
     */
    if (argc != 3) {
        std::cout << "Incorrect number of parameters given:"
                  << " 2 required, " << argc - 1 << " given" << std::endl
                  << "FILENAME: The file name to read data from" << std::endl
                  << "I: Number of iterations to run the simulations for" << std::endl;
        return 0;
    }
    std::string filename(argv[1]);
    int numIterations = atoi(argv[2]);

    // open the file
    std::ifstream inFile("../data/" + filename);

    // the number of particles, magnitude of the position,magnitude of the mass
    int N, P, M;
    inFile >> N >> P >> M;
    inFile.ignore();

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
    inFile.close();

    auto t1 = high_resolution_clock::now();
    BarnesHutCuda(N, numIterations);
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    std::cout << 1.0 * ms_int.count() / numIterations << "ms  average for " << numIterations << " iterations, cuda\n";

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