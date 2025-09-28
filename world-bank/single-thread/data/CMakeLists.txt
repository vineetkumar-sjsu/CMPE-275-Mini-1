cmake_minimum_required(VERSION 3.10)
project(PopulationDataAnalysis)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Try to use GCC if available
find_program(GCC_PATH g++-11)
if(GCC_PATH)
    set(CMAKE_CXX_COMPILER ${GCC_PATH})
    message(STATUS "Using GCC: ${GCC_PATH}")
else()
    find_program(GCC_PATH g++)
    if(GCC_PATH)
        set(CMAKE_CXX_COMPILER ${GCC_PATH})
        message(STATUS "Using GCC: ${GCC_PATH}")
    endif()
endif()

# Set compiler flags for optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")

# Add include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/PopulationData.cpp
)

# Header files
set(HEADERS
    include/PopulationData.h
)

# Create executable
add_executable(population_analysis ${SOURCES} ${HEADERS})

# Set output directory
set_target_properties(population_analysis PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)


