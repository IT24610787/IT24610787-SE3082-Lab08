#!/bin/bash
# Exercise 7 - Monte Carlo Pi with MPI_ANY_SOURCE and MPI_Bsend, run with 1, 2, 4, 8 processes
mpicc -O2 pi_bsend.c -o pi_bsend -lm || exit 1
for np in 1 2 4 8; do
    mpirun -np $np ./pi_bsend
done