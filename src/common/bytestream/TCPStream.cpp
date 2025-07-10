#pragma once
#include "ByteStream.h"
#include <sys/socket.h>
#include "logger/Logger.h"

class TCPStream final : public ByteStream {
    constexpr static size_t BUFFER_SIZE = 1024;
    constexpr char END_OF_STREAM = '\x04';
    constexpr string ACKNOWLEDGE = "ACK";

    string read(const int socket) override {
        string result;
        char buffer[BUFFER_SIZE] = {};
        while (true) {
            const ssize_t bytesRead = recv(socket, buffer, BUFFER_SIZE, 0);
            if (bytesRead < 0) {
                logger.error("Error in read: " + string(strerror(errno)));
                return "";
            }
            if (bytesRead == 0) break;
            result.append(buffer, bytesRead);
            if (result.find(END_OF_STREAM) != string::npos) {
                result.erase(result.find(END_OF_STREAM));
                break;
            }
        }
        return result;

    }

    bool write(const int socket, const string& buffer) override {
        const string package = buffer + END_OF_STREAM;
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
        const string ack = ACKNOWLEDGE + END_OF_STREAM;
        return write(socket, ack);
    }

    bool receive_acknowledge(const int socket) override {
        const string acknowledge = read(socket);
        return acknowledge == ACKNOWLEDGE;
    }

};