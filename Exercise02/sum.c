// sum.c - Exercise 2: parallel sum of 1..N using MPI (default N = 10,000,000)
// Each rank sums its own block of numbers; MPI_Reduce adds the partial sums on rank 0.
// Build: mpicc -O0 sum.c -o sum      Run: mpirun -np 4 ./sum   (-O0: see run.sh)
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define REPS 5   // repeat the timed section and average, because one run is only a few ms

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long N = 10000000LL;
    if (argc > 1) N = atoll(argv[1]);

    // Block decomposition: the first (N % size) ranks get one extra number
    long long base  = N / size;
    long long rem   = N % size;
    long long count = base + (rank < rem ? 1 : 0);
    long long start = rank * base + (rank < rem ? rank : rem) + 1;   // first number of this block
    long long end   = start + count - 1;

    long long total = 0;
    double time_sum = 0.0;

    for (int r = 0; r < REPS; r++) {
        MPI_Barrier(MPI_COMM_WORLD);          // start all ranks together
        double t0 = MPI_Wtime();

        long long local = 0;
        for (long long i = start; i <= end; i++) local += i;

        MPI_Reduce(&local, &total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

        double elapsed = MPI_Wtime() - t0;
        double slowest;                        // the run is only as fast as the slowest rank
        MPI_Reduce(&elapsed, &slowest, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        if (rank == 0) time_sum += slowest;
    }

    if (rank == 0) {
        long long expected = N * (N + 1) / 2;
        printf("np=%d N=%lld sum=%lld expected=%lld %s avg_time=%.6f s\n",
               size, N, total, expected, (total == expected ? "OK" : "MISMATCH"),
               time_sum / REPS);
    }

    MPI_Finalize();
    return 0;
}