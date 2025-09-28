cmake_minimum_required(VERSION 3.10)
project(PopulationDataAnalysis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(population_analysis
    src/main.cpp
    src/PopulationData.cpp
    include/PopulationData.h
)

set_target_properties(population_analysis PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
