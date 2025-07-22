#!/bin/bash

echo "Building Growtopia Server for Ubuntu/Linux..."

# Check if g++ is installed
if ! command -v g++ &> /dev/null; then
    echo "g++ not found. Installing build tools..."
    sudo apt update
    sudo apt install -y build-essential g++ make
fi

# Create obj directories
mkdir -p obj/server
mkdir -p obj/utils
mkdir -p obj/protocol

# Compile source files
echo "Compiling main.cpp..."
g++ -std=c++17 -Wall -Wextra -O2 -c main.cpp -o obj/main.o

echo "Compiling Server.cpp..."
g++ -std=c++17 -Wall -Wextra -O2 -c server/Server.cpp -o obj/server/Server.o

echo "Compiling Client.cpp..."
g++ -std=c++17 -Wall -Wextra -O2 -c server/Client.cpp -o obj/server/Client.o

echo "Compiling Logger.cpp..."
g++ -std=c++17 -Wall -Wextra -O2 -c utils/Logger.cpp -o obj/utils/Logger.o

echo "Compiling Packet.cpp..."
g++ -std=c++17 -Wall -Wextra -O2 -c protocol/Packet.cpp -o obj/protocol/Packet.o

# Link executable
echo "Linking executable..."
g++ obj/main.o obj/server/Server.o obj/server/Client.o obj/utils/Logger.o obj/protocol/Packet.o -o growtopia_server -lpthread

if [ $? -eq 0 ]; then
    echo "Build successful! Run ./growtopia_server to start the server."
    chmod +x growtopia_server
else
    echo "Build failed!"
    exit 1
fi