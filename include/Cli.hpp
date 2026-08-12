#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <chrono>

class Cli {
public:
    Cli();

    std::string promptTargetIp();
    void displayScanStart(std::string ip);
    void displayScanResults(std::vector<int> openPort, std::vector<int> filteredPort, size_t closedCount, std::chrono::microseconds duration);
};