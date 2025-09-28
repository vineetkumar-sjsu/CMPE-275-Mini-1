#include "PopulationData.h"
#include <algorithm>
#include <iomanip>

PopulationData::PopulationData() {
    // Initialize available years (1960-2023)
    for (int year = 1960; year <= 2023; ++year) {
        availableYears.push_back(year);
    }
}

PopulationData::~PopulationData() {
    // Destructor - no dynamic memory to clean up
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
    int lineCount = 0;
    
    while (std::getline(file, line)) {
        lineCount++;
        
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
        for (int i = 4; i < fields.size() && i < 68; ++i) {
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
