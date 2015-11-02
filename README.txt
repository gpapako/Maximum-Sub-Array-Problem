

How to run:

- Type 'make' to compile 
- Type 'make run_test' to test
- Change the Makefile to test with different inputs

- To run the serial code:
                 
CC = gcc, CFLAGS = -Wall -g -Wno-error -std=c99,
EXE = max-sub-array-vla, SRC = max-sub-array-vla.c

- To run the c++11 threads code:

CC = g++, CFLAGS = -Wall -g -Wno-error -std=c++11, 
LFLAGS = -lpthread, EXE = max-sub-array-vla-C++11threads,
SRC = max-sub-array-vla-C++11threads.c

- To run the OpenMp code:

CC = gcc, CFLAGS = -Wall -g -Wno-error -fopenmp,
EXE = max-sub-array-vla-OpenMp, SRC = max-sub-array-vla-OpenMp.c