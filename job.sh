#!/bin/bash
#SBATCH -J Test        # Job Name
#SBATCH -o Test.o%j    # Output and error file name (%j expands to jobID)
#SBATCH -p development  # Queue (partition) name -- normal, development, etc.
#SBATCH -t 01:30:00     # Run time (hh:mm:ss) - 1.5 hours
#SBATCH -n 5
#SBATCH -N 5

export OMP_NUM_THREADS = 8;
#rm *.csv *.plt Test*	
#ibrun ./distributedSort > code_output
export LD_LIBRARY_PATH=$HOME/cilk/lib64/:$PWD:$LD_LIBRARY_PATH

ibrun tacc_affinity ./a.out 1000 > code_output1000sm
ibrun tacc_affinity ./a.out 48807355 > code_output48807355sm
ibrun tacc_affinity ./a.out 97613710 > code_output97613710sm
ibrun tacc_affinity ./a.out 146420065 > code_output146420065sm
ibrun tacc_affinity ./a.out 195226425 > code_output195226425sm
ibrun tacc_affinity ./a.out 244032780 > code_output244032780sm
ibrun tacc_affinity ./a.out 292839135 > code_output292839135sm
ibrun tacc_affinity ./a.out 341645490 > code_output341645490sm
ibrun tacc_affinity ./a.out 390451845 > code_output390451845sm
ibrun tacc_affinity ./a.out 439258200 > code_output439258200sm
ibrun tacc_affinity ./a.out 488064555 > code_output488064555sm
ibrun tacc_affinity ./a.out 536870910 > code_output536870910sm



