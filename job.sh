#!/bin/bash
#SBATCH -J Test        # Job Name
#SBATCH -o Test.o%j    # Output and error file name (%j expands to jobID)
#SBATCH -N 5
#SBATCH -n 16          # Total number of  tasks requested

#SBATCH -p development  # Queue (partition) name -- normal, development, etc.
#SBATCH -t 01:30:00     # Run time (hh:mm:ss) - 1.5 hours

#rm *.csv *.plt Test*	
#ibrun ./distributedSort > code_output
export LD_LIBRARY_PATH=$HOME/cilk/lib64/:$PWD:$LD_LIBRARY_PATH

ibrun tacc_affinity ./a.out 1000 > code_output1000sm
ibrun tacc_affinity ./a.out 97613800 > code_output97613800sm
ibrun tacc_affinity ./a.out 195226605 > code_output195226605sm
ibrun tacc_affinity ./a.out 292839405 > code_output292839405sm
ibrun tacc_affinity ./a.out 390452210 > code_output390452210sm
ibrun tacc_affinity ./a.out 488065010 > code_output488065020sm
ibrun tacc_affinity ./a.out 585677815 > code_output585677815sm
ibrun tacc_affinity ./a.out 683290615 > code_output683290615sm
ibrun tacc_affinity ./a.out 780903420 > code_output780903420sm
ibrun tacc_affinity ./a.out 878516220 > code_output878516220sm
ibrun tacc_affinity ./a.out 976129020 > code_output976129020sm
ibrun tacc_affinity ./a.out 1073741825 > code_output1073741825sm




