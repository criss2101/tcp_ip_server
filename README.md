# TCP/IP Server and Simple Client

This project implements a simple TCP/IP server and a testing client for sending commands and data.

## Overview

The server listens for incoming connections from clients and processes the received payload based on the received command id.
The payload is checked for payload correctness. It cannot read more payload than it was specified. Check [Payload Structure](#payload-structure).
The server processes client data on separate threads. Max number of connections = 10.
The client connects to the server and sends commands. Both the client and the server supports SIGINT (ctrl+c) handling.

## Payload Structure

The payload consists of the following data:

1. **CommandID** - `int32_t`: The identifier for the command being sent.
2. **PayloadSize** - `int32_t`: The size of the payload data.
3. **Payload** - `int8_t[]`: The actual data being transmitted.
4. **Ending mark** - `int32_t`: The ending mark, (`0`) that indicates the end of the packet.

Currently supported `CommandID`s are:

- **STRING_OUT = 1**: Command that makes payload to be interpreted as string and returned.
- **DOUBLE_OUT = 2**: Command that makes payload to be interpreted as `uint64_t` divided by 1000 and returned as a floating-point number.

## How to Build

### To build and run the TCP/IP server:

1. Navigate to the `tcp_ip_server` directory.
2. Use `make clean` to clean up previous builds.
3. Use `make` to build the server.
4. If a rebuild is needed, repeat steps 2 and 3.
5. Start the server: ./tcp_ip_server

### To build and run the client:

1. Navigate to the `PrimitiveClient` directory.
2. Use `make clean` to clean up previous builds.
3. Use `make` to build the client.
4. If a rebuild is needed, repeat steps 2 and 3.
5. Start the client ./simple_client.

## Example Interaction

1. **Client Input**:
   ```
    Enter command number: 1
    Enter payload size: 4
    Enter payload data [string]: abcd
    Provide 0 to send ending mark:
    0
    Closing packet int32_t (0) has been sent.
   ```

2. **Server Processing**:
   ```
    New connection accepted. Client_fd: 5
    New data received from client_fd: 5
    Client_fd: 5 - header total_bytes_received: 4 < 8
    Client_fd: 5 - header total_bytes_received: 8 < 8
    Client_fd: 5 - received Command ID: 1, Payload Size: 4
    Client_fd: 5 - payload total_bytes_received: 4 < 4
    Client_fd: 5 - ending mark total_bytes_received: 4 < 4
    Data received, data processing start...
    OK: abcd
   ```


