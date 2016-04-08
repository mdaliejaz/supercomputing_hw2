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

int binSearch(double *arr,int value, int left, int right,int size)
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
        return left+1;
}




void printArray(double A[], int len)
{
	cout<<"Printing array now  :"<<endl;
	for(uint64_t i=0 ; i<len; i++ )
    {
            cout << A[i] << "  " ;
    }
    cout <<endl<<endl;
}

int main( int argc, char *argv[ ] )
{

	int numOfProc,rank;

    MPI::Init ( argc, argv );
    numOfProc = MPI::COMM_WORLD.Get_size ( );
	double start1 = MPI_Wtime();
    rank = MPI::COMM_WORLD.Get_rank ( );
  //  cout<<"Number of process= " <<numOfProc<<" and rank = "<<rank<<endl;
	double *inputData, *localData;
	int localDataLen ;
	int i,j;
	noOfElems = atoi(argv[1]);

	if (noOfElems %5 != 0)
	{
		cout<<"no is not multiple of 5"<<endl;
		return 1;
	}

    if (rank == 0)
	{
		inputData = new double[noOfElems];

		for(i = 1; i <= noOfElems;i++)
			inputData[i-1] = (double)i;

		for(i=0;i < noOfElems;i++)
		{
			uint64_t x = rand() % noOfElems;
                	uint64_t y = rand() % noOfElems;
                	double temp =  inputData[x];
                	inputData[x] = inputData[y];
                	inputData[y] = temp;
		}

//		printArray(inputData, noOfElems);

	}

	localDataLen =  noOfElems/numOfProc;
	localData = new double[localDataLen];

	MPI_Scatter(inputData, localDataLen, MPI_DOUBLE, localData, localDataLen, MPI_DOUBLE,0,MPI_COMM_WORLD);
	        double start2 = MPI_Wtime();


	//cout<<"scattering done . I am process " <<rank << " and my data is " <<endl;
	//printArray(localData,localDataLen);

	//sort(localData,localData + localDataLen);

	cilk_sm_sort(localData,0,localDataLen-1);

//	cout<<"sorting done. my data is "<<endl;
//	printArray(localData, localDataLen);

	double* localPivots;
	int localPivotLen = numOfProc - 1;
	localPivots = new double[localPivotLen];

	//for(i = 0; i < numOfProc-1;i++)
	//	localPivots[i] = localData[ noOfElems/(numOfProc*numOfProc) * (i+1)];

	for(i=1;i<numOfProc;i++)
		localPivots[i-1] = localData[(i*localDataLen)/numOfProc];


	//cout<<" pivot selection done. My rank is "<< rank <<" and pivot is "<<endl;
	//printArray(localPivots,localPivotLen);


	double* allPivots;
	int allPivotsLen = (numOfProc - 1)*numOfProc;
	allPivots = new double[allPivotsLen];

	MPI_Gather(localPivots,localPivotLen,MPI_DOUBLE,allPivots,localPivotLen,MPI_DOUBLE,0,MPI_COMM_WORLD);

	//cout<<" gathered all pivots. "<<endl;

	if (rank == 0){
//		printArray(allPivots,allPivotsLen);
		sort(allPivots, allPivots + allPivotsLen);
//		cout<<" sorted pivots are finally"<<endl;

//		printArray(allPivots, allPivotsLen);

		for(i = 0; i< localPivotLen;i++)
			localPivots[i] = allPivots[localPivotLen*(i+1)];
	

//		cout<<"selected the pivots for everyone"<<endl;
//		printArray(localPivots, localPivotLen);

	}

	delete []allPivots;
	MPI_Bcast(localPivots,localPivotLen, MPI_DOUBLE,0,MPI_COMM_WORLD);

	//cout<<"finally got pivots from master . they are  "<<endl;
	//printArray(localPivots,localPivotLen);


	double **buckets = new double*[numOfProc]; //for rows
	for(i=0;i< numOfProc;i++)
		buckets[i] = new double[localDataLen];

	double*  binSearchPivotIndex = new double[localPivotLen];

	for(i = 0;i<localPivotLen;i++)
		binSearchPivotIndex[i] = binSearch(localData,localPivots[i],0,localDataLen-1,localDataLen);
		
	delete []localPivots;

//	cout<<" binary search indices = "<<endl;
//	printArray(binSearchPivotIndex, localPivotLen);

	int prev , currIndex=0;
	for(i=0;i<localPivotLen;i++)
	{
		prev = i==0? 0 : binSearchPivotIndex[i-1];
		buckets[i][0] = binSearchPivotIndex[i] - prev; //buckets[i][0] is count of no of elements in this bucket
		for(j = 1;j <= buckets[i][0];j++)
		{
			buckets[i][j] = localData[currIndex];
			currIndex++;
		}

	}


	buckets[numOfProc-1][0] = localDataLen - binSearchPivotIndex[localPivotLen - 1];
	for(i=1;i<= buckets[numOfProc-1][0];i++)
		buckets[numOfProc-1][i] = localData[currIndex++];

	delete []localData;
	delete []binSearchPivotIndex;
	/*if (rank == 2) {
		cout<<" first bucket " <<endl;
		printArray(buckets[0], buckets[0][0]+1);
		cout<<" second bucket " <<endl;
		printArray(buckets[1], buckets[1][0]+1);
		cout<<" third bucket " <<endl;
		printArray(buckets[2], buckets[2][0]+1);
	}*/


	//double* finalBucket = new double[numOfProc*localDataLen];

	/*double **finalBucket = new double*[numOfProc]; //for rows
	for(i=0;i< numOfProc;i++)
		finalBucket[i] = new double[numOfProc*localDataLen];
	*/
	double *finalBucket = new double[numOfProc*localDataLen];
	//double finalBucket[numOfProc][localDataLen];


	/*double* currBucket = new double[numOfProc*localDataLen];
	for(i=0;i<numOfProc;i++)
		for(j=0;j<localDataLen;j++)
			currBucket[i*localDataLen + j] = buckets[i][j];*/

//	cout<<"before gather -----------"<<endl;
	//MPI_Barrier(MPI_COMM_WORLD);

	for(i=0;i<numOfProc;i++)
		//MPI_Alltoall(buckets[i],localDataLen,MPI_DOUBLE,finalBucket[i],localDataLen,MPI_DOUBLE,MPI_COMM_WORLD);
		//MPI_Gather(buckets[i],localDataLen,MPI_DOUBLE,finalBucket[i],localDataLen,MPI_DOUBLE,i,MPI_COMM_WORLD);
		MPI_Gather(buckets[i],localDataLen,MPI_DOUBLE,finalBucket,localDataLen,MPI_DOUBLE,i,MPI_COMM_WORLD);

	for(i = 0;i < numOfProc;i++)
	{
		delete []buckets[i];
		//delete []finalBucket[i];

	}

	delete []buckets;

	//MPI_Alltoall(currBucket,localDataLen,MPI_DOUBLE,finalBucket,localDataLen,MPI_DOUBLE,MPI_COMM_WORLD);

//	cout<<"afdter ---------------"<<endl;
	if (rank == 0 ){
		//cout<<" finalllllllllllllllll for rank 0"<<endl;
		//printArray(finalBucket[0], numOfProc*localDataLen);
		//printArray(finalBucket[0], numOfProc*localDataLen);
		//printArray(finalBucket[1], numOfProc*localDataLen);
		//printArray(finalBucket[2], numOfProc*localDataLen);
	}

	//delete []currBucket;
	//delete []finalBucket;
	int k=0;

	double* finalData = new double[noOfElems];

	for(i=0;i<numOfProc*localDataLen;i = i+localDataLen)
	{
		for(j=1;j<= finalBucket[i];j++)
			finalData[k++] = finalBucket[j+i];

	}

	delete []finalBucket;
	//sort(finalData,finalData + k);

	cilk_sm_sort(finalData,0,k-1);

	if (rank == 0) {
//		cout<<" k = "<<k<<endl;
//		printArray(finalData,k);

	}

	double *sortedArr = new double[noOfElems];
	//const int displ[3] = {0,9,16};
	int *displ = new int[numOfProc];
	//const int recvcount[3] = {9,7,8};

	int *recvcount = new int[numOfProc];

	MPI_Gather(&k,1,MPI_INT,recvcount,1,MPI_INT,0,MPI_COMM_WORLD);

	displ[0] = 0;
	for(i=1;i<numOfProc;i++)
		displ[i] = displ[i-1] + recvcount[i-1];

	/*displ[0] = 0;
	displ[1] = 9;
	displ[2] = 16;
	displ[0] = 0;
	for(i=1;i<numOfProc;i++)
		displ[i] = displ[i-1]+k;*/

	double finish2 = MPI_Wtime();
	MPI_Gatherv(finalData,k,MPI_DOUBLE,sortedArr,recvcount,displ,MPI_DOUBLE,0,MPI_COMM_WORLD);
	delete []displ;
	delete []recvcount;
	delete []finalData;
	delete []sortedArr;
	if(rank == 0)
	{
//		cout<<" recv array = "<<recvcount[0]<<recvcount[1]<<recvcount[2]<<endl;

//		cout<<"----------------- all data ===================="<<endl;
//		printArray(sortedArr, noOfElems);

//		cout<<"-------sorting printed==="<<endl;
	}
	if (rank == 0)
		delete []inputData;

//	cout<<"delete 1"<<endl;
	


	//delete []localData;
	//delete []allPivots;



	//cout<<"delete 2"<<endl;

	double finish1 = MPI_Wtime();
	//cout<<"delete 4"<<endl;

	if (rank == 0)
	{
		cout<<"total time elapsed = "<<finish1 - start1<<endl;
		cout<<"relevant time in middle = "<<finish2-start2<<endl;
	}
	MPI::Finalize();
//	cout<<"Delete 5"<<endl;
	return 0;
}
