# P2P File Exchange
A memory-safe Napster-style P2P file sharing system built in C++, allowing users to locate and share files through a central index.

## Key Features
- **Parallel Downloads**: Download file chunks from multiple peers simultaneously  
- **GUI Interface**: GTK-based graphical user interface
- **Dual Hash System**: Reliable file identification and integrity verification
- **Auto Peer Discovery**: Heartbeat monitoring and dead peer removal

## Architecture
- **Index Server**: Central registry tracking files and peer locations
- **Peer Clients**: File sharing nodes with GUI interface
- **Streaming Protocol**: Memory-efficient chunked file transfers

## Quick Start

### 1. Start Index Server
```bash
# See index/README.md for compilation
cd build/
./index_server 8585
```

### 2. Start Peer Clients  
```bash
# See peer/README.md for compilation
cd build/
./peer_client 9001 9002 127.0.0.1 127.0.0.1 8585 ../assets/Peer1/
```

## Component Documentation
- [Index Server](src/index/README.md) - Central file registry
- [Peer Client](src/peer/README.md) - File sharing client with GUI
