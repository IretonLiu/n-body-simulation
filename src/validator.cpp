#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

bool isEqual(double one, double two) {
    double eps = 1.0;
    return (std::abs(one - two) < eps);
}

bool isIdentical(std::ifstream &file1, std::ifstream &file2) {
    double d1, d2;

    while (file1 >> d1 && file2 >> d2) {
        if (!isEqual(d1, d2)) {
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Incorrect number of parameters given:"
                  << " 1 required, " << argc - 1 << " given" << std::endl
                  << "Filename: the name of the input file used to run all the code" << std::endl;
        return 0;
    }

    std::string filename(argv[1]);

    std::string serialOutput = "../out/serial-" + filename;
    std::string cudaOutput = "../out/cuda-" + filename;
    std::string mpiOutput = "../out/mpi-" + filename;

    std::ifstream serialFile(serialOutput);
    std::ifstream cudaFile(cudaOutput);
    std::ifstream mpiFile(mpiOutput);

    if (isIdentical(serialFile, cudaFile)) {
        std::cout << " Serial output and Cuda output are identical" << std::endl;
    } else {
        std::cout << " Serial output and Cuda output are different" << std::endl;
    }
    serialFile.close();
    serialFile.open(serialOutput);
    if (isIdentical(serialFile, mpiFile)) {
        std::cout << " Serial output and MPI output are identical" << std::endl;
    } else {
        std::cout << " Serial output and MPI output are different" << std::endl;
    }
    cudaFile.close();
    mpiFile.close();
    serialFile.close();
}