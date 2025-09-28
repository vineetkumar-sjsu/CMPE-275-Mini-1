#ifndef POPULATION_DATA_H
#define POPULATION_DATA_H

#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <future>
#include <mutex>

class PopulationData {
private:
    // Data structure to store population data
    // Key: Country Code (e.g., "USA", "CHN")
    // Value: Map of year to population
    std::unordered_map<std::string, std::unordered_map<int, long long>> countryData;
    
    // Store country names for display
    std::unordered_map<std::string, std::string> countryNames;
    
    // Available years for queries
    std::vector<int> availableYears;
    
    // Helper function to parse CSV line
    std::vector<std::string> parseCSVLine(const std::string& line);
    
    // Helper function to convert string to long long
    long long stringToLongLong(const std::string& str);

public:
    // Constructor
    PopulationData();
    
    // Destructor
    ~PopulationData();
    
    // Load data from CSV file
    bool loadFromCSV(const std::string& filename);
    
    // Query functions
    long long getPopulation(const std::string& countryCode, int year);
    long long getPopulationByName(const std::string& countryName, int year);
    
    // Get all available years
    const std::vector<int>& getAvailableYears() const;
    
    // Get country name by code
    std::string getCountryName(const std::string& countryCode) const;
    
    // Get all countries
    std::vector<std::string> getAllCountries() const;
    
    // Get population for a country across all years
    std::unordered_map<int, long long> getCountryPopulationHistory(const std::string& countryCode) const;
    
    // Get total number of countries loaded
    size_t getCountryCount() const;
    
    // Performance measurement helper
    template<typename Func>
    double measureTime(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / 1000.0; // Return time in milliseconds
    }
    
    // Display functions
    void printCountryInfo(const std::string& countryCode) const;
    void printAllCountries() const;
    
    // Parallel processing functions
    std::vector<std::pair<std::string, long long>> getTopCountriesByPopulation(int year, int topN = 10, bool useParallel = true);
    double calculateGlobalPopulationGrowth(int startYear, int endYear, bool useParallel = true);
    std::vector<std::pair<std::string, double>> calculateCountryGrowthRates(int startYear, int endYear, bool useParallel = true);
    long long calculateTotalWorldPopulation(int year, bool useParallel = true);
    std::vector<std::pair<std::string, long long>> findCountriesWithPopulationAbove(long long threshold, int year, bool useParallel = true);
    
    // Analysis functions
    void performComprehensiveAnalysis(bool useParallel = true);
};

#endif // POPULATION_DATA_H
