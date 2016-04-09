#include<mpi.h>
#include<iostream>
#include<stdint.h>
#include<cstdlib>
#include<algorithm>
#include<math.h>
#include<limits.h>

using namespace std;
uint64_t num;
uint64_t noOfElems;

extern "C++" void cilk_sm_sort(double A[], uint64_t n, uint64_t r);

/*
 * Reference: http://www.cse.iitd.ernet.in/~dheerajb/MPI/codes/day-3/c/samplesort.c
 */
int binSearch(double *arr, int value, int left, int right, int size)
{
	while (left <= right) {
		int middle = (left + right) / 2;
		if (arr[middle] == value)
			return middle;
		else if (arr[middle] > value)
			right = middle - 1;
		else
			left = middle + 1;
	}

	if (left == size)
		return size;
	if (arr[left] > value)
		return left;
	return left + 1;
}

void printArray(double A[], int len)
{
	cout << "Printing array now  :" << endl;
	for (uint64_t i = 0 ; i < len; i++ )
	{
		cout << A[i] << "  " ;
	}
	cout << endl << endl;
}

int main( int argc, char *argv[ ] )
{
	int numOfProc, rank;
	MPI::Init ( argc, argv );
	numOfProc = MPI::COMM_WORLD.Get_size ( );
	double start1 = MPI_Wtime();
	rank = MPI::COMM_WORLD.Get_rank ( );
	double *inputData, *localData;
	int localDataLen ;
	int i, j;
	noOfElems = atoi(argv[1]);

	if (noOfElems % 5 != 0)
	{
		cout << "no is not multiple of 5" << endl;
		return 1;
	}

	if (rank == 0)
	{
		inputData = new double[noOfElems];

		for (i = 1; i <= noOfElems; i++)
			inputData[i - 1] = (double)i;

		for (i = 0; i < noOfElems; i++)
		{
			uint64_t x = rand() % noOfElems;
			uint64_t y = rand() % noOfElems;
			double temp =  inputData[x];
			inputData[x] = inputData[y];
			inputData[y] = temp;
		}
	}

	localDataLen =  noOfElems / numOfProc;
	localData = new double[localDataLen];

	MPI_Scatter(inputData, localDataLen, MPI_DOUBLE, localData, localDataLen, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	double start2 = MPI_Wtime();

	cilk_sm_sort(localData, 0, localDataLen - 1);

	double* localPivots;
	int localPivotLen = numOfProc - 1;
	localPivots = new double[localPivotLen];

	for (i = 1; i < numOfProc; i++)
		localPivots[i - 1] = localData[(i * localDataLen) / numOfProc];

	double* allPivots;
	int allPivotsLen = (numOfProc - 1) * numOfProc;
	allPivots = new double[allPivotsLen];

	MPI_Gather(localPivots, localPivotLen, MPI_DOUBLE, allPivots, localPivotLen, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		sort(allPivots, allPivots + allPivotsLen);
		for (i = 0; i < localPivotLen; i++)
			localPivots[i] = allPivots[localPivotLen * (i + 1)];
	}

	MPI_Bcast(localPivots, localPivotLen, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	double **buckets = new double*[numOfProc]; //for rows
	for (i = 0; i < numOfProc; i++)
		buckets[i] = new double[localDataLen];

	double*  binSearchPivotIndex = new double[localPivotLen];

	for (i = 0; i < localPivotLen; i++)
		binSearchPivotIndex[i] = binSearch(localData, localPivots[i], 0, localDataLen - 1, localDataLen);

	int prev , currIndex = 0;
	for (i = 0; i < localPivotLen; i++)
	{
		prev = i == 0 ? 0 : binSearchPivotIndex[i - 1];
		buckets[i][0] = binSearchPivotIndex[i] - prev; //buckets[i][0] is count of no of elements in this bucket
		for (j = 1; j <= buckets[i][0]; j++)
		{
			buckets[i][j] = localData[currIndex];
			currIndex++;
		}

	}


	buckets[numOfProc - 1][0] = localDataLen - binSearchPivotIndex[localPivotLen - 1];
	for (i = 1; i <= buckets[numOfProc - 1][0]; i++)
		buckets[numOfProc - 1][i] = localData[currIndex++];

	double *finalBucket = new double[numOfProc * localDataLen];

	for (i = 0; i < numOfProc; i++)
		MPI_Gather(buckets[i], localDataLen, MPI_DOUBLE, finalBucket, localDataLen, MPI_DOUBLE, i, MPI_COMM_WORLD);

	int k = 0;
	double* finalData = new double[noOfElems];

	for (i = 0; i < numOfProc * localDataLen; i = i + localDataLen)
	{
		for (j = 1; j <= finalBucket[i]; j++)
			finalData[k++] = finalBucket[j + i];
	}

	cilk_sm_sort(finalData, 0, k - 1);

	double *sortedArr = new double[noOfElems];
	int *displ = new int[numOfProc];
	int *recvcount = new int[numOfProc];

	MPI_Gather(&k, 1, MPI_INT, recvcount, 1, MPI_INT, 0, MPI_COMM_WORLD);

	displ[0] = 0;
	for (i = 1; i < numOfProc; i++)
		displ[i] = displ[i - 1] + recvcount[i - 1];

	double finish2 = MPI_Wtime();
	MPI_Gatherv(finalData, k, MPI_DOUBLE, sortedArr, recvcount, displ, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (rank == 0)
		delete []inputData;
	delete []displ;
	delete []recvcount;
	delete []sortedArr;
	delete []finalData;
	delete []localData;
	delete []allPivots;
	delete []localPivots;
	delete []binSearchPivotIndex;

	for (i = 0; i < numOfProc; i++)
	{
		delete []buckets[i];
	}
	delete []buckets;
	delete []finalBucket;

	double finish1 = MPI_Wtime();

	if (rank == 0)
	{
		cout << "total time elapsed = " << finish1 - start1 << endl;
		cout << "relevant time in middle = " << finish2 - start2 << endl;
	}
	MPI::Finalize();
	return 0;
}
