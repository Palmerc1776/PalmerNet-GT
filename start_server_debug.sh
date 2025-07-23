#!/bin/bash

# Debug Server Startup Script
echo "🚀 Starting Growtopia Server in Debug Mode..."
echo "============================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Stop any existing server processes
print_status "Stopping any existing server processes..."
pkill -f growtopia_server 2>/dev/null || true
sleep 2

# Check if binary exists
if [ ! -f "growtopia_server" ]; then
    print_error "Server binary not found. Building..."
    if [ -f "Makefile" ]; then
        make clean && make
    elif [ -f "build_ubuntu.sh" ]; then
        chmod +x build_ubuntu.sh && ./build_ubuntu.sh
    else
        print_error "No build system found!"
        exit 1
    fi
fi

# Make sure binary is executable
chmod +x growtopia_server

# Check config file
if [ ! -f "config.ini" ]; then
    print_warning "config.ini not found. Creating default..."
    cat > config.ini << EOF
[Server]
port=17091
max_clients=100
log_file=server.log
enable_file_logging=true

[Game]
server_name=Growtopia Private Server
motd=Welcome to our private server!
max_worlds=1000

[Security]
enable_authentication=false
admin_password=admin123
EOF
fi

# Clear old logs
if [ -f "server.log" ]; then
    print_status "Backing up old log..."
    mv server.log server.log.old
fi

print_status "Starting server with verbose output..."
print_status "Press Ctrl+C to stop the server"
print_status "Server logs will be saved to server.log"
echo "============================================="

# Start server and show output
./growtopia_server