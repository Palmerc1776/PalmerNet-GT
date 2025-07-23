# Growtopia Server Troubleshooting Guide

## Quick Fix Steps

### 1. Deploy Updated Server to VPS

```bash
# Upload files to VPS
scp -r * your_username@137.220.62.28:~/growtopia-server/

# SSH into VPS
ssh your_username@137.220.62.28

# Navigate to server directory
cd ~/growtopia-server/

# Make scripts executable
chmod +x *.sh

# Check current status
./check_server_status.sh
```

### 2. Build and Start Server

```bash
# Stop any existing server
pkill -f growtopia_server

# Build the server
make clean && make
# OR if no Makefile:
chmod +x build_ubuntu.sh && ./build_ubuntu.sh

# Start server in debug mode
./start_server_debug.sh
```

### 3. Test Connection from Windows

```bash
# From your Windows machine, test the connection
python test_connection.py 137.220.62.28
```

## Common Issues and Solutions

### Issue 1: "getting server address" Error
**Cause**: Client can't establish initial connection
**Solutions**:
1. Ensure server is running on port 17091
2. Check firewall settings on VPS
3. Verify hosts file is correct

### Issue 2: Server Not Responding
**Cause**: Server not handling Growtopia protocol correctly
**Solutions**:
1. Check server logs: `tail -f server.log`
2. Ensure server sends proper handshake response
3. Verify packet format compatibility

### Issue 3: Connection Timeout
**Cause**: Network or protocol issues
**Solutions**:
1. Test basic TCP connection: `telnet 137.220.62.28 17091`
2. Check VPS firewall: `sudo ufw status`
3. Verify server is binding to all interfaces (0.0.0.0)

## VPS Firewall Configuration

```bash
# Allow port 17091
sudo ufw allow 17091/tcp

# Check firewall status
sudo ufw status

# If firewall is inactive, enable it
sudo ufw enable
```

## Server Logs Analysis

### Good Connection Log:
```
[INFO] New client connected from: YOUR_IP
[INFO] Processing string packet from YOUR_IP: requestedName|...
[INFO] Initial connection/login request from YOUR_IP
```

### Bad Connection Log:
```
[ERROR] Failed to accept client connection
# OR no connection attempts at all
```

## Testing Commands

### On VPS:
```bash
# Check if server is running
ps aux | grep growtopia_server

# Check port usage
netstat -tlnp | grep 17091

# View live logs
tail -f server.log

# Test local connection
telnet localhost 17091
```

### On Windows:
```bash
# Test DNS resolution
nslookup growtopia1.com

# Test connection
Test-NetConnection -ComputerName 137.220.62.28 -Port 17091

# Test with Python script
python test_connection.py
```

## Advanced Debugging

### 1. Packet Capture
```bash
# On VPS, capture packets on port 17091
sudo tcpdump -i any port 17091 -v
```

### 2. Detailed Server Logs
Add more logging to server code and rebuild:
```cpp
Logger::info("Client connected, waiting for data...");
Logger::info("Received " + std::to_string(packetData.size()) + " bytes");
```

### 3. Protocol Analysis
The Growtopia client expects specific packet formats. Key points:
- Initial connection must respond with proper handshake
- String packets must be null-terminated
- Update packets must follow exact binary format

## Next Steps if Still Not Working

1. **Verify Protocol Implementation**: Compare with working Growtopia server implementations
2. **Check Client Version**: Ensure your server supports the client version you're using
3. **Network Analysis**: Use Wireshark to analyze packet flow
4. **Alternative Approach**: Consider using existing Growtopia server frameworks

## Contact Information

If you need help with specific errors, provide:
1. Server logs (`server.log`)
2. Connection test results
3. VPS system information (`uname -a`)
4. Firewall status (`sudo ufw status`)