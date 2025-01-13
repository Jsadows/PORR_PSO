cmake_minimum_required(VERSION 3.22)
project(PSO)

set(CMAKE_CXX_STANDARD 17)


find_package(MPI REQUIRED)

add_executable(PSO
    main.cpp
    Lib/Tasks.h
    Lib/Tasks.cpp
    Lib/Pso_mpi.h
    Lib/Pso_mpi.cpp
    Lib/Testing.cpp
    Lib/Testing.h
)

target_link_libraries(PSO PRIVATE MPI::MPI_CXX)
#add_subdirectory(Lib)

