CC=mpic++
CFLAGS=-O3 
#-I $(HOME)/cilk/include/cilk++
all:	 
	$(CC) $(CFLAGS) -o distributedSort distributedSort.cpp

clean:
	rm distributedSort code_* Test*
