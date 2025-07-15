#pragma once
#include <ByteStream.h>
#include <sys/socket.h>
#include <logger/Logger.h>
#include <cstring>

class TCPStream final : public ByteStream {
    static constexpr size_t BUFFER_SIZE = 1024;
    static constexpr size_t MAX_MESSAGE_SIZE = 300 * 1024 * 1024;
    static constexpr int SOCKET_TIMEOUT_SEC = 30;
    static constexpr auto ERROR = "ER";
    static constexpr auto OK = "OK";
    
    bool set_socket_timeout(const int socket) const {
        timeval timeout{};
        timeout.tv_sec = SOCKET_TIMEOUT_SEC;
        timeout.tv_usec = 0;
        
        if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0 ||
            setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
            logger.error("Failed to set socket timeout: " + string(strerror(errno)));
            return false;
        }
        return true;
    }

    pair<bool, string> read(const int socket) override {
        if (socket < 0) return {false, "Invalid socket"};

        set_socket_timeout(socket);

        uint32_t size;
        ssize_t recv_result = recv(socket, &size, sizeof(size), MSG_WAITALL);
        if (recv_result != sizeof(size)) {
            if (recv_result < 0) {
                return {false, "Timeout or error reading size: " + string(strerror(errno))};
            }
            return {false, "Incomplete size header received"};
        }
        size = ntohl(size);
        
        if (size > MAX_MESSAGE_SIZE) {
            return {false, "Message size exceeds limit: " + to_string(size)};
        }
        
        if (size < 2) {
            return {false, "Message too small for status prefix"};
        }
        
        string result;
        result.reserve(size);
        size_t totalBytesRead = 0;
        
        while (totalBytesRead < size) {
            char buffer[BUFFER_SIZE];
            const size_t bytesToRead = min(size - totalBytesRead, BUFFER_SIZE);
            const ssize_t bytesRead = recv(socket, buffer, bytesToRead, 0);
            
            if (bytesRead < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK || errno == ETIMEDOUT) {
                    return {false, "Timeout reading message data"};
                }
                return {false, "Error reading message: " + string(strerror(errno))};
            }
            if (bytesRead == 0) {
                return {false, "Connection closed during read"};
            }
            
            result.append(buffer, bytesRead);
            totalBytesRead += bytesRead;
        }

        if (result.size() < 2) return {false, "Message too short for status"};
        const string status = result.substr(0, 2);
        string payload = result.substr(2);
        
        if (status == OK) return {true, payload};
        if (status == ERROR) return {false, payload};
        
        return {false, "Invalid status prefix: " + status};
    }


    bool write(const bool success, const int socket, const string &buffer) override {
        if (socket < 0) {
            logger.error("Invalid socket for write");
            return false;
        }
        
        if (buffer.size() > MAX_MESSAGE_SIZE - 2) {
            logger.error("Message too large: " + to_string(buffer.size()));
            return false;
        }
        
        set_socket_timeout(socket);
        
        const string package = (success ? OK : ERROR) + buffer;
        
        uint32_t size = htonl(package.size());
        ssize_t send_result = send(socket, &size, sizeof(size), 0);
        if (send_result != sizeof(size)) {
            if (send_result < 0) {
                logger.error("Timeout or error sending size: " + string(strerror(errno)));
            } else {
                logger.error("Incomplete size header sent");
            }
            return false;
        }
        
        size_t totalBytesSent = 0;
        while (totalBytesSent < package.size()) {
            const ssize_t bytesToSend = min(package.size() - totalBytesSent, BUFFER_SIZE);
            const ssize_t bytesSent = send(socket, &package[totalBytesSent], bytesToSend, 0);
            if (bytesSent < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK || errno == ETIMEDOUT) {
                    logger.error("Timeout sending message data");
                    return false;
                }
                logger.error("Error in write: " + string(strerror(errno)));
                return false;
            }
            if (bytesSent == 0) {
                logger.error("Connection closed during write");
                return false;
            }
            totalBytesSent += bytesSent;
        }
        return true;
    }

};
