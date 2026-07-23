# Port Scanner 

--- TCP half open ---

1. send a SYN packet to a port
2. the packet request an ACK 

--- Response --- 

If there is a SYN/ACK response - there is a port open.

If there is any RST - the port is closed 

If thre is no response - can mean a lot of thinks: no port, the packet was filtered (for example: by a firewall), ...

--- What do I need to do? ---

1. find out how to open a tcp connection
2. find out how to send a packet (I think the packet we want to send is the SYN request)
3. find out how to get a response


--- Server Socket Programmin ---

1. creating the server socket: 

```c++
int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
```

- AF_INET: iIPv4
- SOCK_STREAM: TCP


2. Defining Server Adress:

```c++
sockaddr_in serverAddress;
serverAddress.sin_family = AF_INET;
serverAddress.sin_port = htons(8080);
serverAddress.sin_addr.s_addr = INADDR_ANY;
```

- sockaddr_in: data type that is used to store the address of the socket.
- htons(): convert the unsigned int from machine byte order to network byte order.
- INADDR_ANY: It is used when we don't want to bind our socket to any particular IP and instead make it listen to all the available IPs.

3. Bind Socket to Address

```c++ 
bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
```

4. Listening for incomming connection

```c++
listen(serverSocket, 5);
```

5. Accept Client Connection

```c++
int clientSocket = accept(serverSocket, nullptr, nullptr);
```

6. Receive data from Client

```c++
char buffer[1024] = {0};
recv(clientSocket, buffer, sizeof(buffer), 0);
cout << "Message from client: " << buffer << endl;
```

7. close server socket

```c++
close(serverSocket);
```

--- Client Socket Programmin ---

1. creating the client socket: 

```c++
int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
```

- AF_INET: iIPv4
- SOCK_STREAM: TCP


2. Defining Server Adress:

```c++
sockaddr_in serverAddress;
serverAddress.sin_family = AF_INET;
serverAddress.sin_port = htons(8080);
serverAddress.sin_addr.s_addr = INADDR_ANY;
```

- sockaddr_in: data type that is used to store the address of the socket.
- htons(): convert the unsigned int from machine byte order to network byte order.
- INADDR_ANY: It is used when we don't want to bind our socket to any particular IP and instead make it listen to all the available IPs.

3. Connect to a server

```c++ 
connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
```

4. send data to a server

```c++
const char* message = "Hello, server!";
send(clientSocket, message, strlen(message), 0);
```

5. close client socket

```c++
close(clientocket);
```

--- version 2 ---

DONE -> 1. make a better output
```txt 
NetScan v1.0

Target:
192.168.1.20

Scanning 68 ports...

[OPEN]

22     SSH
80     HTTP
443    HTTPS

Scan completed:
Time: 1.42s
Open ports: 3
```

DONE -> 2. use thread

DONE -> 3. refactor the code so it respect the SWE standard

- DONE -> each class need to have only one role (for exemple: output in PortScanner -> need an output class)
- DONE -> make it so the scanner do not run via PortScanner constructor
- DONE -> check if all variable are where they need to be (for example: clientSocket -> in the function not as an attribute)
- DONE -> recheck the naming for functions and variables

4. save it into a json file

5. let the user choose the port, they want to check (make a range - for example: 1-1000

6. using non-blocking connect 

7. make it so we use a poll of thread so that we don't have 1000 threads

DONE -> 8. make clientSocket a LOCAL var in creatingSocket, not a member (68 threads share one fd = race = UB every run)

9. add a connect timeout (blocking connect hangs ~75s on a firewalled port -> looks frozen)

10. split closed vs filtered via errno (ECONNREFUSED = closed, timeout/no answer = filtered/firewall)

11. delete closedPort: every thread locks and fills it but nothing ever reads it

12. check inet_pton return (!= 1): bad input like "hello" scans uninitialized memory

13. banner grab: recv() once on an open port to print WHAT is listening, not just that it's open // don't think i need this one

14. resolve hostnames with getaddrinfo so you can scan example.com, not only dotted IPs

15. use lock_guard instead of manual mtx->lock()/unlock() (throw between them = deadlock)

16. add #include <chrono> and <memory> to the header (only compiles by luck via transitive includes)

17. (stretch) SYN scan with raw sockets: send SYN, read RST/SYN-ACK, never finish the handshake (needs root)