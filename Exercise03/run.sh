#!/bin/bash
# Exercise 3 - Monte Carlo Pi, run with 1, 2, 4, 8 processes
mpicc -O2 pi.c -o pi -lm || exit 1
for np in 1 2 4 8; do
    mpirun -np $np ./pi
done