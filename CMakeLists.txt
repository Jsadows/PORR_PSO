cmake_minimum_required(VERSION 3.22)
project(PSO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(PSO
        main.cpp
        Lib/Tasks.h
        Lib/Tasks.cpp "Lib/Pso.h" "Lib/Pso.cpp")

#add_subdirectory(Lib)
