#ifndef POPULATION_DATA_H
#define POPULATION_DATA_H

#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>

class PopulationData {
private:
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
    PopulationData();
    
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
    
    //used AI 
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
};

#endif // POPULATION_DATA_H
