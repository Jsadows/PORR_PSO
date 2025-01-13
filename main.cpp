#include "mpi.h"   
#include <iostream>
#include "Lib/Tasks.h"
#include "Lib/Pso_mpi.h"
#include "Lib/Testing.h"

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv); 
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        std::cout << "Running on " << size << " ranks.\n";
    }
    // testing_run(); 
    x5_best_test_run();

    MPI_Finalize(); 
    return 0;
}
