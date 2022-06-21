#!/bin/bash
# These lines are for slurm

# specify a partition by its name in which to run your job. 
#SBATCH -p batch
# specify number of nodes you are requesting to run your job 
#SBATCH -N 2
# specify number of tasks/processes
##SBATCH -n 8
# specify the job name 
#SBATCH -J n-body

# specify the filename to be used for writing output.
# You need to put the correct path and the output file name. %N is name of node; %j is job number
#SBATCH -o /home-mscluster/whill/hpc/n-body-simulation/cluster/output/average.%N.%j.out 
# specify the filename for stderr
#SBATCH -e /home-mscluster/whill/hpc/n-body-simulation/cluster/error/average.%N.%j.err

# The following are shell commands -- which will be executed according to the settings specified above

# print extra info about the job and its execution state
echo ----------------------------------------------------------------
echo DATE: $(date)
echo Job is running on nodes $SLURM_JOB_NODELIST
echo ----------------------------------------------------------------
echo SLURM: sbatch is running $SLURM_SUBMIT_HOST
echo SLURM: job ID is $SLURM_JOB_ID
echo SLURM: submit directory is $SLURM_SUBMIT_DIR
echo SLURM: number of nodes allocated is $SLURM_JOB_NUM_NODES
# echo SLURM: number of tasks is $SLURM_NTASKS
#echo SLURM: number of cores is $SLURM_CPUS_PER_TASK
echo SLURM: job name is $SLURM_JOB_NAME
echo ----------------------------------------------------------------
cd $SLURM_SUBMIT_DIR

# run the script

# remove and recompile executables
# ./run.sh -c

declare -a numProcesses=(4 6 8 10 12)
numIterations=5

# loop through all the test files
for data in `ls data`; 
do 
  echo "--------------------Testing file: $data --------------------"

  # run serial
  echo "----- Testing Serial -----"
  ./run.sh Serial $data $numIterations
  echo ""

  # run cuda
  echo "----- Testing Cuda -----"
  ./run.sh Cuda $data $numIterations
  echo ""

  # run MPI, for a number of different processes
  echo "----- Testing MPI -----"
  for numProc in "${numProcesses[@]}"
  do 
    echo "$numProc processes -----"
    ./run.sh -m MPI $numProc $data $numIterations
    echo ""
  done;

  echo "----- Validating -----"
  ./run.sh Validate $data
  echo ""

done;

echo ""
echo "---------- Done! ----------"
