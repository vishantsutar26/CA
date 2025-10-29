#include <mpi.h>
#include <iostream>
#include <vector>


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


   const int array_size = 5;
   vector<double> data(array_size);


   if (world_rank == 0) {
       // Master initializes the array
       data = {100.0, 5.0, 2.0, 10.0, 4.0};


       // Send the data to each slave
       for (int rank = 1; rank <= 4; ++rank) {
           MPI_Send(data.data(), array_size, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD);
       }


       // Receive results from slaves
       double results[4];
       for (int rank = 1; rank <= 4; ++rank) {
           MPI_Recv(&results[rank - 1], 1, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       }


       // Print results
       cout << "Results from slaves:" << endl;
       cout << "Addition (rank 1): " << results[0] << endl;
       cout << "Subtraction (rank 2): " << results[1] << endl;
       cout << "Multiplication (rank 3): " << results[2] << endl;
       cout << "Division (rank 4): " << results[3] << endl;


   } else if (world_rank >= 1 && world_rank <= 4) {
       // Slaves receive the data
       MPI_Recv(data.data(), array_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


       double result = 0.0;


       switch(world_rank) {
           case 1: // Addition
               result = 0.0;
               for (auto x : data) result += x;
               break;


           case 2: // Subtraction (start from first element)
               result = data[0];
               for (int i = 1; i < array_size; ++i) result -= data[i];
               break;


           case 3: // Multiplication
               result = 1.0;
               for (auto x : data) result *= x;
               break;


           case 4: // Division (start from first element)
               result = data[0];
               for (int i = 1; i < array_size; ++i) {
                   if (data[i] != 0) result /= data[i];
                   else {
                       cerr << "Error: Division by zero on rank 4" << endl;
                       result = 0;  // handle division by zero by setting result to 0
                       break;
                   }
               }
               break;
       }


       // Send result back to master
       MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
   }


   MPI_Finalize();
   return 0;
}
