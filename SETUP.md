# Setup Guide for Growtopia Private Server

## Windows Setup

### Option 1: Visual Studio (Recommended)
1. Download and install Visual Studio Community (free) from: https://visualstudio.microsoft.com/
2. During installation, select "Desktop development with C++"
3. Open Command Prompt or PowerShell in the project directory
4. Run: `build_windows.bat`

### Option 2: MSYS2/MinGW-w64
1. Download and install MSYS2 from: https://www.msys2.org/
2. Open MSYS2 terminal and run:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make
   ```
3. Add MinGW-w64 bin directory to your PATH
4. Run: `make` in the project directory

### Option 3: WSL (Windows Subsystem for Linux)
1. Install WSL2 with Ubuntu
2. Follow the Ubuntu setup instructions below

## Ubuntu/Linux Setup

1. Install build tools:
   ```bash
   sudo apt update
   sudo apt install build-essential g++ make cmake
   ```

2. Build the server:
   ```bash
   # Using Make
   make
   
   # Or using the build script
   chmod +x build_ubuntu.sh
   ./build_ubuntu.sh
   
   # Or using CMake
   mkdir build && cd build
   cmake ..
   make
   ```

3. Run the server:
   ```bash
   ./growtopia_server
   ```

## Testing the Server

Once built and running, you can test the server by:

1. The server will start on port 17091
2. You can connect using telnet for basic testing:
   ```bash
   telnet localhost 17091
   ```
3. Or modify a Growtopia client to connect to your server

## Next Steps

After getting the basic server running, you can:

1. Implement Growtopia packet protocol
2. Add world management system
3. Create item database
4. Add player authentication
5. Implement game mechanics

## Troubleshooting

### Windows Issues:
- If you get "Visual Studio not found", install Visual Studio with C++ support
- If you get linker errors, make sure ws2_32.lib is available
- Try running as Administrator if you get permission errors

### Linux Issues:
- If you get "g++ not found", run: `sudo apt install build-essential`
- If you get permission errors, make sure the build script is executable: `chmod +x build_ubuntu.sh`
- For network binding issues, you might need to run with sudo or change the port

### General Issues:
- Make sure port 17091 is not in use by another application
- Check firewall settings if clients can't connect
- Enable file logging in config.ini for debugging