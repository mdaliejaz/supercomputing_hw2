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

ibrun tacc_affinity ./a.out 536870880 > threadpm
#ibrun tacc_affinity ./a.out 48807355 > code_output488pm
#ibrun tacc_affinity ./a.out 97613710 > code_output976pm
#ibrun tacc_affinity ./a.out 146420065 > code_output146pm
#ibrun tacc_affinity ./a.out 195226425 > code_output196pm
#ibrun tacc_affinity ./a.out 244032780 > code_output244pm
#ibrun tacc_affinity ./a.out 292839135 > code_output292pm
#ibrun tacc_affinity ./a.out 341645490 > code_output341pm
#ibrun tacc_affinity ./a.out 390451845 > code_output390pm
#ibrun tacc_affinity ./a.out 439258200 > code_output439pm
#ibrun tacc_affinity ./a.out 488064555 > code_output48806455pm
#ibrun tacc_affinity ./a.out 536870910 > code_output536pm




