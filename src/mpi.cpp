#include <mpi.h>

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
ParallelBody *mpiBodies;
double theta = 0.8;
int rank, nProcess;

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

Node *BuildTree() {
    Node *octree;

    octree = new Node(bodies);
    octree->ConstructTree();

    return octree;
}

void UpdateBodies(int N, bool createBodies) {
    for (int i = 0; i < N; i++) {
        if (createBodies)
            bodies.push_back(new Body(mpiBodies[i].mass, vec3(0, 0, 0)));
        bodies[i]->position.x = mpiBodies[i].position[0];
        bodies[i]->position.y = mpiBodies[i].position[1];
        bodies[i]->position.z = mpiBodies[i].position[2];
        bodies[i]->velocity.x = mpiBodies[i].velocity[0];
        bodies[i]->velocity.y = mpiBodies[i].velocity[1];
        bodies[i]->velocity.z = mpiBodies[i].velocity[2];
    }
}

void PrintBodies(int N) {
    std::string out = "";
    for (int i = 0; i < N; i++) {
        out += std::to_string(bodies[i]->position.x) + " ";
    }
    std::cout << out << std::endl;
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

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcess);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::string filename(argv[1]);
    int numIterations = atoi(argv[2]);

    int N, P, M;
    if (rank == 0) {
        // open the file
        std::ifstream inFile("../data/" + filename);

        // the number of particles, magnitude of the position,magnitude of the mass
        inFile >> N >> P >> M;

        inFile.ignore();
        // data >> N >> P >> M;

        // data.ignore();
        // helper variables for reading input
        std::string line;
        double mass;
        double x, y, z;

        bodies.reserve(N);
        mpiBodies = new ParallelBody[N];

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

            mpiBodies[i].position[0] = position.x;
            mpiBodies[i].position[1] = position.y;
            mpiBodies[i].position[2] = position.z;
            mpiBodies[i].velocity[0] = bodies[i]->velocity.x;
            mpiBodies[i].velocity[1] = bodies[i]->velocity.y;
            mpiBodies[i].velocity[2] = bodies[i]->velocity.z;
            mpiBodies[i].mass = mass;
        }
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) {
        bodies.reserve(N);
        mpiBodies = new ParallelBody[N];
    };

    // end is not inclusive
    int start = rank * (N / nProcess);
    int end = start + N / nProcess;

    // hacky solution
    int recvcounts[nProcess];
    int displs[nProcess];
    displs[0] = 0;
    recvcounts[0] = end - start;
    for (int i = 1; i < nProcess; i++) {
        recvcounts[i] = end - start;
        displs[i] = displs[i - 1] + end - start;
    }

    recvcounts[nProcess - 1] = N - (end - start) * (nProcess - 1);
    // revert back from hacky solution
    if (rank == nProcess - 1) {
        end = N;
    }

    MPI_Datatype BODY_TYPE;
    int blockLengths[3] = {1, 3, 3};
    MPI_Datatype types[3] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};

    ParallelBody dummyBody;
    MPI_Aint displacements[3];
    MPI_Aint base_address;
    MPI_Get_address(&dummyBody, &base_address);
    MPI_Get_address(&dummyBody.mass, &displacements[0]);
    MPI_Get_address(&dummyBody.position[0], &displacements[1]);
    MPI_Get_address(&dummyBody.velocity[0], &displacements[2]);
    displacements[0] = MPI_Aint_diff(displacements[0], base_address);
    displacements[1] = MPI_Aint_diff(displacements[1], base_address);
    displacements[2] = MPI_Aint_diff(displacements[2], base_address);

    MPI_Type_create_struct(3, blockLengths, displacements, types, &BODY_TYPE);
    MPI_Type_commit(&BODY_TYPE);

    auto t1 = high_resolution_clock::now();

    MPI_Bcast(mpiBodies, N, BODY_TYPE, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        UpdateBodies(N, true);
    }

    ParallelBody *recv = new ParallelBody[N];
    for (int i = 0; i < numIterations; i++) {
        Node *octree = BuildTree();

        for (int j = start; j < end; j++) {
            Body *body = bodies[j];
            body->resetForce();

            octree->CalculateForces(body, theta);
        }

        for (int j = start; j < end; j++) {
            Body *body = bodies[j];
            body->update();

            mpiBodies[j].position[0] = body->position.x;
            mpiBodies[j].position[1] = body->position.y;
            mpiBodies[j].position[2] = body->position.z;
            mpiBodies[j].velocity[0] = body->velocity.x;
            mpiBodies[j].velocity[1] = body->velocity.y;
            mpiBodies[j].velocity[2] = body->velocity.z;
        }

        // ParallelBody * recv;
        MPI_Allgatherv(&mpiBodies[start], end - start, BODY_TYPE, recv, recvcounts, displs, BODY_TYPE, MPI_COMM_WORLD);

        // swap points
        ParallelBody *temp;
        temp = mpiBodies;
        mpiBodies = recv;
        recv = temp;

        UpdateBodies(N, false);
        delete octree;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    if (rank == 0)
        std::cout << 1.0 * ms_int.count() / numIterations << "ms average for " << numIterations<<" iterations, mpi\n";

    if (rank == 0) {
        std::ofstream outFile("../out/mpi-" + filename);
        outFile.precision(15);

        outFile << N << std::endl;
        outFile << std::scientific;
        for (Body *body : bodies) {
            outFile << body->mass
                    << " " << body->position.x
                    << " " << body->position.y
                    << " " << body->position.z
                    << std::endl;
        }

        outFile.close();
    }

    for (Body *body : bodies) {
        delete body;
    }

    delete[] mpiBodies;
    delete[] recv;
    MPI_Type_free(&BODY_TYPE);
    MPI_Finalize();
}