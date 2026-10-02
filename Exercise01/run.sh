#!/bin/bash
# Exercise 0/1 - lecture MPI activities
mpicc HelloMPI.c -o HelloMPI        # C file -> mpicc
mpicxx message1.cc -o message1      # C++ files -> mpicxx
mpicxx message2.cc -o message2

mpirun -np 4 ./HelloMPI
mpirun -np 2 ./message1
mpirun -np 2 ./message2