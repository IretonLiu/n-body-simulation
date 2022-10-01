# N-Body Simulation
An n-body problem is one where we need to determine
the states of a number of particles (i.e. bodies) in a dynamical system as they interact with each other. The types of particles and the system in which they interact can vary, from atomic interactions in nuclear physics, to
molecular interactions in chemistry, and even to celestial
body interactions in astrophysics.

Here, the Barnes-Hut algorithm is implemented in serial as a baseline, and parallelised using MPI and Cuda.

The core idea of our parallel solution is to fully reconstruct the Barnes-Hut octree in:
1. Each process's memory for MPI
2. The device memory for Cuda

The parallelisation of each body is then trivial following the Barnes-Hut algorithm with a fully constructed octree.

