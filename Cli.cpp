#include "Cli.hpp"

using namespace std;

Cli::Cli() {}

string Cli::promptTargetIp() {
    string ip;

    cout << "NetScan v1\n\n" << "What ip do you want to scan? ";
    cin >> ip;

    return ip;
}

void Cli::displayScanStart(string ip) {
    cout << "Target:" << endl;
    cout << ip << "\n" << endl;

    cout << "Scanning ...\n" << endl;
}

void Cli::displayScanResults(vector<int> openPort, chrono::microseconds duration) {
    cout << "[OPEN]\n" << endl;
    for (int port: openPort) {
        cout << port << endl;
    } cout << endl;

    cout << "Scan completed" << endl;
    cout << "Time: " << duration.count() << " microsecond" << endl;
    cout << "Open Ports: " << openPort.size() << endl;
}