// pi_any_source.c - Exercise 6: Exercise 3 (Monte Carlo Pi) with MPI_ANY_SOURCE on the receive
//
// Same as Exercise 3 except for rank 0's receive. Exercise 3 receives from rank 1, 2, 3, ...
// in a fixed order. Here rank 0 receives with MPI_ANY_SOURCE, so it takes whichever worker
// finishes first, and reads the sender from status.MPI_SOURCE.
//
// Build: mpicc -O2 pi_any_source.c -o pi_any_source -lm      Run: mpirun -np 4 ./pi_any_source
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define REPS 3   // repeat the timed section and average (same as Exercise 3)

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long N = 10000000LL;
    if (argc > 1) N = atoll(argv[1]);

    long long base  = N / size;
    long long rem   = N % size;
    long long count = base + (rank < rem ? 1 : 0);

    long long total_hits = 0;
    double time_sum = 0.0;
    int* order = (int*)malloc((size > 1 ? size - 1 : 1) * sizeof(int));   // arrival order (last rep)

    for (int r = 0; r < REPS; r++) {
        MPI_Barrier(MPI_COMM_WORLD);
        double t0 = MPI_Wtime();

        unsigned int seed = 12345u + 7919u * (unsigned int)rank;   // same seeds as Exercise 3
        long long hits = 0;
        for (long long i = 0; i < count; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0) hits++;
        }

        if (rank == 0) {
            total_hits = hits;
            for (int k = 0; k < size - 1; k++) {
                long long h;
                MPI_Status status;
                MPI_Recv(&h, 1, MPI_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
                order[k] = status.MPI_SOURCE;       // who actually sent this one
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
        printf("np=%d N=%lld hits=%lld pi=%.6f error=%.6f avg_time=%.6f s order=",
               size, N, total_hits, pi, fabs(pi - M_PI), time_sum / REPS);
        if (size == 1) printf("-");
        for (int k = 0; k < size - 1; k++) printf(k ? ",%d" : "%d", order[k]);
        printf("\n");
    }

    free(order);
    MPI_Finalize();
    return 0;
}