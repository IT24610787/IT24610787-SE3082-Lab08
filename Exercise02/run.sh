#!/bin/bash
# Exercise 2 - parallel sum, run with 1, 2, 4, 8 processes
# -O0 on purpose: with -O2 the compiler can replace the summing loop with a closed-form
# formula, which makes the timings meaningless (the loop is not actually executed).
mpicc -O0 sum.c -o sum || exit 1
for np in 1 2 4 8; do
    mpirun -np $np ./sum
done