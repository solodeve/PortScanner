# Port Scanner 

--- TCP full open ---

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