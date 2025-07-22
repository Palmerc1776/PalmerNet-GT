@echo off
echo Building Growtopia Server for Windows...

REM Check if Visual Studio is available
where cl >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Visual Studio compiler not found. Trying to locate it...
    
    REM Try to find and setup Visual Studio environment
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else (
        echo Visual Studio not found. Please install Visual Studio with C++ support.
        echo Alternatively, install MSYS2/MinGW-w64 and use 'make' command.
        pause
        exit /b 1
    )
)

REM Create obj directories
if not exist obj mkdir obj
if not exist obj\server mkdir obj\server
if not exist obj\utils mkdir obj\utils
if not exist obj\protocol mkdir obj\protocol

REM Compile source files
echo Compiling main.cpp...
cl /c /EHsc /std:c++17 main.cpp /Fo:obj\main.obj

echo Compiling Server.cpp...
cl /c /EHsc /std:c++17 server\Server.cpp /Fo:obj\server\Server.obj

echo Compiling Client.cpp...
cl /c /EHsc /std:c++17 server\Client.cpp /Fo:obj\server\Client.obj

echo Compiling Logger.cpp...
cl /c /EHsc /std:c++17 utils\Logger.cpp /Fo:obj\utils\Logger.obj

echo Compiling Packet.cpp...
cl /c /EHsc /std:c++17 protocol\Packet.cpp /Fo:obj\protocol\Packet.obj

REM Link executable
echo Linking executable...
link obj\main.obj obj\server\Server.obj obj\server\Client.obj obj\utils\Logger.obj obj\protocol\Packet.obj ws2_32.lib /OUT:growtopia_server.exe

if %ERRORLEVEL% EQU 0 (
    echo Build successful! Run growtopia_server.exe to start the server.
) else (
    echo Build failed!
)

pause