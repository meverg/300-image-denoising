// Author: Emin Vergili
// Date: 05.12.2018
// CmpE300 Programming Project
//
//
//
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <math.h>
#include <unistd.h>
#include <algorithm>
#include <ctime>

#define PIC_WIDTH 200
#define PIC_HEGIHT 200

using namespace std;

int main(int argc, char** argv) {
	
	int world_rank, world_size, it_num, ppp, slave_count, max_it, sync;
  float beta, pi;
  double gamma;

	MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  slave_count = world_size - 1;
  max_it = 3000000;
  ppp = PIC_HEGIHT * PIC_WIDTH / slave_count;
  beta = atof(argv[argc-2]);
  pi = atof(argv[argc-1]);
  gamma = 0.5*log((1-pi)/pi);

  //sync = 1;
  srand(time(0));

  if (world_rank == 0) {
  	int** arr = NULL;
  	arr = (int **)malloc(sizeof(int*) * slave_count);
    for(int i = 0 ; i < slave_count ; i++) {
      arr[i] = (int *)malloc(sizeof(int) * ppp);
    }

  	ifstream in;
    ofstream out;

    out.open(argv[argc - 3]);
    in.open(argv[argc - 4]);

    for (int i = 0 ; i < slave_count ; i++) {
      for (int j = 0 ; j < ppp ; j++) {
        in >> arr[i][j];
      }
    }

    for (int i = 1 ; i <= slave_count ; i++) {
      MPI_Send(arr[(i-1)], ppp, MPI_INT, i, 0, MPI_COMM_WORLD);
    }

    int** out_arr = NULL;
    out_arr = (int **)malloc(sizeof(int*) * slave_count);
    for(int i = 0 ; i < slave_count ; i++) {
      out_arr[i] = (int *)malloc(sizeof(int) * ppp);
    }

    for (int i = 1 ; i <= slave_count ; i++) {
      MPI_Recv(out_arr[(i-1)], ppp, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    int WIDTH_COUNT = 0;
    int diff_count = 0;
    for (int i = 0 ; i < slave_count ; i++) {
      for (int j = 0 ; j < ppp ; j++) {
        if(out_arr[i][j] != arr[i][j]){
          diff_count++;
        }
        out << out_arr[i][j];
        out << " ";
        WIDTH_COUNT++;
        if (WIDTH_COUNT == PIC_WIDTH) {
          out << "\n";
          WIDTH_COUNT = 0;
        }
      }
    }
    printf("beta: %f, pi: %f, gamma: %f\n", beta, pi, gamma);
    printf("diff_count: %d\n", diff_count);

  } else {
    printf("this is process %d \n", world_rank);
    int* subarr = NULL;
    subarr = (int *)malloc(sizeof(int) * ppp);
    MPI_Recv(subarr, ppp, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    /*printf("Process %d received elements: ", world_rank);
    for(int i = 0 ; i < ppp ; i++) {
      printf("%d ", subarr[i]);
    }
    printf("\n");*/

    int** two_d_subarr = NULL;
    int** two_d_subarr_curr = NULL;
    two_d_subarr = (int **)malloc(sizeof(int*) * (2 + PIC_HEGIHT/slave_count));
    two_d_subarr_curr = (int **)malloc(sizeof(int*) * (2 + PIC_HEGIHT/slave_count));
    for(int i = 0 ; i < (2 + PIC_HEGIHT/slave_count) ; i++) {
      two_d_subarr[i] = (int *)malloc(sizeof(int) * PIC_WIDTH);
      two_d_subarr_curr[i] = (int *)malloc(sizeof(int) * PIC_WIDTH);
    }

    int WIDTH_COUNT = 0;
    int HEIGHT_COUNT = 1;

    for(int i = 0 ; i < ppp ; i++) {
      two_d_subarr[HEIGHT_COUNT][WIDTH_COUNT] = subarr[i];
      two_d_subarr_curr[HEIGHT_COUNT][WIDTH_COUNT] = subarr[i];
      WIDTH_COUNT++;
      if (WIDTH_COUNT == PIC_WIDTH) {
        WIDTH_COUNT = 0;
        HEIGHT_COUNT++;
      }
    }

    int* topnei = NULL;
    topnei = (int *)calloc(PIC_WIDTH, sizeof(int));
    int* botnei = NULL;
    botnei = (int *)calloc(PIC_WIDTH, sizeof(int));

    int accept_count = 0;
    for (int count = 0 ; count < max_it ; count++) {
      
      // EXCHANGE NEIGHBOUR INFO
      if (world_rank % 2 == 1) {
        if (world_rank != slave_count) {
          MPI_Send(&subarr[(ppp-PIC_WIDTH)], PIC_WIDTH, MPI_INT, (world_rank+1), 0, MPI_COMM_WORLD);
        }
        if (world_rank != 1) {
          MPI_Recv(topnei, PIC_WIDTH, MPI_INT, (world_rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          MPI_Send(&subarr[0], PIC_WIDTH, MPI_INT, (world_rank-1), 0, MPI_COMM_WORLD);
        }
        if (world_rank != slave_count) {
          MPI_Recv(botnei, PIC_WIDTH, MPI_INT, (world_rank+1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
      } else {
        MPI_Recv(topnei, PIC_WIDTH, MPI_INT, (world_rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (world_rank != slave_count) {
          MPI_Send(&subarr[(ppp-PIC_WIDTH)], PIC_WIDTH, MPI_INT, (world_rank+1), 0, MPI_COMM_WORLD);
          MPI_Recv(botnei, PIC_WIDTH, MPI_INT, (world_rank+1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        MPI_Send(&subarr[0], PIC_WIDTH, MPI_INT, (world_rank-1), 0, MPI_COMM_WORLD);
      }
      // check for nei lines
      /*printf("topnei for %d : \n", world_rank);
      for(int i = 0 ; i < PIC_WIDTH ; i++)
        printf("%d ", topnei[i]);
      printf("\n");

      printf("botnei for %d : \n", world_rank);
      for(int i = 0 ; i < PIC_WIDTH ; i++)
        printf("%d ", botnei[i]);
      printf("\n");*/

      for(int i = 0 ; i < PIC_WIDTH ; i++) {
        if (count == 0) {
          two_d_subarr[0][i] = topnei[i];
        }
        two_d_subarr_curr[0][i] = topnei[i];
      }

      for(int i = 0 ; i < PIC_WIDTH ; i++) {
        if (count == 0) {
          two_d_subarr[(1 + PIC_HEGIHT/slave_count)][i] = botnei[i];
        }
        two_d_subarr_curr[(1 + PIC_HEGIHT/slave_count)][i] = botnei[i];
      }

      //sleep(world_rank*1);

      // check if two_d_subarr is forming up right
      /*printf("two_d_subarr for %d : \n", world_rank);
      for(int i = 0 ; i < (2 + PIC_HEGIHT/slave_count) ; i++) {
        for(int j = 0 ; j < PIC_WIDTH ; j++) {
          printf("%d ", two_d_subarr[i][j]);
        }
        printf("\n");
      }*/

      // MAIN TASK
      // choose random pixel
      //srand(time(0));
      int row = rand() % (PIC_HEGIHT/slave_count) + 1;
      int col = rand() % PIC_WIDTH;
      //printf(" row/col: %d/%d \n", row, col);

      // calculate acceptance probability
      float accept;
      int nei_sum = -two_d_subarr_curr[row][col];
      for(int i = row-1 ; i <= row+1; i++) {
        for (int j = col-1 ; j <= col+1; j++) {
          //printf(" nei_sum is now: %d ", nei_sum);
          nei_sum += two_d_subarr_curr[i][j];
        }
      }
      //printf(" nei_sum is now: %d ", nei_sum);
      accept = exp(- (2 * gamma * two_d_subarr[row][col] * two_d_subarr_curr[row][col]) - (2 * beta * two_d_subarr_curr[row][col] * nei_sum));
      // printf("\n %f %f \n", - (2 * gamma * two_d_subarr[row][col] * two_d_subarr_curr[row][col]), - (2 * beta * two_d_subarr_curr[row][col] * nei_sum));
      accept = min(accept, (float)1);
      //srand(time(0));
      float new_rand = ((float)rand())/RAND_MAX;
      //printf(" accept is now: %f/%f \n", accept, new_rand);
      if (accept > new_rand ) {
        two_d_subarr_curr[row][col] = -two_d_subarr_curr[row][col];
        subarr[(row-1) * PIC_WIDTH + col] = -subarr[(row-1) * PIC_WIDTH + col];
        //printf(" accepted \n");
        accept_count++;
      }
      // printf(" progress: %d/%d ", count, max_it);
      // printf("\n");
    } 
    
    printf("accept_count: %d\n", accept_count);
    MPI_Send(subarr, ppp, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();

}