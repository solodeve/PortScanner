#include "PortScanner.hpp"

PortScanner::PortScanner(std::string ip) {
    this->ports = {
        20, 21, 22, 23, 25, 53, 67, 68, 69, 80, 110, 111, 123, 135, 137, 138,
        139, 143, 161, 162, 179, 389, 443, 445, 465, 514, 587, 636, 873, 993,
        995, 1433, 1521, 1723, 1883, 2049, 2375, 2376, 3000, 3306, 3389, 4000,
        5000, 5432, 5601, 5672, 5900, 5985, 5986, 6379, 6443, 7001, 8000, 8008,
        8080, 8081, 8088, 8443, 8888, 9000, 9090, 9200, 9300, 9418, 10000,
        11211, 27017, 50000
    };

    this->ip = ip;
    this->mtx = std::make_shared<std::mutex>();
}

std::vector<int> PortScanner::run() {
    std::vector<std::thread> threads((size_t) ports.size());
    for (int i = 0; i < (size_t) ports.size(); i++) {
        threads[i] = std::thread{&PortScanner::scanner, this, ports[i]}; 
    }
    
    // Wait for all the thread to finish
    for (int i = 0; i < (size_t) ports.size(); i++) {
        threads[i].join();
    }

    return openPort;
}

// this is the work of a thread (this is the function to call for all thread)
void PortScanner::scanner(int port) {
    // Create an IPv4 TCP socket.
    int clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);

    // Check if the socket creation failed
    if (clientSocket == -1) {
        perror("socket");
    }

    // Configure the destination server address.
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;   // IPv4
    serverAddress.sin_port = htons(port); // Target port in network byte order

    // Convert the IP address from text to binary format
    // why c_str: Convert the C++ std::string into a C-style string (const char*)
    //  * required by the inet_pton() socket function. All that, because the socket API are in C (and there is no std::string in C)
    inet_pton(AF_INET, ip.c_str(), &serverAddress.sin_addr);

    // Attempt to establish a connection to the target port
    if (isConnected(serverAddress, clientSocket)) {
        // lock the mutex when created and unlock when destroyed
        std::lock_guard<std::mutex> lck(*mtx);
        openPort.push_back(port);
    }

    // Close the socket after the connection attempt
    close(clientSocket);
}

bool PortScanner::isConnected(sockaddr_in serverAddress, int clientSocket) {
    // Try to establish a TCP connection.
    int res = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    if (res == 0) { // if res == 0 - the connection is established
        // the port is connected - it is open
        return true;
    } 
    return false;
}