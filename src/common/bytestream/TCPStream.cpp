#pragma once
#include <ByteStream.h>
#include <sys/socket.h>
#include <logger/Logger.h>
#include <cstring>

class TCPStream final : public ByteStream {
    static constexpr size_t BUFFER_SIZE = 1024;
    static constexpr char END_OF_STREAM = '\x04';
    static constexpr auto ACKNOWLEDGE = "ACK";
    static constexpr auto ERROR = "ER";
    static constexpr auto OK = "OK";

    pair<bool, string> read(const int socket) override {
        if (socket < 0) return {false, "Invalid socket"};
        
        uint32_t size;
        if (recv(socket, &size, sizeof(size), MSG_WAITALL) != sizeof(size)) {
            return {false, "Failed to read message size"};
        }
        size = ntohl(size);
        
        string result;
        result.reserve(size);
        size_t totalBytesRead = 0;
        
        while (totalBytesRead < size) {
            char buffer[BUFFER_SIZE];
            const size_t bytesToRead = min(size - totalBytesRead, BUFFER_SIZE);
            const ssize_t bytesRead = recv(socket, buffer, bytesToRead, 0);
            
            if (bytesRead < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                return {false, "Error reading message: " + string(strerror(errno))};
            }
            if (bytesRead == 0) break;
            
            result.append(buffer, bytesRead);
            totalBytesRead += bytesRead;
        }

        if (result.empty()) return {false, "Empty payload"};
        const string status = result.substr(0, 2);
        string payload = result.substr(2);
        if (status == OK) return {true, payload};
        if (status == ERROR) return {false, payload};
        const string error = "Invalid status prefix in message: " + status;
        logger.error(error);
        return {false, error};
    }


    bool write(const bool success, const int socket, const string &buffer) override {
        if (socket < 0) {
            logger.error("Invalid socket for write");
            return false;
        }
        
        const string package = (success ? OK : ERROR) + buffer;
        
        uint32_t size = htonl(package.size());
        if (send(socket, &size, sizeof(size), 0) != sizeof(size)) {
            logger.error("Failed to send message size");
            return false;
        }
        
        size_t totalBytesSent = 0;
        while (totalBytesSent < package.size()) {
            const ssize_t bytesToSend = min(package.size() - totalBytesSent, BUFFER_SIZE);
            const ssize_t bytesSent = send(socket, &package[totalBytesSent], bytesToSend, 0);
            if (bytesSent < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                logger.error("Error in write: " + string(strerror(errno)));
                return false;
            }
            totalBytesSent += bytesSent;
        }
        return true;
    }

};
