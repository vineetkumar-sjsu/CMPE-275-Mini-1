#include "PopulationData.h"
#include <algorithm>
#include <iomanip>
#include <numeric>

PopulationData::PopulationData() {
    // Initialize available years (1960-2023)
    for (int year = 1960; year <= 2023; ++year) {
        availableYears.push_back(year);
    }
    
    // Initialize pthread mutex
    pthread_mutex_init(&resultsMutex, NULL);
    
    // Initialize thread attributes
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);
}

PopulationData::~PopulationData() {
    // Clean up pthread resources
    pthread_mutex_destroy(&resultsMutex);
    pthread_attr_destroy(&threadAttr);
}

std::vector<std::string> PopulationData::parseCSVLine(const std::string& line) {
    std::vector<std::string> result;
    std::string current;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            result.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    
    // Add the last field
    if (!current.empty()) {
        result.push_back(current);
    }
    
    return result;
}

long long PopulationData::stringToLongLong(const std::string& str) {
    if (str.empty() || str == "\"\"") {
        return -1; // Indicates no data available
    }
    
    try {
        return std::stoll(str);
    } catch (const std::exception&) {
        return -1; // Indicates invalid data
    }
}

bool PopulationData::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }
    
    std::string line;
    
    while (std::getline(file, line)) {
        
        // Skip empty lines and header lines
        if (line.empty() || line.find("Data Source") != std::string::npos || 
            line.find("Last Updated Date") != std::string::npos) {
            continue;
        }
        
        // Skip the header line with column names (starts with "Country Name")
        if (line.find("Country Name") != std::string::npos) {
            continue;
        }
        
        std::vector<std::string> fields = parseCSVLine(line);
        
        // Expected format: Country Name, Country Code, Indicator Name, Indicator Code, 1960, 1961, ..., 2023
        if (fields.size() < 68) { // 4 metadata columns + 64 years (1960-2023)
            continue;
        }
        
        std::string countryName = fields[0];
        std::string countryCode = fields[1];
        std::string indicatorName = fields[2];
        
        
        // Only process population data
        if (indicatorName != "Population, total") {
            continue;
        }
        
        // Store country name
        countryNames[countryCode] = countryName;
        
        // Parse population data for each year
        for (int i = 4; i < static_cast<int>(fields.size()) && i < 68; ++i) {
            int year = 1960 + (i - 4);
            long long population = stringToLongLong(fields[i]);
            
            if (population > 0) { // Only store valid population data
                countryData[countryCode][year] = population;
            }
        }
    }
    
    file.close();
    
    std::cout << "Loaded data for " << countryData.size() << " countries" << std::endl;
    return true;
}

long long PopulationData::getPopulation(const std::string& countryCode, int year) {
    auto countryIt = countryData.find(countryCode);
    if (countryIt == countryData.end()) {
        return -1; // Country not found
    }
    
    auto yearIt = countryIt->second.find(year);
    if (yearIt == countryIt->second.end()) {
        return -1; // Year not found for this country
    }
    
    return yearIt->second;
}

long long PopulationData::getPopulationByName(const std::string& countryName, int year) {
    // Find country code by name
    for (const auto& pair : countryNames) {
        if (pair.second == countryName) {
            return getPopulation(pair.first, year);
        }
    }
    
    return -1; // Country name not found
}

const std::vector<int>& PopulationData::getAvailableYears() const {
    return availableYears;
}

std::string PopulationData::getCountryName(const std::string& countryCode) const {
    auto it = countryNames.find(countryCode);
    if (it != countryNames.end()) {
        return it->second;
    }
    return "";
}

std::vector<std::string> PopulationData::getAllCountries() const {
    std::vector<std::string> countries;
    for (const auto& pair : countryNames) {
        countries.push_back(pair.first);
    }
    return countries;
}

std::unordered_map<int, long long> PopulationData::getCountryPopulationHistory(const std::string& countryCode) const {
    auto countryIt = countryData.find(countryCode);
    if (countryIt == countryData.end()) {
        return std::unordered_map<int, long long>();
    }
    
    return countryIt->second;
}

size_t PopulationData::getCountryCount() const {
    return countryData.size();
}

void PopulationData::printCountryInfo(const std::string& countryCode) const {
    auto countryIt = countryData.find(countryCode);
    if (countryIt == countryData.end()) {
        std::cout << "Country not found: " << countryCode << std::endl;
        return;
    }
    
    std::string countryName = getCountryName(countryCode);
    std::cout << "\nCountry: " << countryName << " (" << countryCode << ")" << std::endl;
    std::cout << "Population data available for " << countryIt->second.size() << " years" << std::endl;
    
    // Show some sample years
    std::vector<int> years;
    for (const auto& pair : countryIt->second) {
        years.push_back(pair.first);
    }
    std::sort(years.begin(), years.end());
    
    std::cout << "Sample data:" << std::endl;
    for (size_t i = 0; i < std::min(5UL, years.size()); ++i) {
        int year = years[i];
        long long pop = countryIt->second.at(year);
        std::cout << "  " << year << ": " << std::setw(12) << std::right << pop << std::endl;
    }
    
    if (years.size() > 5) {
        std::cout << "  ... and " << (years.size() - 5) << " more years" << std::endl;
    }
}

void PopulationData::printAllCountries() const {
    std::cout << "\nAvailable countries (" << countryData.size() << " total):" << std::endl;
    
    std::vector<std::pair<std::string, std::string>> countries;
    for (const auto& pair : countryNames) {
        countries.push_back({pair.first, pair.second});
    }
    
    // Sort by country name
    std::sort(countries.begin(), countries.end(), 
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    for (size_t i = 0; i < countries.size(); ++i) {
        std::cout << std::setw(3) << std::right << (i + 1) << ". " 
                  << std::setw(50) << std::left << countries[i].second 
                  << " (" << countries[i].first << ")" << std::endl;
    }
}

// Parallel processing implementations
std::vector<std::pair<std::string, long long>> PopulationData::getTopCountriesByPopulation(int year, int topN, bool useParallel) {
    std::vector<std::pair<std::string, long long>> results;
    
    if (useParallel) {
        // Parallel implementation using pthreads
        std::vector<std::pair<std::string, long long>> tempResults;
        
        // Convert to vector for parallel processing
        std::vector<std::pair<std::string, std::unordered_map<int, long long>>> countryVector;
        for (const auto& country : countryData) {
            countryVector.push_back(country);
        }
        
        // Get number of threads
        int numThreads = 4; // Use fixed number for better control
        if (countryVector.size() < numThreads) {
            numThreads = countryVector.size();
        }
        
        pthread_t threads[numThreads];
        ThreadDataTopCountries threadData[numThreads];
        size_t chunkSize = countryVector.size() / numThreads;
        
        for (int t = 0; t < numThreads; ++t) {
            threadData[t].data = this;
            threadData[t].countryVector = &countryVector;
            threadData[t].year = year;
            threadData[t].topN = topN;
            threadData[t].results = &tempResults;
            threadData[t].start = t * chunkSize;
            threadData[t].end = (t == numThreads - 1) ? countryVector.size() : (t + 1) * chunkSize;
            
            pthread_create(&threads[t], &threadAttr, threadWorkerTopCountries, &threadData[t]);
        }
        
        // Wait for all threads to complete
        for (int t = 0; t < numThreads; ++t) {
            pthread_join(threads[t], NULL);
        }
        
        // Sort by population (descending)
        std::sort(tempResults.begin(), tempResults.end(), 
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Take top N
        results.assign(tempResults.begin(), 
                      tempResults.begin() + std::min(topN, static_cast<int>(tempResults.size())));
    } else {
        // Single-threaded implementation
        for (const auto& country : countryData) {
            auto yearIt = country.second.find(year);
            if (yearIt != country.second.end() && yearIt->second > 0) {
                results.push_back({country.first, yearIt->second});
            }
        }
        
        // Sort by population (descending)
        std::sort(results.begin(), results.end(), 
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Take top N
        if (results.size() > static_cast<size_t>(topN)) {
            results.resize(topN);
        }
    }
    
    return results;
}

double PopulationData::calculateGlobalPopulationGrowth(int startYear, int endYear, bool useParallel) {
    long long startPopulation = 0;
    long long endPopulation = 0;
    
    if (useParallel) {
        // Parallel implementation using pthreads
        // Convert to vector for parallel processing
        std::vector<std::pair<std::string, std::unordered_map<int, long long>>> countryVector;
        for (const auto& country : countryData) {
            countryVector.push_back(country);
        }
        
        // Get number of threads
        int numThreads = 4;
        if (countryVector.size() < numThreads) {
            numThreads = countryVector.size();
        }
        
        pthread_t threads[numThreads];
        ThreadDataGlobalGrowth threadData[numThreads];
        size_t chunkSize = countryVector.size() / numThreads;
        
        for (int t = 0; t < numThreads; ++t) {
            threadData[t].data = this;
            threadData[t].countryVector = &countryVector;
            threadData[t].startYear = startYear;
            threadData[t].endYear = endYear;
            threadData[t].startPopulation = &startPopulation;
            threadData[t].endPopulation = &endPopulation;
            threadData[t].start = t * chunkSize;
            threadData[t].end = (t == numThreads - 1) ? countryVector.size() : (t + 1) * chunkSize;
            
            pthread_create(&threads[t], &threadAttr, threadWorkerGlobalGrowth, &threadData[t]);
        }
        
        // Wait for all threads to complete
        for (int t = 0; t < numThreads; ++t) {
            pthread_join(threads[t], NULL);
        }
    } else {
        // Single-threaded implementation
        for (const auto& country : countryData) {
            auto startIt = country.second.find(startYear);
            auto endIt = country.second.find(endYear);
            
            if (startIt != country.second.end() && startIt->second > 0) {
                startPopulation += startIt->second;
            }
            if (endIt != country.second.end() && endIt->second > 0) {
                endPopulation += endIt->second;
            }
        }
    }
    
    if (startPopulation == 0) return 0.0;
    return ((double)(endPopulation - startPopulation) / startPopulation) * 100.0;
}

std::vector<std::pair<std::string, double>> PopulationData::calculateCountryGrowthRates(int startYear, int endYear, bool useParallel) {
    std::vector<std::pair<std::string, double>> results;
    
    if (useParallel) {
        // Parallel implementation using pthreads
        std::vector<std::pair<std::string, double>> tempResults;
        
        // Convert to vector for parallel processing
        std::vector<std::pair<std::string, std::unordered_map<int, long long>>> countryVector;
        for (const auto& country : countryData) {
            countryVector.push_back(country);
        }
        
        // Get number of threads
        int numThreads = 4;
        if (countryVector.size() < numThreads) {
            numThreads = countryVector.size();
        }
        
        pthread_t threads[numThreads];
        ThreadDataGrowthRates threadData[numThreads];
        size_t chunkSize = countryVector.size() / numThreads;
        
        for (int t = 0; t < numThreads; ++t) {
            threadData[t].data = this;
            threadData[t].countryVector = &countryVector;
            threadData[t].startYear = startYear;
            threadData[t].endYear = endYear;
            threadData[t].results = &tempResults;
            threadData[t].start = t * chunkSize;
            threadData[t].end = (t == numThreads - 1) ? countryVector.size() : (t + 1) * chunkSize;
            
            pthread_create(&threads[t], &threadAttr, threadWorkerGrowthRates, &threadData[t]);
        }
        
        // Wait for all threads to complete
        for (int t = 0; t < numThreads; ++t) {
            pthread_join(threads[t], NULL);
        }
        
        results = std::move(tempResults);
    } else {
        // Single-threaded implementation
        for (const auto& country : countryData) {
            auto startIt = country.second.find(startYear);
            auto endIt = country.second.find(endYear);
            
            if (startIt != country.second.end() && endIt != country.second.end() && 
                startIt->second > 0 && endIt->second > 0) {
                double growthRate = ((double)(endIt->second - startIt->second) / startIt->second) * 100.0;
                results.push_back({country.first, growthRate});
            }
        }
    }
    
    // Sort by growth rate (descending)
    std::sort(results.begin(), results.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return results;
}

long long PopulationData::calculateTotalWorldPopulation(int year, bool useParallel) {
    long long totalPopulation = 0;
    
    if (useParallel) {
        // Parallel implementation using pthreads
        // Convert to vector for parallel processing
        std::vector<std::pair<std::string, std::unordered_map<int, long long>>> countryVector;
        for (const auto& country : countryData) {
            countryVector.push_back(country);
        }
        
        // Get number of threads
        int numThreads = 4;
        if (countryVector.size() < numThreads) {
            numThreads = countryVector.size();
        }
        
        pthread_t threads[numThreads];
        ThreadDataWorldPopulation threadData[numThreads];
        size_t chunkSize = countryVector.size() / numThreads;
        
        for (int t = 0; t < numThreads; ++t) {
            threadData[t].data = this;
            threadData[t].countryVector = &countryVector;
            threadData[t].year = year;
            threadData[t].totalPopulation = &totalPopulation;
            threadData[t].start = t * chunkSize;
            threadData[t].end = (t == numThreads - 1) ? countryVector.size() : (t + 1) * chunkSize;
            
            pthread_create(&threads[t], &threadAttr, threadWorkerWorldPopulation, &threadData[t]);
        }
        
        // Wait for all threads to complete
        for (int t = 0; t < numThreads; ++t) {
            pthread_join(threads[t], NULL);
        }
    } else {
        // Single-threaded implementation
        for (const auto& country : countryData) {
            auto yearIt = country.second.find(year);
            if (yearIt != country.second.end() && yearIt->second > 0) {
                totalPopulation += yearIt->second;
            }
        }
    }
    
    return totalPopulation;
}

std::vector<std::pair<std::string, long long>> PopulationData::findCountriesWithPopulationAbove(long long threshold, int year, bool useParallel) {
    std::vector<std::pair<std::string, long long>> results;
    
    if (useParallel) {
        // Parallel implementation using pthreads
        std::vector<std::pair<std::string, long long>> tempResults;
        
        // Convert to vector for parallel processing
        std::vector<std::pair<std::string, std::unordered_map<int, long long>>> countryVector;
        for (const auto& country : countryData) {
            countryVector.push_back(country);
        }
        
        // Get number of threads
        int numThreads = 4;
        if (countryVector.size() < numThreads) {
            numThreads = countryVector.size();
        }
        
        pthread_t threads[numThreads];
        ThreadDataLargeCountries threadData[numThreads];
        size_t chunkSize = countryVector.size() / numThreads;
        
        for (int t = 0; t < numThreads; ++t) {
            threadData[t].data = this;
            threadData[t].countryVector = &countryVector;
            threadData[t].threshold = threshold;
            threadData[t].year = year;
            threadData[t].results = &tempResults;
            threadData[t].start = t * chunkSize;
            threadData[t].end = (t == numThreads - 1) ? countryVector.size() : (t + 1) * chunkSize;
            
            pthread_create(&threads[t], &threadAttr, threadWorkerLargeCountries, &threadData[t]);
        }
        
        // Wait for all threads to complete
        for (int t = 0; t < numThreads; ++t) {
            pthread_join(threads[t], NULL);
        }
        
        results = std::move(tempResults);
    } else {
        // Single-threaded implementation
        for (const auto& country : countryData) {
            auto yearIt = country.second.find(year);
            if (yearIt != country.second.end() && yearIt->second >= threshold) {
                results.push_back({country.first, yearIt->second});
            }
        }
    }
    
    // Sort by population (descending)
    std::sort(results.begin(), results.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return results;
}

void PopulationData::performComprehensiveAnalysis(bool useParallel) {
    std::cout << "\n=== COMPREHENSIVE POPULATION ANALYSIS ===" << std::endl;
    std::cout << "Mode: " << (useParallel ? "PARALLEL" : "SINGLE-THREADED") << std::endl;
    std::cout << "Countries loaded: " << countryData.size() << std::endl;
    
    // Analysis 1: Top 10 countries by population in 2020
    std::cout << "\n1. Top 10 Countries by Population (2020):" << std::endl;
    auto topCountries = getTopCountriesByPopulation(2020, 10, useParallel);
    for (size_t i = 0; i < topCountries.size(); ++i) {
        std::string countryName = getCountryName(topCountries[i].first);
        std::cout << std::setw(2) << std::right << (i + 1) << ". " 
                  << std::setw(30) << std::left << countryName 
                  << " (" << topCountries[i].first << "): " 
                  << std::setw(15) << std::right << topCountries[i].second << std::endl;
    }
    
    // Analysis 2: Global population growth from 1960 to 2020
    std::cout << "\n2. Global Population Growth (1960-2020):" << std::endl;
    double globalGrowth = calculateGlobalPopulationGrowth(1960, 2020, useParallel);
    std::cout << "Growth rate: " << std::fixed << std::setprecision(2) << globalGrowth << "%" << std::endl;
    
    // Analysis 3: Countries with population above 100 million in 2020
    std::cout << "\n3. Countries with Population > 100 Million (2020):" << std::endl;
    auto largeCountries = findCountriesWithPopulationAbove(100000000, 2020, useParallel);
    std::cout << "Found " << largeCountries.size() << " countries:" << std::endl;
    for (const auto& country : largeCountries) {
        std::string countryName = getCountryName(country.first);
        std::cout << "  " << std::setw(30) << std::left << countryName 
                  << " (" << country.first << "): " 
                  << std::setw(15) << std::right << country.second << std::endl;
    }
    
    // Analysis 4: Total world population in 2020
    std::cout << "\n4. Total World Population (2020):" << std::endl;
    long long worldPop = calculateTotalWorldPopulation(2020, useParallel);
    std::cout << "Total: " << std::setw(15) << std::right << worldPop << " people" << std::endl;
    
    // Analysis 5: Top 5 countries by growth rate (1960-2020)
    std::cout << "\n5. Top 5 Countries by Growth Rate (1960-2020):" << std::endl;
    auto growthRates = calculateCountryGrowthRates(1960, 2020, useParallel);
    for (size_t i = 0; i < std::min(5UL, growthRates.size()); ++i) {
        std::string countryName = getCountryName(growthRates[i].first);
        std::cout << std::setw(2) << std::right << (i + 1) << ". " 
                  << std::setw(30) << std::left << countryName 
                  << " (" << growthRates[i].first << "): " 
                  << std::setw(8) << std::right << std::fixed << std::setprecision(2) 
                  << growthRates[i].second << "%" << std::endl;
    }
}

// Pthread worker functions
void* PopulationData::threadWorkerTopCountries(void* arg) {
    ThreadDataTopCountries* data = static_cast<ThreadDataTopCountries*>(arg);
    std::vector<std::pair<std::string, long long>> localResults;
    
    for (size_t i = data->start; i < data->end; ++i) {
        auto yearIt = (*data->countryVector)[i].second.find(data->year);
        if (yearIt != (*data->countryVector)[i].second.end() && yearIt->second > 0) {
            localResults.push_back({(*data->countryVector)[i].first, yearIt->second});
        }
    }
    
    pthread_mutex_lock(&data->data->resultsMutex);
    data->results->insert(data->results->end(), localResults.begin(), localResults.end());
    pthread_mutex_unlock(&data->data->resultsMutex);
    
    return NULL;
}

void* PopulationData::threadWorkerGlobalGrowth(void* arg) {
    ThreadDataGlobalGrowth* data = static_cast<ThreadDataGlobalGrowth*>(arg);
    long long localStartPop = 0;
    long long localEndPop = 0;
    
    for (size_t i = data->start; i < data->end; ++i) {
        auto startIt = (*data->countryVector)[i].second.find(data->startYear);
        auto endIt = (*data->countryVector)[i].second.find(data->endYear);
        
        if (startIt != (*data->countryVector)[i].second.end() && startIt->second > 0) {
            localStartPop += startIt->second;
        }
        if (endIt != (*data->countryVector)[i].second.end() && endIt->second > 0) {
            localEndPop += endIt->second;
        }
    }
    
    pthread_mutex_lock(&data->data->resultsMutex);
    *data->startPopulation += localStartPop;
    *data->endPopulation += localEndPop;
    pthread_mutex_unlock(&data->data->resultsMutex);
    
    return NULL;
}

void* PopulationData::threadWorkerGrowthRates(void* arg) {
    ThreadDataGrowthRates* data = static_cast<ThreadDataGrowthRates*>(arg);
    std::vector<std::pair<std::string, double>> localResults;
    
    for (size_t i = data->start; i < data->end; ++i) {
        auto startIt = (*data->countryVector)[i].second.find(data->startYear);
        auto endIt = (*data->countryVector)[i].second.find(data->endYear);
        
        if (startIt != (*data->countryVector)[i].second.end() && endIt != (*data->countryVector)[i].second.end() && 
            startIt->second > 0 && endIt->second > 0) {
            double growthRate = ((double)(endIt->second - startIt->second) / startIt->second) * 100.0;
            localResults.push_back({(*data->countryVector)[i].first, growthRate});
        }
    }
    
    pthread_mutex_lock(&data->data->resultsMutex);
    data->results->insert(data->results->end(), localResults.begin(), localResults.end());
    pthread_mutex_unlock(&data->data->resultsMutex);
    
    return NULL;
}

void* PopulationData::threadWorkerWorldPopulation(void* arg) {
    ThreadDataWorldPopulation* data = static_cast<ThreadDataWorldPopulation*>(arg);
    long long localPop = 0;
    
    for (size_t i = data->start; i < data->end; ++i) {
        auto yearIt = (*data->countryVector)[i].second.find(data->year);
        if (yearIt != (*data->countryVector)[i].second.end() && yearIt->second > 0) {
            localPop += yearIt->second;
        }
    }
    
    pthread_mutex_lock(&data->data->resultsMutex);
    *data->totalPopulation += localPop;
    pthread_mutex_unlock(&data->data->resultsMutex);
    
    return NULL;
}

void* PopulationData::threadWorkerLargeCountries(void* arg) {
    ThreadDataLargeCountries* data = static_cast<ThreadDataLargeCountries*>(arg);
    std::vector<std::pair<std::string, long long>> localResults;
    
    for (size_t i = data->start; i < data->end; ++i) {
        auto yearIt = (*data->countryVector)[i].second.find(data->year);
        if (yearIt != (*data->countryVector)[i].second.end() && yearIt->second >= data->threshold) {
            localResults.push_back({(*data->countryVector)[i].first, yearIt->second});
        }
    }
    
    pthread_mutex_lock(&data->data->resultsMutex);
    data->results->insert(data->results->end(), localResults.begin(), localResults.end());
    pthread_mutex_unlock(&data->data->resultsMutex);
    
    return NULL;
}
