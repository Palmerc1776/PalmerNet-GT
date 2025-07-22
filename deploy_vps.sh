#!/bin/bash

# Enhanced Growtopia Server Deployment Script for VPS
# This script handles systemd service management

echo "🚀 Starting PalmerNet-GT VPS deployment..."

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SERVICE_NAME="growtopia-server"
SERVER_BINARY="growtopia_server"

# Function to print colored output
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
    echo -e "${BLUE}[STEP]${NC} $1"
}

# Check if running as root (for systemctl commands)
check_sudo() {
    if ! sudo -n true 2>/dev/null; then
        print_warning "Some operations require sudo privileges. You may be prompted for your password."
    fi
}

# Check if git is installed
if ! command -v git &> /dev/null; then
    print_error "Git is not installed. Please install git first."
    exit 1
fi

# Check if we're in a git repository
if [ ! -d ".git" ]; then
    print_error "This is not a git repository. Please run this script from the project root."
    exit 1
fi

print_step "1/7 Checking system requirements..."
check_sudo

print_step "2/7 Backing up configuration..."
# Backup current config if it exists
if [ -f "config.ini" ]; then
    print_status "Backing up current config.ini..."
    cp config.ini config.ini.backup.$(date +%Y%m%d_%H%M%S)
fi

print_step "3/7 Stopping server service..."
# Stop the systemd service if it exists and is running
if systemctl is-active --quiet $SERVICE_NAME 2>/dev/null; then
    print_status "Stopping $SERVICE_NAME service..."
    sudo systemctl stop $SERVICE_NAME
    sleep 2
elif pgrep -f "$SERVER_BINARY" > /dev/null; then
    print_warning "Server process found running outside systemd. Stopping it..."
    pkill -f "$SERVER_BINARY"
    sleep 2
else
    print_status "Server is not currently running."
fi

print_step "4/7 Pulling latest changes from GitHub..."
# Pull latest changes from GitHub
git fetch origin

# Check if there are any changes
LOCAL=$(git rev-parse HEAD)
REMOTE=$(git rev-parse origin/main)

if [ "$LOCAL" = "$REMOTE" ]; then
    print_status "Already up to date. No changes to pull."
    UPDATED=false
else
    print_status "New changes found. Pulling updates..."
    
    # Stash any local changes
    if ! git diff-index --quiet HEAD --; then
        print_warning "Local changes detected. Stashing them..."
        git stash push -m "Auto-stash before deployment $(date)"
    fi
    
    # Pull the changes
    if git pull origin main; then
        print_status "Successfully pulled latest changes."
        UPDATED=true
    else
        print_error "Failed to pull changes. Please check for conflicts."
        exit 1
    fi
fi

print_step "5/7 Restoring configuration..."
# Restore config backup if it was backed up
if [ -f "config.ini.backup.$(date +%Y%m%d_%H%M%S)" ]; then
    print_status "Restoring config.ini..."
    # Use the most recent backup
    LATEST_BACKUP=$(ls -t config.ini.backup.* 2>/dev/null | head -n1)
    if [ -n "$LATEST_BACKUP" ]; then
        cp "$LATEST_BACKUP" config.ini
    fi
fi

print_step "6/7 Building the project..."
# Build the project only if there were updates
if [ "$UPDATED" = true ] || [ ! -f "$SERVER_BINARY" ]; then
    print_status "Building the project..."
    
    if [ -f "Makefile" ]; then
        if make clean && make; then
            print_status "Build successful!"
        else
            print_error "Build failed. Please check the build output."
            exit 1
        fi
    elif [ -f "build_ubuntu.sh" ]; then
        if chmod +x build_ubuntu.sh && ./build_ubuntu.sh; then
            print_status "Build successful!"
        else
            print_error "Build failed. Please check the build output."
            exit 1
        fi
    else
        print_warning "No build system found. Skipping build step."
    fi
else
    print_status "No updates found, skipping build."
fi

print_step "7/7 Starting server service..."
# Start the systemd service if it exists
if systemctl list-unit-files | grep -q "^$SERVICE_NAME.service"; then
    print_status "Starting $SERVICE_NAME service..."
    sudo systemctl start $SERVICE_NAME
    sleep 2
    
    if systemctl is-active --quiet $SERVICE_NAME; then
        print_status "✅ Service started successfully!"
    else
        print_error "❌ Failed to start service. Check logs with: sudo journalctl -u $SERVICE_NAME -f"
        exit 1
    fi
else
    print_warning "Systemd service not found. You can start the server manually:"
    print_warning "  ./$SERVER_BINARY"
fi

print_status "✅ Deployment completed successfully!"
print_status ""
print_status "📊 System Status:"
if systemctl list-unit-files | grep -q "^$SERVICE_NAME.service"; then
    SERVICE_STATUS=$(systemctl is-active $SERVICE_NAME 2>/dev/null || echo "inactive")
    print_status "   Service Status: $SERVICE_STATUS"
fi

if [ -f "$SERVER_BINARY" ]; then
    print_status "   Server Binary: ✅ Present"
else
    print_warning "   Server Binary: ❌ Missing"
fi

print_status ""
print_status "🔧 Useful commands:"
print_status "   - Check service status: sudo systemctl status $SERVICE_NAME"
print_status "   - View service logs: sudo journalctl -u $SERVICE_NAME -f"
print_status "   - Restart service: sudo systemctl restart $SERVICE_NAME"
print_status "   - Stop service: sudo systemctl stop $SERVICE_NAME"
print_status "   - View stashed changes: git stash list"
print_status "   - Apply stashed changes: git stash pop"

echo ""
echo "🎉 PalmerNet-GT deployment complete!"