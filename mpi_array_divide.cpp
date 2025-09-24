#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <iomanip>  // for setprecision

using namespace std;

struct Result {
    double sum;
    double product;
    double minVal;
    double maxVal;
};

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int ARRAY_SIZE = 10000;
    const int PARTS = 4;
    const int PART_SIZE = ARRAY_SIZE / PARTS;

    // Define MPI datatype for Result struct
    MPI_Datatype MPI_RESULT;
    MPI_Type_contiguous(4, MPI_DOUBLE, &MPI_RESULT);
    MPI_Type_commit(&MPI_RESULT);

    vector<double> data;

    // Master process
    if (rank == 0) {
        data.resize(ARRAY_SIZE);

        // Fill array with random values
        srand(time(0));
        for (int i = 0; i < ARRAY_SIZE; ++i)
            data[i] = rand() % 100 + 1;

        // Start timer
        double start_time = MPI_Wtime();

        // Send 4 parts to ranks 1-4
        for (int i = 1; i <= PARTS; ++i) {
            MPI_Send(&data[(i - 1) * PART_SIZE], PART_SIZE, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }

        // Receive partial results from slaves
        vector<Result> results(PARTS);
        for (int i = 1; i <= PARTS; ++i) {
            MPI_Recv(&results[i - 1], 1, MPI_RESULT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Combine results
        double total_sum = 0.0;
        double total_product = 1.0;
        double total_min = numeric_limits<double>::max();
        double total_max = numeric_limits<double>::lowest();

        for (const auto& r : results) {
            total_sum += r.sum;
            total_product *= r.product;
            if (r.minVal < total_min) total_min = r.minVal;
            if (r.maxVal > total_max) total_max = r.maxVal;
        }

        double end_time = MPI_Wtime();

        // Print results
        cout << fixed << setprecision(8);
        cout << "Total Sum: " << total_sum << endl;
        cout << "Total Product: " << total_product << endl;
        cout << "Minimum Value: " << total_min << endl;
        cout << "Maximum Value: " << total_max << endl;
        cout << "Total Execution Time: " << (end_time - start_time) << " seconds" << endl;

    } else if (rank >= 1 && rank <= PARTS) {
        vector<double> sub_array(PART_SIZE);
        MPI_Recv(sub_array.data(), PART_SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        Result result;
        result.sum = 0.0;
        result.product = 1.0;
        result.minVal = numeric_limits<double>::max();
        result.maxVal = numeric_limits<double>::lowest();

        for (auto x : sub_array) {
            result.sum += x;
            result.product *= x;
            if (x < result.minVal) result.minVal = x;
            if (x > result.maxVal) result.maxVal = x;
        }

        MPI_Send(&result, 1, MPI_RESULT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Type_free(&MPI_RESULT);
    MPI_Finalize();
    return 0;
}
