#!/bin/bash
# Exercise 5
mpicxx mismatch.cc -o mismatch || exit 1
mpicxx bsend.cc -o bsend || exit 1

echo "=== Part 1, mode 1: rank 0 sends to rank 2, rank 1 waits for rank 0 ==="
mpirun -np 3 ./mismatch 1
echo
echo "=== Part 1, mode 2: rank 0 sends to rank 1, rank 1 waits for rank 2 ==="
mpirun -np 3 ./mismatch 2
echo
echo "=== Part 1, mode 3: rank 0 sends to rank 5, which does not exist (expect an MPI error) ==="
mpirun -np 2 ./mismatch 3
echo
echo "=== Part 2: message2 rewritten with MPI_Bsend ==="
mpirun -np 2 ./bsend