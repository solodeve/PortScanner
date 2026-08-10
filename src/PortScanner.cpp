#include "PortScanner.hpp"
#include <chrono>
#include <unordered_map>

PortScanner::PortScanner(std::string& ip) {
    this->ports = {
        20, 21, 22, 23, 25, 53, 67, 68, 69, 80, 110, 111, 123, 135, 137, 138,
        139, 143, 161, 162, 179, 389, 443, 445, 465, 514, 587, 636, 873, 993,
        995, 1433, 1521, 1723, 1883, 2049, 2375, 2376, 3000, 3306, 3389, 4000,
        5000, 5432, 5601, 5672, 5900, 5985, 5986, 6379, 6443, 7001, 8000, 8008,
        8080, 8081, 8088, 8443, 8888, 9000, 9090, 9200, 9300, 9418, 10000,
        11211, 27017, 50000
    };

    this->ip = ip;
}

std::vector<int> PortScanner::run() {
    // Start every connect() first, then monitor them all in one poll() loop.
    std::vector<pollfd> fds;
    std::unordered_map<int, int> fdToPort;

    for (int port : ports) {
        int fd = scanner(port);
        
        // still connecting -> watch it
        if (fd >= 0) {                       
            fds.push_back(pollfd{fd, POLLOUT, 0});
            fdToPort[fd] = port;
        }
    }

    // ponytail: fixed 10s connect timeout
    const int timeoutMs = 10000;
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);

    while (!fds.empty()) {
        int remaining = (int) std::chrono::duration_cast<std::chrono::milliseconds>(
            deadline - std::chrono::steady_clock::now()).count();
        if (remaining <= 0) break;           // overall timeout reached

        int ready = poll(fds.data(), fds.size(), remaining);
        if (ready < 0) {
            if (errno == EINTR) continue;
            perror("poll");
            break;
        }

        // poll timed out
        if (ready == 0) { break; }               

        // Reap every socket that finished connecting (POLLOUT, or POLLERR/HUP).
        for (size_t i = 0; i < fds.size();) {
            if (fds[i].revents == 0) { i++; continue; }

            int fd = fds[i].fd;
            sockaddr_in unused{};
            if (isConnected(unused, fd)) {
                openPort.push_back(fdToPort[fd]);
            }
            close(fd);
            fdToPort.erase(fd);
            fds.erase(fds.begin() + i);
        }
    }

    // Sockets that never completed within the timeout: treat as closed.
    for (pollfd& p : fds) close(p.fd);

    return openPort;
}

int PortScanner::scanner(int port) {
    // Create an IPv4 TCP socket.
    int clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == -1) {
        perror("socket");
        return -1;
    }

    // Set socket to non-blocking so connect() returns immediately.
    int flags = fcntl(clientSocket, F_GETFL, 0);
    fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);

    // Configure the destination server address.
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;   // IPv4
    serverAddress.sin_port = htons(port); // Target port in network byte order
    inet_pton(AF_INET, ip.c_str(), &serverAddress.sin_addr);

    // Kick off the connection. On a non-blocking socket this usually returns -1
    // with errno == EINPROGRESS while the handshake proceeds in the background.
    int res = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // connected instantly (e.g. localhost)
    if (res == 0) {                     
        openPort.push_back(port);
        close(clientSocket);
        return -1;
    }

    // genuine failure, nothing to monitor
    if (errno != EINPROGRESS) {         
        close(clientSocket);
        return -1;
    }

    // pending: hand it to poll() in run()
    return clientSocket;                
}

bool PortScanner::isConnected(sockaddr_in& serverAddress, int& clientSocket) {
    // ponytail: kept for signature; state now read via getsockopt.
    (void) serverAddress; 

    // After poll() reports the socket is writable, SO_ERROR holds the
    // connect() result: 0 == success, otherwise the failure errno.
    int err = 0;
    socklen_t len = sizeof(err);
    if (getsockopt(clientSocket, SOL_SOCKET, SO_ERROR, &err, &len) < 0) {
        return false;
    }
    return err == 0;
}
