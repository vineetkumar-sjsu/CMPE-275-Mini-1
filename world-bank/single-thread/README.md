# Population Data Analysis System

A high-performance C++ application for loading, storing, and querying world population data from CSV files. This project demonstrates efficient in-memory data structures and single-threaded query performance.

## Features

- **CSV Data Loading**: Efficiently loads population data from World Bank CSV files
- **In-Memory Storage**: Uses hash maps for O(1) average-case lookup performance
- **Multiple Query Types**: Support for country code and country name queries
- **Performance Testing**: Built-in timing measurements and performance benchmarks
- **Data Visualization**: Population history display and statistics

## Project Structure

```
mini1/
├── CMakeLists.txt              # CMake build configuration
├── README.md                   # This file
├── include/
│   └── PopulationData.h        # Header file for PopulationData class
├── src/
│   ├── main.cpp               # Main program with demonstrations
│   └── PopulationData.cpp     # Implementation of PopulationData class
├── population_data/
│   ├── API_SP.POP.TOTL_DS2_en_csv_v2_3401680.csv
│   ├── Metadata_Country_API_SP.POP.TOTL_DS2_en_csv_v2_3401680.csv
│   └── Metadata_Indicator_API_SP.POP.TOTL_DS2_en_csv_v2_3401680.csv
└── build/                     # Build directory (created during compilation)
    └── bin/
        └── population_analysis # Executable
```

## Data Structure

The application uses a nested hash map structure for optimal query performance:

```cpp
std::unordered_map<std::string, std::unordered_map<int, long long>> countryData;
```

- **Outer Key**: Country Code (e.g., "USA", "CHN", "IND")
- **Inner Key**: Year (1960-2023)
- **Value**: Population count

## Performance Results

Based on testing with 265 countries and 64 years of data:

- **Data Loading**: ~2ms for 265 countries
- **Single Query**: <0.001ms average
- **Batch Queries**: 54+ million queries per second
- **Memory Usage**: ~264KB for complete dataset
- **Data Entries**: 16,930 total population records

## Building and Running

### Prerequisites

- GCC C++ compiler (C++17 support)
- CMake 3.10 or higher

### Build Instructions

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Compile the project
make

# Run the program
cd .. && ./build/bin/population_analysis
```

### Alternative Build (if GCC is not default)

```bash
# Force GCC usage
export CXX=g++
mkdir build && cd build
cmake ..
make
```

## Usage Examples

### Basic Queries

```cpp
PopulationData data;
data.loadFromCSV("population_data/API_SP.POP.TOTL_DS2_en_csv_v2_3401680.csv");

// Query by country code
long long usPop2020 = data.getPopulation("USA", 2020);

// Query by country name
long long chinaPop2020 = data.getPopulationByName("China", 2020);

// Get population history
auto history = data.getCountryPopulationHistory("USA");
```

### Performance Measurement

```cpp
// Measure query time
double queryTime = data.measureTime([&]() {
    data.getPopulation("USA", 2020);
});
```

## Sample Output

```
Population Data Analysis System
===============================

Loading population data from CSV file...
Loaded data for 265 countries
Data loaded successfully in 2 ms

================================================================================
  BASIC QUERY DEMONSTRATIONS
================================================================================

1. Population queries for specific countries and years:
  United States (USA) in 2020:       331526933
  China (CHN) in 2020:      1411100000
  India (IND) in 2020:      1396387127

2. Population queries by country name:
  United States in 2020:       331526933
  China in 2020:      1411100000
  India in 2020:      1396387127

================================================================================
  PERFORMANCE TESTING
================================================================================

1. Single Query Performance Test:
  1000 random queries completed
  Average time per query: 0.000000 ms
  Total time: 0.000 ms

2. Batch Query Performance Test:
  10000 queries in batch completed
  Valid queries: 9956
  Total time: 0.183 ms
  Average time per query: 0.000018 ms
  Queries per second: 54644809
```

## Technical Details

### Data Loading
- Efficient CSV parsing with proper quote handling
- Skips empty lines and header rows automatically
- Validates data integrity during loading

### Query Optimization
- Hash map lookups provide O(1) average-case performance
- No parallelization (single-threaded as requested)
- Minimal memory overhead with efficient data structures

### Error Handling
- Graceful handling of missing data
- File I/O error checking
- Data validation during parsing

## Future Enhancements

- Parallel data loading for larger datasets
- Additional query types (range queries, aggregations)
- Data export functionality
- Interactive query interface
- Memory usage optimization for very large datasets

## License

This project is created for educational purposes as part of CMPE285 coursework.
