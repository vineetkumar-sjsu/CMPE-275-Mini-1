#include "PopulationData.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>

void printPerformanceComparison(const std::vector<std::pair<std::string, double>>& times) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "PERFORMANCE COMPARISON RESULTS" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    std::cout << std::setw(30) << std::left << "Operation" 
              << std::setw(20) << std::right << "Single-threaded (ms)"
              << std::setw(20) << std::right << "Parallel (ms)"
              << std::setw(15) << std::right << "Speedup" << std::endl;
    std::cout << std::string(85, '-') << std::endl;
    
    for (const auto& time : times) {
        std::cout << std::setw(30) << std::left << time.first
                  << std::setw(20) << std::right << std::fixed << std::setprecision(2) << time.second
                  << std::setw(20) << std::right << std::fixed << std::setprecision(2) << time.second
                  << std::setw(15) << std::right << std::fixed << std::setprecision(2) << 1.0 << std::endl;
    }
}

int main() {
    std::cout << "Parallel Population Analysis - Performance Comparison" << std::endl;
    std::cout << "=====================================================" << std::endl;
    
    // Load population data
    PopulationData data;
    std::string csvFile = "data/API_SP.POP.TOTL_DS2_en_csv_v2_3401680.csv";
    
    std::cout << "\nLoading population data from: " << csvFile << std::endl;
    if (!data.loadFromCSV(csvFile)) {
        std::cerr << "Failed to load data from CSV file!" << std::endl;
        return 1;
    }
    
    std::cout << "Data loaded successfully!" << std::endl;
    std::cout << "Countries loaded: " << data.getCountryCount() << std::endl;
    
    // Performance measurement vectors
    std::vector<std::pair<std::string, double>> singleThreadTimes;
    std::vector<std::pair<std::string, double>> parallelTimes;
    
    // Test 1: Top Countries by Population
    std::cout << "\n" << std::string(60, '-') << std::endl;
    std::cout << "TEST 1: Top Countries by Population (2020)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    // Single-threaded
    double time1 = data.measureTime([&]() {
        auto result = data.getTopCountriesByPopulation(2020, 10, false);
    });
    singleThreadTimes.push_back({"Top Countries", time1});
    std::cout << "Single-threaded time: " << std::fixed << std::setprecision(2) << time1 << " ms" << std::endl;
    
    // Parallel
    double time2 = data.measureTime([&]() {
        auto result = data.getTopCountriesByPopulation(2020, 10, true);
    });
    parallelTimes.push_back({"Top Countries", time2});
    std::cout << "Parallel time: " << std::fixed << std::setprecision(2) << time2 << " ms" << std::endl;
    std::cout << "Speedup: " << std::fixed << std::setprecision(2) << (time1 / time2) << "x" << std::endl;
    
    // Test 2: Global Population Growth
    std::cout << "\n" << std::string(60, '-') << std::endl;
    std::cout << "TEST 2: Global Population Growth (1960-2020)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    // Single-threaded
    double time3 = data.measureTime([&]() {
        data.calculateGlobalPopulationGrowth(1960, 2020, false);
    });
    singleThreadTimes.push_back({"Global Growth", time3});
    std::cout << "Single-threaded time: " << std::fixed << std::setprecision(2) << time3 << " ms" << std::endl;
    
    // Parallel
    double time4 = data.measureTime([&]() {
        data.calculateGlobalPopulationGrowth(1960, 2020, true);
    });
    parallelTimes.push_back({"Global Growth", time4});
    std::cout << "Parallel time: " << std::fixed << std::setprecision(2) << time4 << " ms" << std::endl;
    std::cout << "Speedup: " << std::fixed << std::setprecision(2) << (time3 / time4) << "x" << std::endl;
    
    // Test 3: Country Growth Rates
    std::cout << "\n" << std::string(60, '-') << std::endl;
    std::cout << "TEST 3: Country Growth Rates (1960-2020)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    // Single-threaded
    double time5 = data.measureTime([&]() {
        auto result = data.calculateCountryGrowthRates(1960, 2020, false);
    });
    singleThreadTimes.push_back({"Growth Rates", time5});
    std::cout << "Single-threaded time: " << std::fixed << std::setprecision(2) << time5 << " ms" << std::endl;
    
    // Parallel
    double time6 = data.measureTime([&]() {
        auto result = data.calculateCountryGrowthRates(1960, 2020, true);
    });
    parallelTimes.push_back({"Growth Rates", time6});
    std::cout << "Parallel time: " << std::fixed << std::setprecision(2) << time6 << " ms" << std::endl;
    std::cout << "Speedup: " << std::fixed << std::setprecision(2) << (time5 / time6) << "x" << std::endl;
    
    // Test 4: Total World Population
    std::cout << "\n" << std::string(60, '-') << std::endl;
    std::cout << "TEST 4: Total World Population (2020)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    // Single-threaded
    double time7 = data.measureTime([&]() {
        data.calculateTotalWorldPopulation(2020, false);
    });
    singleThreadTimes.push_back({"World Population", time7});
    std::cout << "Single-threaded time: " << std::fixed << std::setprecision(2) << time7 << " ms" << std::endl;
    
    // Parallel
    double time8 = data.measureTime([&]() {
        data.calculateTotalWorldPopulation(2020, true);
    });
    parallelTimes.push_back({"World Population", time8});
    std::cout << "Parallel time: " << std::fixed << std::setprecision(2) << time8 << " ms" << std::endl;
    std::cout << "Speedup: " << std::fixed << std::setprecision(2) << (time7 / time8) << "x" << std::endl;
    
    // Test 5: Countries with Population Above Threshold
    std::cout << "\n" << std::string(60, '-') << std::endl;
    std::cout << "TEST 5: Countries with Population > 100M (2020)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    // Single-threaded
    double time9 = data.measureTime([&]() {
        auto result = data.findCountriesWithPopulationAbove(100000000, 2020, false);
    });
    singleThreadTimes.push_back({"Large Countries", time9});
    std::cout << "Single-threaded time: " << std::fixed << std::setprecision(2) << time9 << " ms" << std::endl;
    
    // Parallel
    double time10 = data.measureTime([&]() {
        auto result = data.findCountriesWithPopulationAbove(100000000, 2020, true);
    });
    parallelTimes.push_back({"Large Countries", time10});
    std::cout << "Parallel time: " << std::fixed << std::setprecision(2) << time10 << " ms" << std::endl;
    std::cout << "Speedup: " << std::fixed << std::setprecision(2) << (time9 / time10) << "x" << std::endl;
    
    // Comprehensive Analysis Comparison
    std::cout << "\n" << std::string(60, '-') << std::endl;
    std::cout << "COMPREHENSIVE ANALYSIS COMPARISON" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    // Single-threaded comprehensive analysis
    std::cout << "\nSINGLE-THREADED COMPREHENSIVE ANALYSIS:" << std::endl;
    double comprehensiveSingle = data.measureTime([&]() {
        data.performComprehensiveAnalysis(false);
    });
    singleThreadTimes.push_back({"Comprehensive", comprehensiveSingle});
    std::cout << "Single-threaded comprehensive analysis time: " 
              << std::fixed << std::setprecision(2) << comprehensiveSingle << " ms" << std::endl;
    
    // Parallel comprehensive analysis
    std::cout << "\nPARALLEL COMPREHENSIVE ANALYSIS:" << std::endl;
    double comprehensiveParallel = data.measureTime([&]() {
        data.performComprehensiveAnalysis(true);
    });
    parallelTimes.push_back({"Comprehensive", comprehensiveParallel});
    std::cout << "Parallel comprehensive analysis time: " 
              << std::fixed << std::setprecision(2) << comprehensiveParallel << " ms" << std::endl;
    std::cout << "Comprehensive analysis speedup: " 
              << std::fixed << std::setprecision(2) << (comprehensiveSingle / comprehensiveParallel) << "x" << std::endl;
    
    // Final Performance Summary
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "FINAL PERFORMANCE SUMMARY" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    std::cout << std::setw(30) << std::left << "Operation" 
              << std::setw(20) << std::right << "Single-threaded (ms)"
              << std::setw(20) << std::right << "Parallel (ms)"
              << std::setw(15) << std::right << "Speedup" << std::endl;
    std::cout << std::string(85, '-') << std::endl;
    
    double totalSingleTime = 0;
    double totalParallelTime = 0;
    
    for (size_t i = 0; i < singleThreadTimes.size(); ++i) {
        double speedup = singleThreadTimes[i].second / parallelTimes[i].second;
        totalSingleTime += singleThreadTimes[i].second;
        totalParallelTime += parallelTimes[i].second;
        
        std::cout << std::setw(30) << std::left << singleThreadTimes[i].first
                  << std::setw(20) << std::right << std::fixed << std::setprecision(2) << singleThreadTimes[i].second
                  << std::setw(20) << std::right << std::fixed << std::setprecision(2) << parallelTimes[i].second
                  << std::setw(15) << std::right << std::fixed << std::setprecision(2) << speedup << std::endl;
    }
    
    std::cout << std::string(85, '-') << std::endl;
    double overallSpeedup = totalSingleTime / totalParallelTime;
    std::cout << std::setw(30) << std::left << "TOTAL"
              << std::setw(20) << std::right << std::fixed << std::setprecision(2) << totalSingleTime
              << std::setw(20) << std::right << std::fixed << std::setprecision(2) << totalParallelTime
              << std::setw(15) << std::right << std::fixed << std::setprecision(2) << overallSpeedup << std::endl;
    
    std::cout << "\nOverall Performance Improvement: " << std::fixed << std::setprecision(2) 
              << overallSpeedup << "x faster with parallel processing" << std::endl;
    
    // System Information
    std::cout << "\n" << std::string(60, '-') << std::endl;
    std::cout << "SYSTEM INFORMATION" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "Number of threads used: " << std::thread::hardware_concurrency() << std::endl;
    std::cout << "Countries processed: " << data.getCountryCount() << std::endl;
    std::cout << "Years of data: 1960-2023 (64 years)" << std::endl;
    
    return 0;
}
