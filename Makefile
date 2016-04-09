CC1=cilk++
CC2=mpicxx
CFLAGS=-O3\
	-fPIC\
	-shared
	
#-I $(HOME)/cilk/include/cilk++
all: 
	$(CC1) $(CFLAGS) -o libpm.so cilk_sample_sort_pm.cilk

	$(CC2) distributed_sample_sort_pm.cpp -o dist_samplesort_pm -L. -L$(HOME)/cilk/lib64 -Wl,-rpath=. -lpm -lcilk_main -lcilkrts -lcilkutil

	$(CC1) $(CFLAGS) -o libsm.so cilk_sample_sort_sm.cilk

	$(CC2) distributed_sample_sort_sm.cpp -o dist_samplesort_sm -L. -L$(HOME)/cilk/lib64 -Wl,-rpath=. -lsm -lcilk_main -lcilkrts -lcilkutil





clean:
	rm dist_samplesort_* code_* Test*
