// mismatch.cc - Exercise 5 (part 1): source/destination mismatch, modified from message1.cc
//
// Mode 1 (needs 3 processes): rank 0 sends to rank 2, but rank 1 waits for a message from rank 0.
// Mode 2 (needs 3 processes): rank 0 sends to rank 1, but rank 1 waits for a message from rank 2.
// Mode 3 (needs 2 processes): rank 0 sends to rank 5, which does not exist.
//
// A plain MPI_Recv with no matching send waits forever. To keep the demo from hanging, the
// receiver polls with MPI_Irecv/MPI_Test and gives up after TIMEOUT seconds.
// Add the word "block" as a 2nd argument to use a plain blocking MPI_Recv (it will hang: Ctrl-C).
//
// Build: mpicxx mismatch.cc -o mismatch      Run: mpirun -np 3 ./mismatch 1
#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <cstring>

const double TIMEOUT = 3.0;

// Returns true if a message arrived, false if we gave up waiting.
bool recv_int(int* value, int source, bool block) {
    if (block) {
        MPI_Recv(value, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        return true;
    }
    MPI_Request req;
    MPI_Irecv(value, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &req);
    double t0 = MPI_Wtime();
    int done = 0;
    while (!done && MPI_Wtime() - t0 < TIMEOUT)
        MPI_Test(&req, &done, MPI_STATUS_IGNORE);
    if (!done) {
        MPI_Cancel(&req);
        MPI_Wait(&req, MPI_STATUS_IGNORE);
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int mode = (argc > 1) ? atoi(argv[1]) : 1;
    bool block = (argc > 2 && std::strcmp(argv[2], "block") == 0);

    int need = (mode == 3) ? 2 : 3;
    if (size < need) {
        if (rank == 0) std::cout << "Mode " << mode << " needs at least " << need << " processes\n";
        MPI_Finalize();
        return 1;
    }

    int number;
    int dest   = (mode == 1) ? 2 : (mode == 2) ? 1 : 5;   // where rank 0 sends
    int expect = (mode == 2) ? 2 : 0;                     // who rank 1 listens to

    if (rank == 0) {
        number = 42;
        std::cout << "Process 0 sending " << number << " to rank " << dest << "\n";
        MPI_Send(&number, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        std::cout << "Process 0: MPI_Send returned\n";
    } else if (rank == 1) {
        std::cout << "Process 1 waiting for a message from rank " << expect << "\n";
        if (recv_int(&number, expect, block))
            std::cout << "Process 1 received " << number << "\n";
        else
            std::cout << "Process 1: nothing arrived from rank " << expect
                      << " after " << TIMEOUT << " s - a plain MPI_Recv would block forever\n";
    }

    MPI_Finalize();
    return 0;
}