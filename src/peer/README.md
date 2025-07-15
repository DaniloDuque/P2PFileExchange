# Peer Client

P2P file sharing client with GUI that connects to the index server to share and download files from other peers.

## How It Works

### Architecture
- **Orchestrator**: Coordinates client and server components
- **TCPClient**: Communicates with index server and other peers
- **PeerServer**: Serves file chunks to other peers
- **FileIndexer**: Scans and hashes local files for sharing
- **PeerGUI**: GTK-based graphical interface

### File Sharing Process
1. **Startup**: Index local files and register with index server
2. **Search**: Query index server for files by name
3. **Download**: Request file locations and download chunks from multiple peers
4. **Serve**: Respond to chunk requests from other peers

## Compilation

### Dependencies
```bash
# Ubuntu/Debian
sudo apt-get install libgtk-3-dev build-essential

# macOS
brew install gtk+3 pkg-config
```

### Build
```bash
# Go to the root directory, create build/ and compile the project
cd ../.. && mkdir build && cd build && cmake .. && make
```

## Running

```bash
./peer_client <server_port> <client_port> <local_ip> <index_ip> <index_port> <shared_directory>
```

**Parameters:**
- `server_port`: Port to serve files to other peers
- `client_port`: Port for client operations  
- `local_ip`: This peer's IP address
- `index_ip`: Index server IP address
- `index_port`: Index server port
- `shared_directory`: Directory containing files to share (also used for downloads)

**Example:**
```bash
./peer_client 9001 9002 127.0.0.1 127.0.0.1 8080 ../assets/Peer1/
```

## GUI Usage

### File Search
1. Enter filename in search box
2. Click "Search Network"
3. Results show available files with size information

### File Download
1. Click on search result to auto-fill download form
2. Modify filename if desired
3. Click "Download" to start parallel download from multiple peers

### Status
- Status bar shows current operation progress
- Downloads are saved to `<shared_directory>/<client_port>/`

## File Format Support

- **Any file type** supported
- **Duplicate detection** via dual hash system (polynomial + FNV-1a)
- **Integrity verification** through hash comparison
- **Chunked transfer** for large files

## Network Protocol

### With Index Server
- Registration with file list and hashes
- File search by name pattern matching
- File location requests by hash descriptor

### Peer-to-Peer
- Chunk requests with offset and size
- Base64 encoded file data transfer
- Parallel downloads from multiple sources