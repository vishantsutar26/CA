#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size < 5) {
        if (world_rank == 0)
            cerr << "Please run with at least 5 processes (1 master + 4 slaves)" << endl;
        MPI_Finalize();
        return 1;
    }

    const int array_size = 100000000;
    vector<double> data(array_size);

    // Start timing before main computation
    double start_time = MPI_Wtime();

    if (world_rank == 0) {
        srand(time(0));
        for (int i = 0; i < array_size; ++i) {
            data[i] = rand() % 100 + 1;
        }

        for (int rank = 1; rank <= 4; ++rank) {
            MPI_Send(data.data(), array_size, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD);
        }

        double results[4];
        for (int rank = 1; rank <= 4; ++rank) {
            MPI_Recv(&results[rank - 1], 1, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        cout << "Results from slaves:" << endl;
        cout << "Addition (rank 1): " << results[0] << endl;
        cout << "Multiplication (rank 2): " << results[1] << endl;
        cout << "Minimum element (rank 3): " << results[2] << endl;
        cout << "Maximum element (rank 4): " << results[3] << endl;

        // End timing after everything is done
        double end_time = MPI_Wtime();
        cout << "Total execution time (master): " << (end_time - start_time) << " seconds" << endl;

    } else if (world_rank >= 1 && world_rank <= 4) {
        MPI_Recv(data.data(), array_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        double result;
        switch(world_rank) {
            case 1:
                result = 0.0;
                for (auto x : data) result += x;
                break;
            case 2:
                result = 1.0;
                for (auto x : data) result *= x;
                break;
            case 3:
                result = numeric_limits<double>::max();
                for (auto x : data) if (x < result) result = x;
                break;
            case 4:
                result = numeric_limits<double>::lowest();
                for (auto x : data) if (x > result) result = x;
                break;
        }

        MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

        // You can also time each slave's local computation if needed:
        
        double end_time = MPI_Wtime();
        cout << "Execution time (slave " << world_rank << "): " << (end_time - start_time) << " seconds" << endl;
       
    }

    MPI_Finalize();
    return 0;
}
