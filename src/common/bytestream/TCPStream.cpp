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
        string result;
        char buffer[BUFFER_SIZE] = {};
        while (true) {
            const ssize_t bytesRead = recv(socket, buffer, BUFFER_SIZE, 0);
            if (bytesRead < 0) {
                const string error = "Error in read: " + string(strerror(errno));
                logger.error(error);
                return {false, error};
            }
            if (bytesRead == 0) break;
            result.append(buffer, bytesRead);
            if (result.find(END_OF_STREAM) != string::npos) {
                result.erase(result.find(END_OF_STREAM));
                break;
            }
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
        const string package = (success ? OK : ERROR) + buffer + END_OF_STREAM;
        size_t totalBytesSent = 0;
        const size_t bufferSize = package.size();
        while (totalBytesSent < bufferSize) {
            const ssize_t bytesToSend = min(bufferSize - totalBytesSent, BUFFER_SIZE);
            const ssize_t bytesSent = send(socket, &package[totalBytesSent], bytesToSend, 0);
            if (bytesSent < 0) {
                logger.error("Error in write: " + string(strerror(errno)));
                return false;
            }
            totalBytesSent += bytesSent;
        }
        return true;
    }

    bool send_acknowledge(const int socket) override {
        return write(true, socket, ACKNOWLEDGE);
    }

    bool receive_acknowledge(const int socket) override {
        const auto [success, payload] = read(socket);
        return success && payload == ACKNOWLEDGE;
    }
};
