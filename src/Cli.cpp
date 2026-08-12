#include "Cli.hpp"

Cli::Cli() {}

std::string Cli::promptTargetIp() {
    std::string ip;

    std::cout << "NetScan v1\n\n" << "What ip do you want to scan? ";
    std::cin >> ip;

    return ip;
}

void Cli::displayScanStart(std::string ip) {
    std::cout << "Target:" << std::endl;
    std::cout << ip << "\n" <<std:: endl;

    std::cout << "Scanning ...\n" << std::endl;
}

void Cli::displayScanResults(std::vector<int> openPort, std::vector<int> filteredPort, size_t closedCount, std::chrono::microseconds duration) {
    std::cout << "[OPEN]\n" << std::endl;
    for (int port: openPort) {
        std::cout << port << std::endl;
    } std::cout << std::endl;

    std::cout << "[FILTERED] (no response / firewall)\n" << std::endl;
    for (int port: filteredPort) {
        std::cout << port << std::endl;
    } std::cout << std::endl;

    std::cout << "Scan completed" << std::endl;
    std::cout << "Time: " << duration.count() << " microsecond" << std::endl;
    std::cout << "Open: " << openPort.size() << "  Closed: " << closedCount << "  Filtered: " << filteredPort.size() << std::endl;
}