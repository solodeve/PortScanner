#include <iostream>
#include <algorithm>
#include <chrono>

#include "PortScanner.hpp"
#include "Cli.hpp"


using namespace std;

int main() {
    Cli cli{};

    string ip = cli.promptTargetIp();
    cli.displayScanStart(ip);

    auto start = chrono::high_resolution_clock::now();
 
    PortScanner portScanner{ip};
    vector<int> openPort = portScanner.run();

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    cli.displayScanResults(openPort, duration);
    return 0;
}