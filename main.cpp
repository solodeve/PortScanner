#include <iostream>
#include <algorithm>
#include <chrono>

#include "PortScanner.hpp"
#include "Cli.hpp"

int main() {
    Cli cli{};

    std::string ip = cli.promptTargetIp();
    cli.displayScanStart(ip);

    auto start = std::chrono::high_resolution_clock::now();
 
    PortScanner portScanner{ip};
    std::vector<int> openPort = portScanner.run();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    cli.displayScanResults(openPort, duration);
    return 0;
}