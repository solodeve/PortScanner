# NetScan — TCP Port Scanner

A small single-threaded TCP port scanner written in C++. It checks a fixed list
of 68 common ports on a target IPv4 address and reports which ones are open.

Instead of spawning a thread per port, it fires off all the connections at once
using **non-blocking sockets** and waits on them concurrently with a single
**`poll()`** loop.

> For the networking theory and socket-programming notes behind this project,
> see [LEARN.md](LEARN.md).

## Requirements

- A C++17 compiler (`g++` or `clang++`)
- `make`
- A POSIX system (Linux / macOS) — uses `sys/socket.h`, `poll.h`, `fcntl.h`

## Build

```bash
make
```

The binary is produced at `build/port_scanner`.

## Run

```bash
make run
```

or directly:

```bash
./build/port_scanner
```

You'll be prompted for a target IP, then the scanner reports the open ports and
how long the scan took:

```
NetScan v1

What ip do you want to scan? 127.0.0.1
Target:
127.0.0.1

Scanning ...

[OPEN]

22
80
443

Scan completed
Time: 12345 microsecond
Open Ports: 3
```

## How it works

The scan is a **TCP connect scan**: for each port it starts a real TCP
handshake and treats a successful connection as "open".

1. **`scanner(port)`** creates a socket, switches it to non-blocking with
   `fcntl()`, and calls `connect()`. On a non-blocking socket this returns
   immediately with `EINPROGRESS` while the handshake continues in the
   background. The socket file descriptor is handed back to be monitored.
2. **`run()`** starts *every* port's connection first, collects all the
   sockets into one `pollfd` array, then loops on a single `poll()` to watch
   them all at once (with a connection timeout).
3. **`isConnected()`** reads the result of a finished connection with
   `getsockopt(SO_ERROR)`: `0` means the port is open.

This gives thread-like concurrency without any threads.

## Project structure

```
.
├── main.cpp              # Entry point: wires the CLI to the scanner
├── include/
│   ├── Cli.hpp           # CLI prompts and result display
│   └── PortScanner.hpp   # Scanner interface
├── src/
│   ├── Cli.cpp
│   └── PortScanner.cpp   # Non-blocking connect + poll() scan logic
├── Makefile              # Auto-globs *.cpp and src/*.cpp
├── README.md
└── LEARN.md              # Networking / socket-programming notes
```

## Notes & limits

- Ports are a hardcoded list (see the `PortScanner` constructor). No CLI flag
  to override the set yet.
- Connection timeout is fixed at 10s in `run()`.
- A no-response port (firewalled/filtered) simply times out and is reported as
  closed — the scanner can't tell "filtered" from "closed".
