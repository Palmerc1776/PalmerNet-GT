# Growtopia Private Server

A cross-platform Growtopia private server implementation written in C++.

## Features

- Cross-platform support (Windows & Ubuntu/Linux)
- Multi-threaded client handling
- Growtopia protocol implementation (basic)
- String and update packet handling
- Player login and world join system
- Chat message broadcasting
- Logging system with file output and console colors
- Modular architecture
- Test client for debugging

## Building

### Prerequisites

#### Ubuntu/Linux:
```bash
sudo apt update
sudo apt install build-essential g++ make cmake
```

#### Windows:
- Install Visual Studio with C++ support, or
- Install MSYS2/MinGW-w64 from https://www.msys2.org/

### Build Options

#### Option 1: Using Make
```bash
# Build the server
make

# Clean build files
make clean

# Build debug version
make debug

# Run the server
make run

# Build test client
make test_client
```

#### Option 2: Using CMake
```bash
# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build the project
cmake --build .

# Run the server
./bin/growtopia_server
```

#### Windows (Visual Studio)
```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

## Running the Server

The server will start on port 17091 (default Growtopia port).

```bash
./growtopia_server
```

## Testing the Server

Build and run the test client to verify the server works:

```bash
# Build test client
make test_client

# Run server in one terminal
./growtopia_server

# Run test client in another terminal
./test_client  # Linux
# or
test_client.exe  # Windows
```

The test client will:
1. Connect to the server
2. Send a login request
3. Join a world
4. Send a chat message
5. Disconnect

## Configuration

Currently, the server uses hardcoded settings. Future versions will include:
- Configuration files
- Database integration
- World management
- Item system
- Player authentication

## Architecture

```
├── main.cpp              # Entry point
├── server/
│   ├── Server.h/cpp      # Main server class
│   └── Client.h/cpp      # Client connection handling
├── utils/
│   └── Logger.h/cpp      # Logging system
└── Makefile/CMakeLists.txt # Build systems
```

## Development

This is a basic foundation for a Growtopia private server. To extend it:

1. Add packet parsing for Growtopia protocol
2. Implement world system
3. Add item database
4. Create player authentication
5. Add game mechanics

## License

This project is for educational purposes only.