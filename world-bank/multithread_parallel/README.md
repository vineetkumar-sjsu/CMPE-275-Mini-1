# Parallel Population Analysis

This project implements a comprehensive population data analysis system that compares single-threaded and parallel processing approaches using C++ and std::thread.

## Features

- **Data Loading**: Loads population data from World Bank CSV files
- **Multiple Analysis Functions**:
  - Top countries by population
  - Global population growth calculation
  - Country growth rate analysis
  - Total world population calculation
  - Countries with population above threshold
- **Performance Comparison**: Direct comparison between single-threaded and parallel implementations
- **Comprehensive Analysis**: Complete population analysis with detailed reporting

## Project Structure

```
parallel_population_analysis/
├── include/
│   └── PopulationData.h          # Header file with class definition
├── src/
│   ├── PopulationData.cpp        # Implementation of PopulationData class
│   └── main.cpp                  # Main program with performance comparison
├── data/
│   └── API_SP.POP.TOTL_DS2_en_csv_v2_3401680.csv  # World Bank population data
├── build/                        # Build directory
├── CMakeLists.txt               # CMake build configuration
└── README.md                    # This file
```

## Building the Project

1. Create build directory:
   ```bash
   mkdir build && cd build
   ```

2. Configure with CMake:
   ```bash
   cmake ..
   ```

3. Build the project:
   ```bash
   make
   ```

4. Run the executable:
   ```bash
   ./bin/population_analysis
   ```

## Performance Results

The analysis shows interesting results regarding parallel vs single-threaded performance:

### Key Findings

1. **Thread Overhead**: For small datasets (265 countries), the overhead of thread creation and synchronization exceeds the benefits of parallelization.

2. **Performance Comparison**:
   - Single-threaded total time: 0.23 ms
   - Parallel total time: 9.74 ms
   - Overall speedup: 0.02x (parallel is actually slower)

3. **Why Parallel is Slower**:
   - **Small Dataset**: Only 265 countries to process
   - **Thread Creation Overhead**: Creating 10 threads has significant overhead
   - **Memory Allocation**: Converting unordered_map to vector for parallel processing
   - **Synchronization**: Mutex locks for thread-safe result collection

### When Parallel Would Be Beneficial

Parallel processing would show benefits with:
- **Larger datasets** (thousands of countries)
- **More complex calculations** per data point
- **CPU-intensive operations** rather than simple lookups
- **Better load balancing** across threads

## Technical Implementation

### Parallel Processing Strategy

The implementation uses `std::thread` with the following approach:

1. **Data Preparation**: Convert `unordered_map` to `vector` for random access
2. **Thread Distribution**: Divide work among available CPU cores
3. **Local Processing**: Each thread processes its assigned chunk
4. **Result Aggregation**: Use mutex-protected critical sections to combine results
5. **Thread Synchronization**: Wait for all threads to complete

### Analysis Functions

1. **Top Countries**: Find countries with highest population
2. **Global Growth**: Calculate worldwide population growth rate
3. **Growth Rates**: Compute individual country growth rates
4. **World Population**: Sum total population for a given year
5. **Threshold Filtering**: Find countries above population threshold

## Data Source

The project uses World Bank population data from the file:
`API_SP.POP.TOTL_DS2_en_csv_v2_3401680.csv`

This contains population data for 265 countries/regions from 1960-2023.

## System Requirements

- C++17 compatible compiler
- CMake 3.10+
- POSIX threads support
- macOS/Linux environment

## Compiler Notes

The project was built using g++ on macOS. The CMakeLists.txt is configured to use g++ instead of clang++ to avoid linking issues with the C++ standard library.

## Future Improvements

1. **Optimize Thread Count**: Use fewer threads for small datasets
2. **Better Load Balancing**: Implement work-stealing algorithms
3. **Memory Optimization**: Avoid unnecessary data copying
4. **Async Processing**: Use std::async for better resource management
5. **Larger Datasets**: Test with more comprehensive population data

## Conclusion

This project demonstrates that parallel processing is not always faster than single-threaded approaches. The key lesson is that parallelization benefits depend heavily on:

- Dataset size
- Computational complexity per operation
- Thread creation overhead
- Synchronization costs

For this specific use case with 265 countries and simple operations, single-threaded processing is more efficient. However, the parallel implementation provides a solid foundation for scaling to larger datasets and more complex analyses.
