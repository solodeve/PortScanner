#pragma once

#include <string>
#include <iostream>
#include <vector>

class Cli {
public:
    Cli();

    std::string promptTargetIp();
    void displayScanStart(std::string ip);
    void displayScanResults(std::vector<int> openPort, std::chrono::microseconds duration);
};