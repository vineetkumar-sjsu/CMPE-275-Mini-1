#include "PopulationData.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <chrono>

void printSeparator() {
    std::cout << std::string(80, '=') << std::endl;
}

void printSectionHeader(const std::string& title) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

void demonstrateBasicQueries(PopulationData& data) {
    printSectionHeader("BASIC QUERY DEMONSTRATIONS");
    
    // Query 1: Get population for specific countries in specific years
    std::cout << "\n1. Population queries for specific countries and years:" << std::endl;
    
    std::vector<std::pair<std::string, int>> testQueries = {
        {"USA", 2020},
        {"CHN", 2020},
        {"IND", 2020},
        {"USA", 2000},
        {"CHN", 2000},
        {"IND", 2000},
        {"USA", 1990},
        {"CHN", 1990},
        {"IND", 1990}
    };
    
    for (const auto& query : testQueries) {
        long long population = data.getPopulation(query.first, query.second);
        std::string countryName = data.getCountryName(query.first);
        
        if (population > 0) {
            std::cout << "  " << countryName << " (" << query.first << ") in " 
                      << query.second << ": " << std::setw(15) << std::right 
                      << std::to_string(population) << std::endl;
        } else {
            std::cout << "  " << countryName << " (" << query.first << ") in " 
                      << query.second << ": No data available" << std::endl;
        }
    }
    
    // Query 2: Get population by country name
    std::cout << "\n2. Population queries by country name:" << std::endl;
    
    std::vector<std::pair<std::string, int>> nameQueries = {
        {"United States", 2020},
        {"China", 2020},
        {"India", 2020},
        {"Brazil", 2020},
        {"Germany", 2020}
    };
    
    for (const auto& query : nameQueries) {
        long long population = data.getPopulationByName(query.first, query.second);
        
        if (population > 0) {
            std::cout << "  " << query.first << " in " << query.second 
                      << ": " << std::setw(15) << std::right 
                      << std::to_string(population) << std::endl;
        } else {
            std::cout << "  " << query.first << " in " << query.second 
                      << ": No data available" << std::endl;
        }
    }
}

void demonstratePopulationHistory(PopulationData& data) {
    printSectionHeader("POPULATION HISTORY DEMONSTRATIONS");
    
    std::vector<std::string> countries = {"USA", "CHN", "IND", "BRA", "DEU"};
    
    for (const std::string& countryCode : countries) {
        std::string countryName = data.getCountryName(countryCode);
        if (countryName.empty()) continue;
        
        std::cout << "\nPopulation history for " << countryName << " (" << countryCode << "):" << std::endl;
        
        auto history = data.getCountryPopulationHistory(countryCode);
        if (history.empty()) {
            std::cout << "  No data available" << std::endl;
            continue;
        }
        
        // Get years and sort them
        std::vector<int> years;
        for (const auto& pair : history) {
            years.push_back(pair.first);
        }
        std::sort(years.begin(), years.end());
        
        // Show every 10th year
        std::cout << "  Year    Population" << std::endl;
        std::cout << "  ----    ----------" << std::endl;
        
        for (size_t i = 0; i < years.size(); i += 10) {
            int year = years[i];
            long long pop = history[year];
            std::cout << "  " << year << "    " << std::setw(12) << std::right 
                      << std::to_string(pop) << std::endl;
        }
        
        if (years.size() % 10 != 0) {
            int lastYear = years.back();
            long long lastPop = history[lastYear];
            std::cout << "  " << lastYear << "    " << std::setw(12) << std::right 
                      << std::to_string(lastPop) << std::endl;
        }
    }
}

void performanceTesting(PopulationData& data) {
    printSectionHeader("PERFORMANCE TESTING");
    
    // Get all countries for random testing
    auto allCountries = data.getAllCountries();
    if (allCountries.empty()) {
        std::cout << "No countries available for testing" << std::endl;
        return;
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> countryDist(0, allCountries.size() - 1);
    std::uniform_int_distribution<> yearDist(1960, 2023);
    
    // Test 1: Single query performance
    std::cout << "\n1. Single Query Performance Test:" << std::endl;
    
    const int singleQueryTests = 1000;
    double totalTime = 0.0;
    
    for (int i = 0; i < singleQueryTests; ++i) {
        std::string country = allCountries[countryDist(gen)];
        int year = yearDist(gen);
        
        double queryTime = data.measureTime([&]() {
            data.getPopulation(country, year);
        });
        
        totalTime += queryTime;
    }
    
    double avgTime = totalTime / singleQueryTests;
    std::cout << "  " << singleQueryTests << " random queries completed" << std::endl;
    std::cout << "  Average time per query: " << std::fixed << std::setprecision(6) 
              << avgTime << " ms" << std::endl;
    std::cout << "  Total time: " << std::fixed << std::setprecision(3) 
              << totalTime << " ms" << std::endl;
    
    // Test 2: Batch query performance
    std::cout << "\n2. Batch Query Performance Test:" << std::endl;
    
    const int batchSize = 10000;
    std::vector<std::pair<std::string, int>> batchQueries;
    
    // Prepare batch queries
    for (int i = 0; i < batchSize; ++i) {
        std::string country = allCountries[countryDist(gen)];
        int year = yearDist(gen);
        batchQueries.push_back({country, year});
    }
    
    // Measure batch execution time
    auto start = std::chrono::high_resolution_clock::now();
    
    long long totalPopulation = 0;
    int validQueries = 0;
    for (const auto& query : batchQueries) {
        long long pop = data.getPopulation(query.first, query.second);
        if (pop > 0) {
            totalPopulation += pop;
            validQueries++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double batchTime = duration.count() / 1000.0; // Convert to milliseconds
    
    std::cout << "  " << batchSize << " queries in batch completed" << std::endl;
    std::cout << "  Valid queries: " << validQueries << std::endl;
    std::cout << "  Total time: " << std::fixed << std::setprecision(3) 
              << batchTime << " ms" << std::endl;
    std::cout << "  Average time per query: " << std::fixed << std::setprecision(6) 
              << (batchTime / batchSize) << " ms" << std::endl;
    std::cout << "  Queries per second: " << std::fixed << std::setprecision(0) 
              << (batchSize * 1000.0 / batchTime) << std::endl;
    
    // Test 3: Memory usage estimation
    std::cout << "\n3. Memory Usage Estimation:" << std::endl;
    std::cout << "  Countries loaded: " << data.getCountryCount() << std::endl;
    std::cout << "  Available years: " << data.getAvailableYears().size() << std::endl;
    
    // Estimate memory usage (rough calculation)
    size_t estimatedEntries = 0;
    for (const auto& country : allCountries) {
        auto history = data.getCountryPopulationHistory(country);
        estimatedEntries += history.size();
    }
    
    std::cout << "  Total data entries: " << estimatedEntries << std::endl;
    std::cout << "  Estimated memory usage: ~" << (estimatedEntries * 16) / 1024 
              << " KB (rough estimate)" << std::endl;
}

void demonstrateDataStatistics(PopulationData& data) {
    printSectionHeader("DATA STATISTICS");
    
    std::cout << "\nDataset Information:" << std::endl;
    std::cout << "  Total countries: " << data.getCountryCount() << std::endl;
    std::cout << "  Year range: " << data.getAvailableYears().front() 
              << " - " << data.getAvailableYears().back() << std::endl;
    std::cout << "  Total years: " << data.getAvailableYears().size() << std::endl;
    
    // Show some sample countries
    std::cout << "\nSample countries in dataset:" << std::endl;
    auto countries = data.getAllCountries();
    std::sort(countries.begin(), countries.end());
    
    for (size_t i = 0; i < std::min(10UL, countries.size()); ++i) {
        std::string countryName = data.getCountryName(countries[i]);
        std::cout << "  " << (i + 1) << ". " << countryName 
                  << " (" << countries[i] << ")" << std::endl;
    }
    
    if (countries.size() > 10) {
        std::cout << "  ... and " << (countries.size() - 10) << " more countries" << std::endl;
    }
}

int main() {
    std::cout << "Population Data Analysis System" << std::endl;
    std::cout << "===============================" << std::endl;
    
    // Initialize the population data system
    PopulationData data;
    
    // Load data from CSV file
    std::cout << "\nLoading population data from CSV file..." << std::endl;
    std::string csvFile = "population_data/API_SP.POP.TOTL_DS2_en_csv_v2_3401680.csv";
    
    auto loadStart = std::chrono::high_resolution_clock::now();
    bool loadSuccess = data.loadFromCSV(csvFile);
    auto loadEnd = std::chrono::high_resolution_clock::now();
    
    if (!loadSuccess) {
        std::cerr << "Failed to load population data. Exiting..." << std::endl;
        return 1;
    }
    
    auto loadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(loadEnd - loadStart);
    std::cout << "Data loaded successfully in " << loadDuration.count() << " ms" << std::endl;
    
    // Demonstrate various functionalities
    demonstrateDataStatistics(data);
    demonstrateBasicQueries(data);
    demonstratePopulationHistory(data);
    performanceTesting(data);
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "  Analysis Complete!" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    return 0;
}
