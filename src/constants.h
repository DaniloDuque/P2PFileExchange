#pragma once
#include <string>
using namespace std;

constexpr size_t RESPONSE_LIST_SIZE = 100;
constexpr size_t BUFFER_SIZE = 1024;
constexpr size_t INDEX_HEARTBEAT_INTERVAL = 25;
constexpr size_t INDEX_PEER_TIMEOUT = 50;

constexpr string GREEN = "\033[0;32m";
constexpr string RED = "\033[0;31m";
constexpr string YELLOW = "\033[0;33m";
constexpr string BLUE = "\033[0;34m";
constexpr string RESET_COLOR = "\033[0m";

constexpr string INFO = "INFO";
constexpr string ERROR = "ERROR";
constexpr string DEBUG = "DEBUG";
constexpr string WARN = "WARN";

constexpr string ACK = "ACK";
constexpr string ERR = "1";
constexpr string OK = "0";

constexpr char ADD_PEER = '1';
constexpr char FILE_REQUEST = '2';
constexpr char REMOVE_PEER = '3';
constexpr char FILE_SEARCH = '4';