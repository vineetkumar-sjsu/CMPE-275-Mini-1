cmake_minimum_required(VERSION 3.10)
project(ParallelPopulationAnalysis)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force use of g++
set(CMAKE_CXX_COMPILER /usr/bin/g++)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Include directories
include_directories(include)

# Add executable
add_executable(population_analysis
    src/main.cpp
    src/PopulationData.cpp
)

# Set output directory
set_target_properties(population_analysis PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Print configuration info
message(STATUS "OpenMP found: ${OpenMP_CXX_FOUND}")
message(STATUS "OpenMP version: ${OpenMP_CXX_VERSION}")
message(STATUS "OpenMP flags: ${OpenMP_CXX_FLAGS}")

# Create data directory in build
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data)
file(COPY data/ DESTINATION ${CMAKE_BINARY_DIR}/data)
