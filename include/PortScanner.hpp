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
#include <fcntl.h>
#include <poll.h>


class PortScanner {
    std::string ip;

    std::vector<int> ports;
    std::vector<int> openPort;

public:
    PortScanner(std::string& ip);

    std::vector<int> run();

    /* Creates a non-blocking TCP socket and starts a connect().
       Returns the socket fd to monitor with poll(), or -1 if the attempt
       already finished (immediate success/failure). */
    int scanner(int port);

    /* Reads the completed connect() result via getsockopt(SO_ERROR).
       Returns true if the connection succeeded (port open). */
    bool isConnected(sockaddr_in& serverAddress, int& clientSocket);
};
