# Fire Data Analyzer - Single Thread

A C++ program to analyze air quality data from the 2020 fire season using CMake and GCC on single threaded environments.

## Overview

This program loads air quality data from CSV files and provides efficient querying capabilities with performance timing measurements. It implements a single threaded approach without parallelization as requested.

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
- GCC C++ compiler with C++17 support
- CMake 3.3 or higher

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
make

# Run the analyzer
cd ..
./build/fire-data-analyzer
```

## Performance Results

Based on the test run with 1,167,525 records:

- **Data Loading**: ~2.1 seconds for all CSV files
- **Date Query**: ~20-21 milliseconds average per query
- **AQI Threshold Query**: ~62 milliseconds
- **Average AQI Query**: ~6 milliseconds

## Data Insights

From the 2020 fire data analysis:
- **43 consecutive days** of data from August 10 to September 24, 2020
- **All 43 days** had AQI readings above 100 (indicating poor air quality)
- **PM2.5 and OZONE** were the most frequently measured parameters
- AQI values ranged from -999 (sensor errors) to 7190 (extremely hazardous)

## Architecture

The program follows a single-threaded design with:
- **Sequential file processing**: CSV files are loaded one by one
- **Linear search algorithms**: Queries use simple loops for data filtering
- **In-memory storage**: All data is kept in RAM for fast querying
- **Standard library containers**: Uses `std::vector` and `std::map` for efficiency

This approach prioritizes simplicity and ensures predictable performance characteristics without the complexity of parallel processing.
