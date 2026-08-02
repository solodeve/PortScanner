#pragma once

#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <thread>
#include <mutex>

class PortScanner { 
    const int MAX_THREADS = 20;

    std::string ip;
    std::shared_ptr<std::mutex> mtx;
    
    std::vector<int> ports;
    std::vector<int> openPort;

public:
    PortScanner(std::string ip);

    std::vector<int> run();

    /* Creates a TCP socket and attempts to connect to the specified port. */
    void scanner(int port);

    /* Attempts to connect to the specified server and reports whether the port is open. */
    bool isConnected(sockaddr_in serverAddress, int clientSocket);
};