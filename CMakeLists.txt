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
include_directories(src/peer/orchestrator)

# Index Server Executable
add_executable(index_server
    src/index/main.cpp
    src/logger/ConsoleLogger.cpp
)

# Find GTK3 for GUI
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Peer Executable
add_executable(peer
    src/peer/main.cpp
    src/peer/ui/PeerGUI.cpp
    src/logger/ConsoleLogger.cpp
    src/peer/indexer/FileIndexer.cpp
    src/peer/hasher/FileHasher.cpp
    src/peer/orchestrator/Orchestrator.cpp
)

target_link_libraries(index_server m)
target_include_directories(peer PRIVATE ${GTK3_INCLUDE_DIRS})
target_compile_options(peer PRIVATE ${GTK3_CFLAGS_OTHER})
target_link_directories(peer PRIVATE ${GTK3_LIBRARY_DIRS})
target_link_libraries(peer ${GTK3_LIBRARIES} m)
target_compile_options(peer PRIVATE -Wno-unused-parameter)