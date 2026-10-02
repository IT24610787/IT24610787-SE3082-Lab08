// pi_bsend.c - Exercise 7: Exercise 6 (Monte Carlo Pi, MPI_ANY_SOURCE) with Buffered Send
//
// Same as Exercise 6, except the workers send their hit counts with MPI_Bsend instead of
// MPI_Send. MPI_Bsend copies the message into a buffer that the program attaches first
// (MPI_Buffer_attach) and returns at once, without waiting for rank 0 to receive it.
// The buffer must hold every message that can be pending: REPS messages of one long long,
// each needing MPI_BSEND_OVERHEAD extra bytes. It is detached after the loop; the detach
// waits until all buffered messages have been delivered.
// Rank 0 only receives, so it needs no buffer.
//
// Build: mpicc -O2 pi_bsend.c -o pi_bsend -lm      Run: mpirun -np 4 ./pi_bsend
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define REPS 3   // repeat the timed section and average (same as Exercises 3 and 6)

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
    int* order = (int*)malloc((size > 1 ? size - 1 : 1) * sizeof(int));

    // Workers attach a buffer big enough for REPS buffered messages
    void* bsend_buf = NULL;
    int bsend_size = 0;
    if (rank != 0) {
        bsend_size = REPS * (int)(sizeof(long long) + MPI_BSEND_OVERHEAD);
        bsend_buf = malloc(bsend_size);
        MPI_Buffer_attach(bsend_buf, bsend_size);
    }

    for (int r = 0; r < REPS; r++) {
        MPI_Barrier(MPI_COMM_WORLD);
        double t0 = MPI_Wtime();

        unsigned int seed = 12345u + 7919u * (unsigned int)rank;   // same seeds as Exercises 3 and 6
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
                order[k] = status.MPI_SOURCE;
                total_hits += h;
            }
        } else {
            MPI_Bsend(&hits, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);   // the only change from Exercise 6
        }

        double elapsed = MPI_Wtime() - t0;
        double slowest;
        MPI_Reduce(&elapsed, &slowest, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        if (rank == 0) time_sum += slowest;
    }

    if (rank != 0) {
        MPI_Buffer_detach(&bsend_buf, &bsend_size);   // blocks until buffered messages are delivered
        free(bsend_buf);
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