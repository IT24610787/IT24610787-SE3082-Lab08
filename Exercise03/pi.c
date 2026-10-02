// pi.c - Exercise 3: Monte Carlo estimate of Pi using MPI (default 10,000,000 samples in total)
// Random points (x, y) in the unit square; the fraction inside the quarter circle
// x*x + y*y <= 1 approaches Pi/4. Samples are split across the ranks.
// Each worker sends its hit count to rank 0 with MPI_Send; rank 0 receives with MPI_Recv.
// Build: mpicc -O2 pi.c -o pi      Run: mpirun -np 4 ./pi
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define REPS 3   // repeat the timed section and average

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long N = 10000000LL;
    if (argc > 1) N = atoll(argv[1]);

    // Samples handled by this rank (first N % size ranks take one extra)
    long long base  = N / size;
    long long rem   = N % size;
    long long count = base + (rank < rem ? 1 : 0);

    long long total_hits = 0;
    double time_sum = 0.0;

    for (int r = 0; r < REPS; r++) {
        MPI_Barrier(MPI_COMM_WORLD);
        double t0 = MPI_Wtime();

        // A different seed per rank so the ranks do not generate the same points
        unsigned int seed = 12345u + 7919u * (unsigned int)rank;
        long long hits = 0;
        for (long long i = 0; i < count; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0) hits++;
        }

        if (rank == 0) {
            total_hits = hits;
            for (int src = 1; src < size; src++) {
                long long h;
                MPI_Recv(&h, 1, MPI_LONG_LONG, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                total_hits += h;
            }
        } else {
            MPI_Send(&hits, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
        }

        double elapsed = MPI_Wtime() - t0;
        double slowest;
        MPI_Reduce(&elapsed, &slowest, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        if (rank == 0) time_sum += slowest;
    }

    if (rank == 0) {
        double pi = 4.0 * (double)total_hits / (double)N;
        printf("np=%d N=%lld hits=%lld pi=%.6f error=%.6f avg_time=%.6f s\n",
               size, N, total_hits, pi, fabs(pi - M_PI), time_sum / REPS);
    }

    MPI_Finalize();
    return 0;
}