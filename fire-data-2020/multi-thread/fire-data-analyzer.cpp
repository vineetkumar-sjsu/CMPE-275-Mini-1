#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <algorithm>
#include <filesystem>
#include <iomanip>

#include "omp.h"

// Structure to represent a single air quality record
struct AirQualityRecord {
    double latitude;
    double longitude;
    std::string datetime;
    std::string parameter;
    double value;
    std::string unit;
    double rawConcentration;
    int aqi;
    int aqiCategory;
    std::string siteName;
    std::string agencyName;
    std::string siteId;
    std::string fullSiteId;
    
    // Extract date from datetime (YYYY-MM-DD)
    std::string getDate() const {
        return datetime.substr(0, 10);
    }
    
    // Extract hour from datetime
    int getHour() const {
        std::string hourStr = datetime.substr(11, 2);
        return std::stoi(hourStr);
    }
};

class FireDataAnalyzer {
private:
    std::vector<AirQualityRecord> records;
    
    // Helper function to remove quotes and clean string
    std::string cleanString(const std::string& str) {
        std::string cleaned = str;
        if (!cleaned.empty() && cleaned.front() == '"') {
            cleaned.erase(0, 1);
        }
        if (!cleaned.empty() && cleaned.back() == '"') {
            cleaned.pop_back();
        }
        return cleaned;
    }
    
    // Parse a single CSV line
    std::vector<std::string> parseCSVLine(const std::string& line) {
        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string field;
        bool inQuotes = false;
        std::string currentField;
        
        // Remove problematic parallel for - character parsing must be sequential
        for (char c : line) {
            if (c == '"') {
                inQuotes = !inQuotes;
                currentField += c;
            } else if (c == ',' && !inQuotes) {
                fields.push_back(cleanString(currentField));
                currentField.clear();
            } else {
                currentField += c;
            }
        }
        fields.push_back(cleanString(currentField));
        
        return fields;
    }
    
public:
    // Load all CSV files from the data directory
    void loadData(const std::string& dataDir) {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::cout << "Loading fire data from: " << dataDir << std::endl;
        
        try {
            // 1st potential candidate for OMP parallelization
            std::vector<std::string> files;
            for (const auto& entry : std::filesystem::recursive_directory_iterator(dataDir)) {
                if (entry.path().extension() == ".csv") {
                    files.push_back(entry.path().string());
                }
            }

            #pragma omp parallel for
            for (int i =0; i < files.size(); i++) {
                #pragma omp critical
                loadCSVFile(files[i]);
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
            return;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Loaded " << records.size() << " records in " 
                << duration.count() << " milliseconds" << std::endl;
    }
    
    // Load a single CSV file
    void loadCSVFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        std::vector<std::string> lines; 
        std::string line;
        int lineCount = 0;
        
        while (std::getline(file, line)) {
            lines.push_back(line);
            if (line.empty()) continue; 
        }

        std::vector<AirQualityRecord> localRecords((lines.size()));

        #pragma omp parallel for
        for (int i =0; i < lines.size(); i++) {
            std::vector<std::string> fields = parseCSVLine(lines[i]);
                if (fields.size() >= 13) {
                    AirQualityRecord record;
                    record.latitude = std::stod(fields[0]);
                    record.longitude = std::stod(fields[1]);
                    record.datetime = fields[2];
                    record.parameter = fields[3];
                    record.value = std::stod(fields[4]);
                    record.unit = fields[5];
                    record.rawConcentration = std::stod(fields[6]);
                    record.aqi = std::stoi(fields[7]);
                    record.aqiCategory = std::stoi(fields[8]);
                    record.siteName = fields[9];
                    record.agencyName = fields[10];
                    record.siteId = fields[11];
                    record.fullSiteId = fields[12];
                    
                    localRecords[i] = record;
        }
        }

        for (const auto& rec: localRecords){
            if (!rec.datetime.empty()) {
                records.push_back(rec);
            }
        }
        
        
        file.close();
    }
    
    // Query 1: Get AQI data for a specific date
    std::vector<AirQualityRecord> getAQIDataForDate(const std::string& targetDate) {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<AirQualityRecord> results;
        
        #pragma omp parallel for
        for (int i = 0; i < records.size(); i++) {
            // 3rd potential candidate for OMP parallelization
            if (records[i].getDate() == targetDate) {
                #pragma omp critical
                results.push_back(records[i]);
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Query completed in " << duration.count() << " microseconds" << std::endl;
        std::cout << "Found " << results.size() << " records for date: " << targetDate << std::endl;
        
        return results;
    }
    
    // Query 2: Get dates where AQI was above a threshold
    std::vector<std::string> getDaysWithAQIAbove(int threshold) {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::map<std::string, int> dateMaxAQI;
        
        // Find maximum AQI for each date
        for (const auto& record : records) {
            std::string date = record.getDate();
            if (dateMaxAQI.find(date) == dateMaxAQI.end() || record.aqi > dateMaxAQI[date]) {
                dateMaxAQI[date] = record.aqi;
            }
        }
        
        std::vector<std::string> results;
        for (const auto& pair : dateMaxAQI) {
            if (pair.second > threshold) {
                results.push_back(pair.first);
            }
        }
        
        std::sort(results.begin(), results.end());
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Query completed in " << duration.count() << " microseconds" << std::endl;
        std::cout << "Found " << results.size() << " days with AQI above " << threshold << std::endl;
        
        return results;
    }
    
    // Additional query: Get average AQI for a date
    double getAverageAQIForDate(const std::string& targetDate) {
        auto start = std::chrono::high_resolution_clock::now();
        
        double totalAQI = 0.0;
        int count = 0;
        
        for (const auto& record : records) {
            if (record.getDate() == targetDate) {
                totalAQI += record.aqi;
                count++;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Average AQI query completed in " << duration.count() << " microseconds" << std::endl;
        
        return count > 0 ? totalAQI / count : 0.0;
    }
    
    // Get statistics about the loaded data
    void printDataStatistics() {
        if (records.empty()) {
            std::cout << "No data loaded." << std::endl;
            return;
        }
        
        std::map<std::string, int> parameterCounts;
        std::map<std::string, int> dateCounts;
        int minAQI = records[0].aqi;
        int maxAQI = records[0].aqi;
        
        for (const auto& record : records) {
            parameterCounts[record.parameter]++;
            dateCounts[record.getDate()]++;
            minAQI = std::min(minAQI, record.aqi);
            maxAQI = std::max(maxAQI, record.aqi);
        }
        
        std::cout << "\n=== DATA STATISTICS ===" << std::endl;
        std::cout << "Total records: " << records.size() << std::endl;
        std::cout << "Date range: " << dateCounts.begin()->first << " to " 
                << dateCounts.rbegin()->first << std::endl;
        std::cout << "AQI range: " << minAQI << " to " << maxAQI << std::endl;
        std::cout << "Number of unique dates: " << dateCounts.size() << std::endl;
        
        std::cout << "\nParameter distribution:" << std::endl;
        for (const auto& pair : parameterCounts) {
            std::cout << "  " << pair.first << ": " << pair.second << " records" << std::endl;
        }
    }
};

int main() {
    std::cout << "=== Fire Data Analyzer ===" << std::endl;
    
    FireDataAnalyzer analyzer;
    
    // Load data from the data directory
    analyzer.loadData("data");
    
    // Print statistics
    analyzer.printDataStatistics();
    
    std::cout << "\n=== SAMPLE QUERIES ===" << std::endl;
    
    // Example Query 1: Get AQI data for a specific day
    std::cout << "\n1. Getting AQI data for 2020-08-15:" << std::endl;
    auto dayData = analyzer.getAQIDataForDate("2020-08-15");
    
    if (!dayData.empty()) {
        std::cout << "Sample records:" << std::endl;
        for (int i = 0; i < std::min(5, (int)dayData.size()); i++) {
            const auto& record = dayData[i];
            std::cout << "  " << record.siteName << " - AQI: " << record.aqi 
                    << " (" << record.parameter << ": " << record.value << " " 
                    << record.unit << ")" << std::endl;
        }
    }
    
    // Example Query 2: Get days where AQI was above 100
    std::cout << "\n2. Getting days with AQI above 100:" << std::endl;
    auto highAQIDays = analyzer.getDaysWithAQIAbove(100);
    
    std::cout << "Days with high AQI:" << std::endl;
    for (const auto& date : highAQIDays) {
        std::cout << "  " << date << std::endl;
    }
    
    // Example Query 3: Get average AQI for a specific date
    std::cout << "\n3. Getting average AQI for 2020-08-20:" << std::endl;
    double avgAQI = analyzer.getAverageAQIForDate("2020-08-20");
    std::cout << "Average AQI: " << std::fixed << std::setprecision(2) << avgAQI << std::endl;
    
    // Additional performance test with multiple queries
    std::cout << "\n=== PERFORMANCE TESTING ===" << std::endl;
    
    auto perfStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; i++) {
        analyzer.getAQIDataForDate("2020-08-15");
    }
    auto perfEnd = std::chrono::high_resolution_clock::now();
    auto perfDuration = std::chrono::duration_cast<std::chrono::microseconds>(perfEnd - perfStart);
    
    std::cout << "10 date queries took " << perfDuration.count() 
            << " microseconds (avg: " << perfDuration.count() / 10 << " μs per query)" << std::endl;
    
    return 0;
}
