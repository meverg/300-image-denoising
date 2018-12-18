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

#define PIC_WIDTH 6
#define PIC_HEGIHT 6

using namespace std;

int main(int argc, char** argv) {
	
	int world_rank, world_size, it_num, ppp, slave_count;
  float beta, pi, gamma;

	MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  slave_count = world_size - 1;
  //max_it = 500000/(slave_count*30*30);
  ppp = PIC_HEGIHT * PIC_WIDTH / slave_count;
  beta = argv[argc-2];
  pi = argv[argc-1];
  gamma = 0.5*log((1-pi)/pi);
  printf("beta: %f, pi: %f, gamma: %f", beta, pi, gamma);

  if (world_rank == 0) {
  	int** arr = NULL;
  	arr = (int **)malloc(sizeof(int*) * slave_count);
    for(int i = 0 ; i < slave_count ; i++) {
      arr[i] = (int *)malloc(sizeof(int) * ppp);
    }

  	ifstream in;
    ofstream out;

    in.open("./tmp_input.txt");
    out.open("./tmp_out.txt");

    for (int i = 0 ; i < slave_count ; i++) {
      for (int j = 0 ; j < ppp ; j++) {
        in >> arr[i][j];
      }
    }

    for (int i = 1 ; i <= slave_count ; i++) {
      MPI_Send(arr[(i-1)], ppp, MPI_INT, i, 0, MPI_COMM_WORLD);
    }

    for (int i = 1 ; i <= slave_count ; i++) {
      MPI_Recv(arr[(i-1)], ppp, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    int WIDTH_COUNT = 0;
    for (int i = 0 ; i < slave_count ; i++) {
      for (int j = 0 ; j < ppp ; j++) {
        out << arr[i][j];
        out << " ";
        WIDTH_COUNT++;
      }
      if (WIDTH_COUNT == PIC_WIDTH) {
        out << "\n";
        WIDTH_COUNT = 0;
      }
    }

  } else {
    printf("this is process %d \n", world_rank);
    int* subarr = NULL;
    subarr = (int *)malloc(sizeof(int) * ppp);
    MPI_Recv(subarr, ppp, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Process %d received elements: ", world_rank);
    for(int i = 0 ; i < ppp ; i++) {
      printf("%d ", subarr[i]);
    }
    printf("\n");

    int** 2d_subarr = NULL;
    2d_subarr = (int **)malloc(sizeof(int*) * (2 + PIC_HEGIHT/slave_count));
    for(int i = 0 ; i < slave_count ; i++) {
      2d_subarr[i] = (int *)malloc(sizeof(int) * PIC_WIDTH);
    }

    int WIDTH_COUNT = 0;
    int HEIGHT_COUNT = 1;

    for(int i = 0 ; i < ppp ; i++) {
      2d_subarr[HEIGHT_COUNT][WIDTH_COUNT] = subarr[i];
      WIDTH_COUNT++;
      if (WIDTH_COUNT = PIC_WIDTH) {
        WIDTH_COUNT = 0;
        HEIGHT_COUNT++;
      }
    }

    int* topnei = NULL;
    topnei = (int *)malloc(sizeof(int) * PIC_WIDTH);
    int* botnei = NULL;
    botnei = (int *)malloc(sizeof(int) * PIC_WIDTH);

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

      printf("topnei for %d : \n", world_rank);
      for(int i = 0 ; i < PIC_WIDTH ; i++)
        printf("%d ", topnei[i]);
      printf("\n");

      printf("botnei for %d : \n", world_rank);
      for(int i = 0 ; i < PIC_WIDTH ; i++)
        printf("%d ", botnei[i]);
      printf("\n");

      for(int i = 0 ; i < PIC_WIDTH ; i++) {
        2d_subarr[0][i] = topnei[i];
      }

      for(int i = 0 ; i < PIC_WIDTH ; i++) {
        2d_subarr[(1 + PIC_HEGIHT/slave_count)][i] = botnei[i];
      }

      // MAIN TASK

    
    } 
    
    MPI_Send(subarr, ppp, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();

}