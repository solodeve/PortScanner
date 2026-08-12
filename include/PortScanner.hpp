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
    std::vector<int> closedPort;   
    std::vector<int> filteredPort;  

public:
    PortScanner(std::string& ip);

    std::vector<int> run();

    const std::vector<int>& getClosed()   const { return closedPort; }
    const std::vector<int>& getFiltered() const { return filteredPort; }

    /* Creates a non-blocking TCP socket and starts a connect().
       Returns the socket fd to monitor with poll(), or -1 if the attempt
       already finished (immediate success/failure). */
    int scanner(int port);

    /* Reads the completed connect() result via getsockopt(SO_ERROR).
       Returns the connect() errno: 0 == open, ECONNREFUSED == closed,
       anything else == filtered/unreachable. */
    int connectError(sockaddr_in& serverAddress, int& clientSocket);
};
