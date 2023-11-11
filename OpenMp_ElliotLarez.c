#include <stdio.h>
#include <omp.h>
#include <time.h>
#include "time.h"
#include <stdlib.h>

#define FactorIntToDouble 1.1; 


// Size of square Matrix
int N = 0;

// Number of threads
int threads = 0;

// First Matrix
double** A = NULL;

// Second Matrix
double** B = NULL;

// Result Matrix of A * B
double** C = NULL;



//Calculates the partial product of A, B and stores it partial C, defined by the thread running
void matrixMultiplication()
{
	if (threads != omp_get_num_threads())
		exit(-1);

	int thread_rank = omp_get_thread_num();
	
	printf("\tThread: %d of %d started\n", thread_rank, threads);
	for (int i = thread_rank ; i < N ; i = i + threads)
		for (int k = 0 ; k < N ; k++)
			for (int j = 0; j < N ; j++)
				C[i][j] += A[i][k] * B[k][j] ;
	printf("\tThread: %d of %d ended\n", thread_rank, threads);
}


//Calculates the partial product of A, B and stores it partial C, defined by the thread running using block otimization
void matrixMultiplication_block(int block_size)
{
	
	if(block_size == N){
		matrixMultiplication();
		return;
	}

	if (threads != omp_get_num_threads())
		exit(-1);

	int thread_rank = omp_get_thread_num();
	
	printf("\tThread: %d of %d started\n", thread_rank, threads);
	for (int i2 =0; i2 < N; i2 += block_size)
		for (int j2 =0; j2 < N; j2 += block_size)
			for (int k2 =0; k2 < N; k2 += block_size){
				for (int i = i2 + thread_rank; i < i2 + block_size && i < N; i = i+threads)
					for (int j = j2; j < j2 + block_size && j2 < N; j++)
						for (int k = k2 ; k < k2 + block_size && k < N ; k++)		
							C[i][j] += A[i][k] * B[k][j] ;
			}
	
	printf("\tThread: %d of %d ended\n", thread_rank, threads);
}


// Fills A, B with random values between [0-9]
void matrixInit()
{
	A = (double**)malloc(N * sizeof(double *));
	B = (double**)malloc(N * sizeof(double *));
	C = (double**)malloc(N * sizeof(double *));

	for (int i = 0 ; i < N ; i++ ){
		A[i] = (double*)malloc(N * sizeof(double));
		B[i] = (double*)malloc(N * sizeof(double));
		C[i] = (double*)malloc(N * sizeof(double));
		for (int j = 0 ; j < N ; j++){
			srand(i * N + j);
			A[i][j] = (rand() % 10) * FactorIntToDouble;
			B[i][j] = (rand() % 10) * FactorIntToDouble;
			C[i][j] = 0;
		}
	}
}


int main(int argc, char *argv[])
{
	
	N = atoi(argv[1]);
	threads = atoi(argv[2]);
	int block_size = atoi(argv[3]);	
	printf("Matrix Dimentions N*N: %d*%d\n", N, N);
	printf("Number of threads = %d\n", threads);
	printf("Block Size = %d\n", block_size);
	
	matrixInit();

	double t1 = omp_get_wtime();
	
	#pragma omp parallel num_threads(threads)
	matrixMultiplication_block(block_size);

	double t2 = omp_get_wtime();
	
	
	printf("Time in seconds: %lf\n\n\n", t2-t1);
	return 0;
}



