# Learning Notes

Networking theory and socket-programming notes collected while building this
port scanner.

- [1. How a TCP full-open scan works](#1-how-a-tcp-full-open-scan-works)
- [2. What I need to do](#2-what-i-need-to-do)
- [3. Server socket programming](#3-server-socket-programming)
- [4. Client socket programming](#4-client-socket-programming)
- [5. Non-blocking connect + poll()](#5-non-blocking-connect--poll)
- [6. Problems encountered](#6-problems-encountered)
- [7. Ports reference](#7-ports-reference)

---

## 1. How a TCP full-open scan works

The handshake:

1. Send a SYN packet to a port.
2. The packet requests an ACK.

The response tells you the port's state:

- **SYN/ACK** → the port is **open**.
- **RST** → the port is **closed**.
- **No response** → can mean many things: no port, or the packet was filtered
  (e.g. by a firewall), etc.

## 2. What I need to do

1. Find out how to open a TCP connection.
2. Find out how to send a packet (the packet we want to send is the SYN request).
3. Find out how to get a response.

## 3. Server socket programming

1. Create the server socket:

   ```c++
   int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
   ```

   - `AF_INET`: IPv4
   - `SOCK_STREAM`: TCP

2. Define the server address:

   ```c++
   sockaddr_in serverAddress;
   serverAddress.sin_family = AF_INET;
   serverAddress.sin_port = htons(8080);
   serverAddress.sin_addr.s_addr = INADDR_ANY;
   ```

   - `sockaddr_in`: data type used to store the address of the socket.
   - `htons()`: converts the unsigned int from machine byte order to network byte order.
   - `INADDR_ANY`: used when we don't want to bind our socket to any particular
     IP and instead make it listen on all available IPs.

3. Bind the socket to the address:

   ```c++
   bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
   ```

4. Listen for incoming connections:

   ```c++
   listen(serverSocket, 5);
   ```

5. Accept a client connection:

   ```c++
   int clientSocket = accept(serverSocket, nullptr, nullptr);
   ```

6. Receive data from the client:

   ```c++
   char buffer[1024] = {0};
   recv(clientSocket, buffer, sizeof(buffer), 0);
   std::cout << "Message from client: " << buffer << std::endl;
   ```

7. Close the server socket:

   ```c++
   close(serverSocket);
   ```

## 4. Client socket programming

1. Create the client socket:

   ```c++
   int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
   ```

   - `AF_INET`: IPv4
   - `SOCK_STREAM`: TCP

2. Define the server address:

   ```c++
   sockaddr_in serverAddress;
   serverAddress.sin_family = AF_INET;
   serverAddress.sin_port = htons(8080);
   serverAddress.sin_addr.s_addr = INADDR_ANY;
   ```

   - `sockaddr_in`: data type used to store the address of the socket.
   - `htons()`: converts the unsigned int from machine byte order to network byte order.
   - `INADDR_ANY`: used when we don't want to bind our socket to any particular
     IP and instead make it listen on all available IPs.

3. Connect to a server:

   ```c++
   connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
   ```

4. Send data to a server:

   ```c++
   const char* message = "Hello, server!";
   send(clientSocket, message, strlen(message), 0);
   ```

5. Close the client socket:

   ```c++
   close(clientSocket);
   ```

## 5. Non-blocking connect + poll()

To scan many ports concurrently without threads, the scanner uses non-blocking
sockets and one `poll()` loop (see `src/PortScanner.cpp`).

1. Make the socket non-blocking so `connect()` returns immediately:

   ```c++
   int flags = fcntl(clientSocket, F_GETFL, 0);
   fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);
   ```

2. `connect()` on a non-blocking socket returns `-1` with `errno == EINPROGRESS`
   while the handshake continues in the background. That is **not** an error —
   the fd just needs to be watched.

3. Start every port's `connect()` first, then wait on all of them at once with
   `poll()` (requesting `POLLOUT`). `poll()` reports a socket as ready when its
   connect finishes (or fails).

4. Once ready, read the actual result with `getsockopt(SO_ERROR)`:

   ```c++
   int err = 0;
   socklen_t len = sizeof(err);
   getsockopt(clientSocket, SOL_SOCKET, SO_ERROR, &err, &len);
   // err == 0  -> connected (port open)
   // err != 0  -> connection failed
   ```

### Full pattern

Putting it together: start one non-blocking `connect()` per port, then monitor
them all with a single `poll()`.

1. Start a non-blocking connect for each port and keep the pending sockets:

   ```c++
   int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

   int flags = fcntl(clientSocket, F_GETFL, 0);
   fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);

   sockaddr_in serverAddress{};
   serverAddress.sin_family = AF_INET;
   serverAddress.sin_port = htons(port);
   inet_pton(AF_INET, ip.c_str(), &serverAddress.sin_addr);

   int res = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
   if (res == 0) {
       // connected instantly (e.g. localhost) -> port open
   } else if (errno == EINPROGRESS) {
       // handshake in progress -> watch this fd with poll()
   } else {
       // real failure -> close and drop it
   }
   ```

2. Build the `pollfd` array (request `POLLOUT` = "writable when connect finishes"):

   ```c++
   std::vector<pollfd> fds;
   fds.push_back(pollfd{clientSocket, POLLOUT, 0});
   ```

3. Wait on all of them at once, with a timeout in milliseconds:

   ```c++
   int ready = poll(fds.data(), fds.size(), timeoutMs);
   // ready < 0  -> error (retry on EINTR)
   // ready == 0 -> timed out, remaining sockets never answered
   // ready > 0  -> some connects finished
   ```

4. For each socket that became ready (`revents != 0`), check `SO_ERROR` to see
   if the connect actually succeeded, then close it:

   ```c++
   for (auto& p : fds) {
       if (p.revents == 0) continue;      // still pending
       int err = 0;
       socklen_t len = sizeof(err);
       getsockopt(p.fd, SOL_SOCKET, SO_ERROR, &err, &len);
       if (err == 0) {
           // port open
       }
       close(p.fd);
   }
   ```

`revents` may carry `POLLERR` / `POLLHUP` instead of `POLLOUT` when a connect
fails — that's fine, `getsockopt(SO_ERROR)` is what distinguishes success from
failure either way.

## 6. Problems encountered

**1. Segmentation fault: 11**

Means:
- The code wants to access a memory location but can't.
- A function wants to access a memory location it doesn't have access to.

In this project it came from accessing a `vector` from multiple threads. The
attempted fix was a mutex lock/unlock, which didn't fully solve it — the current
design avoids the issue entirely by being single-threaded (non-blocking connect
+ `poll()`).

**2. Linker failed**

- Check the Makefile — it usually means the build is missing something (e.g. I
  compiled `main` and `PortScanner` but not the `Cli` class).

## 7. Ports reference

The ports the scanner checks (see the `PortScanner` constructor for the exact
list in code):

| Port  | Service                    |
|-------|----------------------------|
| 20    | FTP Data                   |
| 21    | FTP                        |
| 22    | SSH                        |
| 23    | Telnet                     |
| 25    | SMTP                       |
| 53    | DNS                        |
| 67    | DHCP Server                |
| 68    | DHCP Client                |
| 69    | TFTP                       |
| 80    | HTTP                       |
| 110   | POP3                       |
| 111   | RPCbind                    |
| 123   | NTP                        |
| 135   | MS RPC                     |
| 137   | NetBIOS Name               |
| 138   | NetBIOS Datagram           |
| 139   | NetBIOS Session            |
| 143   | IMAP                       |
| 161   | SNMP                       |
| 162   | SNMP Trap                  |
| 179   | BGP                        |
| 389   | LDAP                       |
| 443   | HTTPS                      |
| 445   | SMB                        |
| 465   | SMTPS                      |
| 514   | Syslog                     |
| 587   | SMTP Submission            |
| 636   | LDAPS                      |
| 873   | rsync                      |
| 993   | IMAPS                      |
| 995   | POP3S                      |
| 1433  | Microsoft SQL Server       |
| 1521  | Oracle                     |
| 1723  | PPTP                       |
| 1883  | MQTT                       |
| 2049  | NFS                        |
| 2375  | Docker (insecure)          |
| 2376  | Docker TLS                 |
| 3000  | Node.js / Grafana          |
| 3306  | MySQL                      |
| 3389  | RDP                        |
| 5432  | PostgreSQL                 |
| 5601  | Kibana                     |
| 5672  | RabbitMQ                   |
| 5900  | VNC                        |
| 5985  | WinRM HTTP                 |
| 5986  | WinRM HTTPS                |
| 6379  | Redis                      |
| 6443  | Kubernetes API             |
| 7001  | WebLogic                   |
| 8080  | HTTP Alternate             |
| 8443  | HTTPS Alternate            |
| 9200  | Elasticsearch              |
| 9418  | Git                        |
| 10000 | Webmin                     |
| 11211 | Memcached                  |
| 27017 | MongoDB                    |
