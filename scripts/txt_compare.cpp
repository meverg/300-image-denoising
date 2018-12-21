#include <stdio.h>
#include <stdlib.h>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <math.h>
#include <unistd.h>
#include <algorithm>

#define PIC_WIDTH 200
#define PIC_HEGIHT 200

using namespace std;

int main(int argc, char** argv) {
  ifstream in1;
  ifstream in2;

  in1.open(argv[argc - 1]);
  in2.open(argv[argc - 2]);

  int diff_count = 0;
  int tmp1 = 0;
  int tmp2 = 0;

  for (int i = 0; i < PIC_HEGIHT ; i++) {
  	for(int j = 0; j < PIC_WIDTH; j++) {
  		in1 >> tmp1;
  		in2 >> tmp2;
  		if (tmp2 != tmp1) {
  			diff_count++;
  		}
  	}
  }
  printf("diff_count: %d\n", diff_count);
}