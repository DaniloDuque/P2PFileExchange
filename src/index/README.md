# Index Server

The central index server that maintains a registry of all files available in the P2P network and tracks which peers have which files.

## How It Works

### Core Components
- **FileIndex**: Maps file descriptors to peer locations using hash-based file identification
- **HeartbeatManager**: Monitors peer health and removes dead peers automatically
- **IndexServer**: TCP server handling peer registration, file searches, and file location requests

### Operations
1. **Peer Registration**: Peers register with their shared files list
2. **File Search**: Returns list of files matching search query with peer locations
3. **File Location**: Provides peer locations for specific file downloads
4. **Peer Removal**: Handles graceful peer disconnection and dead peer cleanup

## Compilation

```bash
# Go to the root directory, create build/ and compile the project
cd ../.. && mkdir build && cd build && cmake .. && make
```

## Running

```bash
# Run the index_server executable with a port
./index_server <port>
```

**Parameters:**
- `port`: TCP port for the index server to listen on (8080)

## Protocol

### Message Format
```
[COMMAND][SPACE][DATA]
```

### Commands
- `1`: Add peer with file list
- `2`: Request file location by descriptor  
- `3`: Remove peer
- `4`: Search files by name

### Response Format
- Success: `OK[DATA]`
- Error: `ER[ERROR_MESSAGE]`

## Configuration

Default settings in `constants.h`:
- Heartbeat interval: 25 seconds
- Peer timeout: 50 seconds
- Buffer size: 1KB for network I/O