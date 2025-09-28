cmake_minimum_required(VERSION 3.3)

project (fire-data-analysis)

# Set C++17 standard for filesystem support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# compilers to use (9.2) - commented out to use default system compilers
# set(CMAKE_C_COMPILER /opt/homebrew/Cellar/gcc/11.2.0_3/bin/gcc-11)
# set(CMAKE_CXX_COMPILER  /opt/homebrew/Cellar/gcc/11.2.0_3/bin/g++-11)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules)

# Find OpenMP - handle Apple Clang specifically
find_package(OpenMP)
if(NOT OpenMP_CXX_FOUND)
    # For Apple Clang, manually set OpenMP paths
    if(APPLE)
        set(OpenMP_CXX_FLAGS "-Xpreprocessor;-fopenmp")
        set(OpenMP_CXX_LIB_NAMES "omp")
        find_library(OpenMP_omp_LIBRARY NAMES omp PATHS /opt/homebrew/lib /opt/homebrew/Cellar/libomp/*/lib /usr/local/lib)
        find_path(OpenMP_INCLUDE_DIR omp.h PATHS /opt/homebrew/include /opt/homebrew/Cellar/libomp/*/include)
    endif()
endif()

# Compiler flags for optimization
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# fire data analyzer - main program
add_executable(fire-data-analyzer fire-data-analyzer.cpp)

# Link OpenMP to the executable
if(OpenMP_CXX_FOUND)
    target_link_libraries(fire-data-analyzer OpenMP::OpenMP_CXX)
else()
    # Manual linking for Apple Clang
    target_compile_options(fire-data-analyzer PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_libraries(fire-data-analyzer ${OpenMP_omp_LIBRARY})
    target_include_directories(fire-data-analyzer PRIVATE ${OpenMP_INCLUDE_DIR})
endif()
