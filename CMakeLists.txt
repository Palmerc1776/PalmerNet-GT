cmake_minimum_required(VERSION 3.10)
project(GrowtopiaServer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Source files
set(SOURCES
    main.cpp
    server/Server.cpp
    server/Client.cpp
    utils/Logger.cpp
    protocol/Packet.cpp
)

# Create executable
add_executable(growtopia_server ${SOURCES})

# Platform-specific libraries
if(WIN32)
    target_link_libraries(growtopia_server ws2_32)
else()
    target_link_libraries(growtopia_server pthread)
endif()

# Include directories
target_include_directories(growtopia_server PRIVATE .)

# Set output directory
set_target_properties(growtopia_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Debug configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(growtopia_server PRIVATE DEBUG)
endif()