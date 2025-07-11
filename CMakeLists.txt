cmake_minimum_required(VERSION 3.20)
project(P2PFileExchange)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

# Include directories
include_directories(src)
include_directories(src/common)
include_directories(src/common/dto)
include_directories(src/index)
include_directories(src/logger)
include_directories(src/peer)
include_directories(src/peer/client)
include_directories(src/peer/server)
include_directories(src/peer/server/hash)

# Index Server Executable
add_executable(index_server
    src/index/main.cpp
    src/logger/ConsoleLogger.cpp
)

# Peer Executable
add_executable(peer
    src/peer/main.cpp
    src/logger/ConsoleLogger.cpp
)

target_link_libraries(index_server m)
target_link_libraries(peer m)