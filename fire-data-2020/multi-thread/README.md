# Fire Data Analyzer - Multi Thread

A C++ program to analyze air quality data from the 2020 fire season using CMake, OpenMP on multi threaded environment using OMP library.

## Overview

This program loads air quality data from CSV files and provides efficient querying capabilities with performance timing measurements. It implements **OpenMP parallelization** for improved performance on multi-core systems, achieving up to 3x speedup for query operations.

## Data Structure

The program uses a `std::vector<AirQualityRecord>` to store the fire data in memory. Each record contains:

- **Latitude/Longitude**: Geographic coordinates
- **DateTime**: Timestamp in YYYY-MM-DDTHH:MM format
- **Parameter**: Air quality parameter (PM2.5, PM10, OZONE, CO, NO2, SO2)
- **Value**: Measured concentration value
- **Unit**: Measurement unit (typically UG/M3)
- **AQI**: Air Quality Index (0-500+ scale)
- **AQI Category**: AQI category number
- **Site Information**: Site name, agency, and IDs

## Features

### Implemented Queries

1. **Get AQI data for a specific day**: Returns all air quality records for a given date
2. **Get days where AQI was above threshold**: Finds all dates where the maximum AQI exceeded a specified value
3. **Get average AQI for a specific date**: Calculates the mean AQI for all measurements on a given day

### Performance Measurements

All queries include timing measurements using `std::chrono::high_resolution_clock` to measure execution time in microseconds.

## Building and Running

### Prerequisites
- C++ compiler with C++17 support (GCC, Clang, or Apple Clang)
- CMake 3.3 or higher
- OpenMP library (libomp for macOS, libgomp for Linux)

### Build Instructions

```bash
# Clean build (recommended)
rm -rf build

# Create build directory
mkdir build
cd build

# Configure with CMake (includes OpenMP detection)
cmake ..

# Build the project
make

# Return to project root
cd ..

# Run with different thread counts
OMP_NUM_THREADS=1 ./build/fire-data-analyzer  # Sequential
OMP_NUM_THREADS=4 ./build/fire-data-analyzer  # 4 threads (optimal)
OMP_NUM_THREADS=8 ./build/fire-data-analyzer  # 8 threads
./build/fire-data-analyzer                     # Default system threads
```

## Performance Results

Based on test runs with 1,167,525 records:

### Single Thread Performance (OMP_NUM_THREADS=1):
- **Data Loading**: 2,342 ms
- **Date Query**: 20,383 μs average per query
- **AQI Threshold Query**: ~62 ms
- **Average AQI Query**: ~6 ms

### Multi-Thread Performance (OMP_NUM_THREADS=4):
- **Data Loading**: 2,191 ms (6.4% improvement)
- **Date Query**: 7,350 μs average per query (**2.77x speedup**)
- **AQI Threshold Query**: ~60 ms
- **Average AQI Query**: ~6 ms

### Performance Summary:
- **Best Thread Count**: 4 threads for optimal performance
- **Query Speedup**: Up to 2.77x faster with parallelization
- **Memory Usage**: ~1.2GB for full dataset in memory

## Data Insights

From the 2020 fire data analysis:
- **43 consecutive days** of data from August 10 to September 24, 2020
- **All 43 days** had AQI readings above 100 (indicating poor air quality)
- **PM2.5 and OZONE** were the most frequently measured parameters
- AQI values ranged from -999 (sensor errors) to 7190 (extremely hazardous)

## Architecture

The program implements **OpenMP parallelization** with:

### Parallelized Components:
1. **File Loading**: Parallel processing of multiple CSV files with critical sections
2. **CSV Parsing**: Parallel parsing of lines within each file
3. **Query Operations**: Parallel search through records with thread-safe operations

### Design Features:
- **Thread-Safe Operations**: Uses `#pragma omp critical` for shared data access
- **Scalable Performance**: Optimal performance with 4 threads on modern systems
- **Memory Efficient**: In-memory storage using `std::vector` and `std::map`
- **Cross-Platform**: Compatible with GCC, Clang, and Apple Clang compilers

### OpenMP Implementation Details:
- **File Loading Parallelization**: `#pragma omp parallel for` with critical sections
- **Search Query Parallelization**: Indexed loops for thread-safe parallel processing
- **Thread Configuration**: Controlled via `OMP_NUM_THREADS` environment variable

This approach provides significant performance improvements while maintaining code clarity and cross platform compatibility.
