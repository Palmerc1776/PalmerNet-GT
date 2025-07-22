#!/bin/bash

# Growtopia Server Deployment Script
# This script pulls the latest code from GitHub and rebuilds the server

echo "🚀 Starting Growtopia Server deployment..."

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

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

# Backup current config if it exists
if [ -f "config.ini" ]; then
    print_status "Backing up current config.ini..."
    cp config.ini config.ini.backup
fi

# Stop the server if it's running (you can customize this based on how you run your server)
print_status "Checking for running server processes..."
if pgrep -f "growtopia_server" > /dev/null; then
    print_warning "Server is running. Stopping it..."
    pkill -f "growtopia_server"
    sleep 2
fi

# Pull latest changes from GitHub
print_status "Pulling latest changes from GitHub..."
git fetch origin

# Check if there are any changes
LOCAL=$(git rev-parse HEAD)
REMOTE=$(git rev-parse origin/main)

if [ "$LOCAL" = "$REMOTE" ]; then
    print_status "Already up to date. No changes to pull."
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
    else
        print_error "Failed to pull changes. Please check for conflicts."
        exit 1
    fi
fi

# Restore config backup if it was backed up
if [ -f "config.ini.backup" ]; then
    print_status "Restoring config.ini backup..."
    mv config.ini.backup config.ini
fi

# Build the project
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

# Optional: Start the server (uncomment and modify as needed)
# print_status "Starting the server..."
# nohup ./growtopia_server > server.log 2>&1 &
# print_status "Server started in background. Check server.log for output."

print_status "✅ Deployment completed successfully!"
print_status "📝 Don't forget to:"
print_status "   - Check your config.ini settings"
print_status "   - Review any stashed changes with 'git stash list'"
print_status "   - Start your server if it's not auto-starting"

echo ""
echo "🔧 Useful commands:"
echo "   - View stashed changes: git stash list"
echo "   - Apply stashed changes: git stash pop"
echo "   - Check server status: ps aux | grep growtopia_server"
echo "   - View server logs: tail -f server.log"