#!/bin/bash
# Exercise 6 - Monte Carlo Pi with MPI_ANY_SOURCE, run with 1, 2, 4, 8 processes
mpicc -O2 pi_any_source.c -o pi_any_source -lm || exit 1
for np in 1 2 4 8; do
    mpirun -np $np ./pi_any_source
done