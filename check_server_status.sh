#!/bin/bash

# Server Status Check Script
echo "🔍 Checking Growtopia Server Status..."
echo "=================================="

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

print_step() {
    echo -e "${BLUE}[CHECK]${NC} $1"
}

# Check if server binary exists
print_step "1. Checking server binary..."
if [ -f "growtopia_server" ]; then
    print_status "✅ Server binary found: growtopia_server"
else
    print_error "❌ Server binary not found: growtopia_server"
    echo "   Try building with: make or ./build_ubuntu.sh"
fi

# Check if server is running
print_step "2. Checking if server is running..."
if pgrep -f "growtopia_server" > /dev/null; then
    PID=$(pgrep -f "growtopia_server")
    print_status "✅ Server is running (PID: $PID)"
else
    print_warning "⚠️  Server is not running"
fi

# Check systemd service status
print_step "3. Checking systemd service..."
if systemctl list-unit-files | grep -q "growtopia-server.service"; then
    SERVICE_STATUS=$(systemctl is-active growtopia-server 2>/dev/null || echo "inactive")
    if [ "$SERVICE_STATUS" = "active" ]; then
        print_status "✅ Systemd service is active"
    else
        print_warning "⚠️  Systemd service is $SERVICE_STATUS"
    fi
else
    print_warning "⚠️  Systemd service not configured"
fi

# Check if port 17091 is listening
print_step "4. Checking port 17091..."
if netstat -ln | grep -q ":17091"; then
    print_status "✅ Port 17091 is listening"
    netstat -ln | grep ":17091"
else
    print_error "❌ Port 17091 is not listening"
fi

# Check recent logs
print_step "5. Checking recent logs..."
if [ -f "server.log" ]; then
    print_status "📋 Last 10 lines of server.log:"
    echo "----------------------------------------"
    tail -10 server.log
    echo "----------------------------------------"
else
    print_warning "⚠️  No server.log file found"
fi

# Check systemd logs if service exists
if systemctl list-unit-files | grep -q "growtopia-server.service"; then
    print_step "6. Checking systemd logs..."
    print_status "📋 Last 5 systemd log entries:"
    echo "----------------------------------------"
    sudo journalctl -u growtopia-server -n 5 --no-pager
    echo "----------------------------------------"
fi

echo ""
echo "🔧 Quick Commands:"
echo "   - Start server manually: ./growtopia_server"
echo "   - View live logs: tail -f server.log"
echo "   - Check systemd logs: sudo journalctl -u growtopia-server -f"
echo "   - Kill server process: pkill -f growtopia_server"
echo "   - Check port usage: netstat -tlnp | grep 17091"