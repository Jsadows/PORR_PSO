cmake_minimum_required(VERSION 3.22)
project(PSO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(OpenMP)

add_executable(PSO
        main.cpp
        Lib/Tasks.h
        Lib/Tasks.cpp "Lib/Pso.h" "Lib/Pso.cpp")

if(OpenMP_CXX_FOUND)
    target_link_libraries(PSO PUBLIC OpenMP::OpenMP_CXX)
endif()

#add_subdirectory(Lib)
