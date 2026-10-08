# MiniRedis

MiniRedis is a small Redis-like server written in C99.

The project is meant to practise the basics of network programming in C:
TCP sockets, a simple text protocol, linked lists and memory management.
It is an educational project. It is not meant to replace Redis.

## Features

- TCP server on a configurable port;
- small command-line client;
- `PING` and `QUIT` commands;
- `SET` and `GET` commands;
- `DEL` and `EXISTS` commands;
- `INCR` for integer values;
- values containing spaces;
- tests for the main commands;
- strict compilation flags and GitHub Actions configuration.

## Build

Requirements:

- a C99 compiler;
- a POSIX system such as Linux or macOS;
- `make`.

```sh
make
```

This creates two programs:

```text
miniredis-server
miniredis-cli
```

## Start the server

Use the default port `6379`:

```sh
./miniredis-server
```

Or choose another port:

```sh
./miniredis-server -p 6380
```

## Use the client

Open another terminal and send commands with the client:

```sh
printf 'PING\n' | ./miniredis-cli
printf 'SET name Cyril\nGET name\n' | ./miniredis-cli
```

Example output:

```text
PONG
OK
Cyril
```

The server keeps its data while it is running:

```sh
printf 'SET counter 10\nINCR counter\nGET counter\n' \
    | ./miniredis-cli
```

Output:

```text
OK
11
11
```

## Commands

| Command | Description |
| --- | --- |
| `PING` | Check that the server is responding |
| `SET key value` | Store a value |
| `GET key` | Read a value |
| `DEL key` | Delete a key |
| `EXISTS key` | Check whether a key exists |
| `INCR key` | Increase an integer value |
| `QUIT` | Close the current connection |

## Tests

```sh
make check
```

The test script starts a local server, checks the main commands and stops the
server at the end.

Formatting can be checked with:

```sh
make check-format
```

## Project structure

```text
.
├── Makefile
├── include/
│   ├── client.h
│   ├── network.h
│   ├── protocol.h
│   ├── server.h
│   └── store.h
├── src/
│   ├── client.c
│   ├── main_client.c
│   ├── main_server.c
│   ├── network.c
│   ├── protocol.c
│   ├── server.c
│   └── store.c
└── tests/
    └── test.sh
```

## Limitations

MiniRedis is intentionally small:

- data is kept only in memory;
- there is no authentication;
- there is no persistence file;
- one client is handled at a time;
- the protocol is a simple line-based protocol;
- it does not implement the real Redis protocol.

These limits keep the code readable and make the project easier to study.

## License

This project is distributed under the MIT License. See [LICENSE](LICENSE).
