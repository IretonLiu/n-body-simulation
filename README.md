# N Body Simulation

### William Hill (2115261) and Ireton Liu (2089889)

## Structure

The project consists of 5 executables:

1. A data generator: this will be used to generate the initial problem configuration (i.e. the bodies' masses and initial positions)
2. A Serial implementation of Barnes Hut
3. A Cuda implementation of Barnes Hut
4. An MPI implementation of Barnes Hut
5. A validator to compare the outputs of each of the 3 Barnes Hut implementations to ensure correctness (Serial is used as the baseline)

The directory structure is as follows:

- `bin`: contains the binary executables
- `cluster`: contains _output_ and _error_ sub-folders containing the output from executing experiments on the cluster
- `data`: contains pre-generated test data
- `out`: contains the output of the runs using the different Barnes Hut implementations (each file is prepended with the implemenation used to create it, e.g. `cuda-example`)
- `src`: contains the source code for the project
- `CMakeLists.txt`: contains the information required to compile the executables
- `jobslurm.slurm`: batch file for creating a job on the cluster and initiating the experiments
- `run.sh`: run script to ease the execution of the executables

## How to run

### Requirements:

- cmake (version 3.10.2) _(this is the same version on the cluster, that we used for experimentation)_

### Compile:

- Navigate to the root of the n-body-simulation directory
- Run `cmake .`

### Execution:

The run script will be used to execute the files. In the root of the n-body-simulation directory (i.e. at the same level as the run script), execute the following:

#### Enable the run script

- `chmod +x run.sh`: grant the run script execute permission

#### Generate Data

- `./run.sh DataGenerator <N> <P> <M> <filename>`: generate new data, to be stored in a file in `data/`
  - `N`: is the number of particles
  - `P`: is the order of magnitude to use for the positions of the particles
  - `M`: is the order of magnitude to use for the mass of the particles
  - `filename`: is _only_ the name of the file to which to write the data
- **Example:** `./run.sh DataGenerator 5000 5 5 example`

#### Execute Serial Barnes Hut

- `./run.sh Serial <filename> <iterations>`: execute the Serial Barnes Hut implementation:
  - `filename`: the filename _only_ of the file in `data/` to use for initialising the problem
  - `iterations`: the number of iterations for which to simulate
- **Example:** `./run.sh Serial example 5`

#### Execute Cuda Barnes Hut

- `./run.sh Cuda <filename> <iterations>`: execute the Cuda Barnes Hut implementation:
  - `filename`: the filename _only_ of the file in `data/` to use for initialising the problem
  - `iterations`: the number of iterations for which to simulate
- **Example:** `./run.sh Cuda example 5`

#### Execute MPI Barnes Hut

- `./run.sh -m MPI <num processes> <filename> <iterations>`: execute the MPI Barnes Hut implementation:
  - `num processes`: the number of MPI processes to launch
  - `filename`: the filename _only_ of the file in `data/` to use for initialising the problem
  - `iterations`: the number of iterations for which to simulate
- **Example:** `./run.sh -m MPI 6 example 5`

#### Execute Validator

- `./run.sh Validate <filename>`: validate the correctness of the output of the MPI and Cuda implementations against the output of the Serial implementation:
  - `filename`: the filename _only_ of the file in `data/` that was used for initialising the problem
- **Example:** `./run.sh Validate example`
