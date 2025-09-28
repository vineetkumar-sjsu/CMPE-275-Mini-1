cmake_minimum_required(VERSION 3.3)

project (fire-data-analysis)

# Set C++17 standard for filesystem support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# compilers to use (9.2) - commented out to use default system compilers
# set(CMAKE_C_COMPILER /opt/homebrew/Cellar/gcc/11.2.0_3/bin/gcc-11)
# set(CMAKE_CXX_COMPILER  /opt/homebrew/Cellar/gcc/11.2.0_3/bin/g++-11)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules)

# Compiler flags for optimization
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# fire data analyzer - main program
add_executable(fire-data-analyzer fire-data-analyzer.cpp)
