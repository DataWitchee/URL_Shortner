# C++ URL Shortener

A lightweight URL shortener built entirely in **C++**, without relying on external web frameworks.

## Features

* URL shortening and redirection
* Custom short URL generation
* HTTP request/response handling
* TCP socket-based client-server communication
* Low-level socket programming
* URL storage and retrieval using SQLite
* Persistent database storage
* CMake-based build system
* Modular C++ architecture

## Tech Stack

* **C++**
* **TCP / Sockets**
* **HTTP**
* **SQLite**
* **CMake**

## Build

```bash
mkdir build
cd build
cmake ..
make
```

## Run

```bash
./url_shortener
```

The server listens for incoming HTTP requests and handles URL creation, storage, lookup, and redirection through TCP socket connections.

## Project Goal

The project was built from the ground up to understand **network programming, TCP sockets, HTTP communication, database integration, and C++ system-level development** without using a web framework.
