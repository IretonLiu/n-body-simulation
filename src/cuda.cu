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
double theta = 0.8;
size_t maxStackSize = 0;
int numNodes = 0;

std::stringstream data("6 6 15 8.401877171547095e+15 -2.032763069026615e+04 -1.811363407102262e+05 1.140483239660925e+05 3.943829268190930e+15 -1.791981915863382e+05 5.544912736748732e+03 1.241392662871304e+05 7.830992237586059e+15 5.601020441137095e+03 -8.777286769754969e+04 7.708867393003816e+04 7.984400334760733e+15 3.344990699950628e+04 3.541127793886492e+05 6.195353842703000e+04 9.116473579367844e+15 1.701604548859206e+05 1.305680358210422e+05 -2.130698223755301e+04 1.000000000000000e+19 0.000000000000000e+00 0.000000000000000e+00 0.000000000000000e+00");
int NUM_THREADX = 1;
// Building a separate tree for access on cuda
// preprocessing
struct CudaNode {
    double centreOfMass[3];
    double mass;
    int numBodies;
    CudaNode *deviceChildren[8];
};

template <typename T>
void DebugPrint(T info) {
    std::cout << info << std::endl;
}

__host__ std::pair<Node *, CudaNode *> CopyTreeToDevice() {
    Node *octree = new Node(bodies);
    octree->ConstructTree();
    // CudaNode *cudaOctree_h;
    CudaNode *cudaOctree_d;
    checkCudaErrors(cudaMalloc(&cudaOctree_d, sizeof(CudaNode)));

    std::pair<Node *, CudaNode *> octree_pair = std::make_pair(octree, cudaOctree_d);

    std::stack<std::pair<Node *, CudaNode *>> traversalStack;
    traversalStack.push(octree_pair);
    // traverse the original tree
    numNodes++;
    while (!traversalStack.empty()) {
        maxStackSize = std::max(traversalStack.size(), maxStackSize);
        std::pair<Node *, CudaNode *> currNodePair = traversalStack.top();
        traversalStack.pop();

        if (currNodePair.first->GetNumBodies() < 1) continue;

        CudaNode *cudaNode_h = new CudaNode();
        CudaNode *cudaNode_d = currNodePair.second;

        // copy the bare minimum info needed for barneshut to a Cuda Node

        cudaNode_h->centreOfMass[0] = currNodePair.first->GetCOM(false).x;
        cudaNode_h->centreOfMass[1] = currNodePair.first->GetCOM(false).y;
        cudaNode_h->centreOfMass[2] = currNodePair.first->GetCOM(false).z;
        cudaNode_h->numBodies = currNodePair.first->GetNumBodies();
        cudaNode_h->mass = currNodePair.first->GetMass();

        if (currNodePair.first->GetNumBodies() != 1) {
            for (int i = 0; i < 8; i++) {
                // allocate each child on the device
                CudaNode *child_d;
                checkCudaErrors(cudaMalloc(&child_d, sizeof(CudaNode)));

                // keep track of the address of children from the device on host
                cudaNode_h->deviceChildren[i] = child_d;
                currNodePair.first->devicechildren[i] = child_d;
                traversalStack.push(std::make_pair(currNodePair.first->GetChildren()[i], child_d));
                numNodes++;
            }
        };

        // copy the data over
        if (currNodePair.second == cudaOctree_d)
            checkCudaErrors(cudaMemcpy(cudaOctree_d, cudaNode_h, sizeof(CudaNode), cudaMemcpyHostToDevice));
        else
            checkCudaErrors(cudaMemcpy(cudaNode_d, cudaNode_h, sizeof(CudaNode), cudaMemcpyHostToDevice));

        delete cudaNode_h;
    }

    return octree_pair;
}

void FreeDeviceTree(Node *octree, CudaNode *cudaOctree_d) {
    std::stack<Node *> traversalStack;

    traversalStack.push(octree);

    while (!traversalStack.empty()) {
        Node *currNode = traversalStack.top();
        traversalStack.pop();
        if (currNode->GetNumBodies() <= 1) continue;

        for (int i = 0; i < 8; i++) {
            // clean up gpu memory
            CudaNode *deviceChild = currNode->devicechildren[i];
            checkCudaErrors(cudaFree(deviceChild));

            traversalStack.push(currNode->GetChildren()[i]);
        }
    }

    checkCudaErrors(cudaFree(cudaOctree_d));
}

__global__ void BarnesHutKernel(CudaNode *octree_d, size_t maxStackSize_d, double *output_d) {
    extern __shared__ CudaNode *traversalStack[];
    int stackIndex = threadIdx.x * maxStackSize_d;
    int topIndex = -1;  // index of the top element in the stack

    // pushing to stack
    topIndex += 1;
    traversalStack[topIndex + stackIndex] = octree_d;

    int counter = 0;
    // output_d[1] = 420.0;
    while (topIndex != -1) {
        // peeking and popping
        CudaNode *currNode = traversalStack[topIndex + stackIndex];
        topIndex--;

        if (currNode->numBodies == 0) continue;
        output_d[counter] = currNode->mass;
        counter++;
        if (currNode->numBodies > 1) {
            for (int i = 0; i < 8; i++) {
                topIndex++;
                traversalStack[topIndex + stackIndex] = currNode->deviceChildren[i];
            }
        }
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
    // inFile >> N >> P >> M;
    // inFile.ignore();
    data >> N >> P >> M;
    // data.ignore();
    // helper variables for reading input
    std::string line;
    double mass;
    double x, y, z;

    bodies.reserve(N);
    for (int i = 0; i < N; i++) {
        // read each body
        // std::getline(inFile, line);

        // create a string stream
        // std::stringstream ss(line);

        data >> mass >> x >> y >> z;

        vec3 position(x, y, z);

        bodies.push_back(new Body(mass, position));
        bodies[i]->velocity = bodies[i]->position / std::pow(10, P - 3);
        bodies[i]->velocity.rot90z();
    }

    std::pair<Node *, CudaNode *> octreePair = CopyTreeToDevice();

    CudaNode *cudaNode_h = new CudaNode();
    CudaNode *cudaNode_d;
    cudaNode_h->mass = 123.0;

    checkCudaErrors(cudaMalloc(&cudaNode_d, sizeof(CudaNode)));

    checkCudaErrors(cudaMemcpy(cudaNode_d, cudaNode_h, sizeof(CudaNode), cudaMemcpyHostToDevice));

    Node *octree = new Node(bodies);
    octree->ConstructTree();
    octree->DFS();
    double *output = (double *)malloc(numNodes * sizeof(double));
    double *output_d;
    checkCudaErrors(cudaMalloc((void **)&output_d, sizeof(double) * numNodes));

    dim3 dimBlock(NUM_THREADX, 1, 1);
    dim3 dimGrid((bodies.size() + NUM_THREADX) / NUM_THREADX, 1, 1);

    BarnesHutKernel<<<1, 1, maxStackSize * 1 * sizeof(CudaNode *)>>>(octreePair.second, maxStackSize, output_d);
    // BarnesHutKernel<<<1, 1>>>(octreePair.second, maxStackSize, output_d);
    checkCudaErrors(cudaDeviceSynchronize());

    checkCudaErrors(cudaMemcpy(output, output_d, sizeof(double) * numNodes, cudaMemcpyDeviceToHost));

    for (int i = 0; i < numNodes; i++) {
        DebugPrint(output[i]);
    }

    FreeDeviceTree(octreePair.first, octreePair.second);

    checkCudaErrors(cudaFree(output_d));
    delete[] output;

    checkCudaErrors(cudaDeviceReset());
}