#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define message tags to distinguish commands
#define GO_TAG 1
#define STOP_TAG 2
#define DATA_TAG 3

long long calculate_hits(int num_points) {
    long long local_hits = 0;
    // Seed the random number generator differently for each process
    unsigned int seed = time(NULL) ^ getpid();
    for (int i = 0; i < num_points; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if (x * x + y * y <= 1.0) {
            local_hits++;
        }
    }
    return local_hits;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank != 0) { // Worker Code
        MPI_Status status;
        const int POINTS_PER_ROUND = 1000000;

        while (1) {
            // 1. Wait for a signal from the coordinator
            MPI_Recv(NULL, 0, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // 2. Check if it's the STOP signal
            if (status.MPI_TAG == STOP_TAG) {
                break; // Exit the loop and terminate
            }

            // 3. It must be a GO signal, so do the work
            long long local_hits = calculate_hits(POINTS_PER_ROUND);

            // 4. Send the result back to the coordinator
            MPI_Send(&local_hits, 1, MPI_LONG_LONG, 0, DATA_TAG, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    return 0;
}