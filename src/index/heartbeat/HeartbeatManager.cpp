#pragma once
#include "util.h"
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <functional>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

class HeartbeatManager {
    map<pair<string, int>, chrono::steady_clock::time_point> peerLastSeen;
    mutable mutex peerMutex;
    atomic<bool> running{false};
    thread heartbeatThread;
    const chrono::seconds HEARTBEAT_INTERVAL{25};
    const chrono::seconds PEER_TIMEOUT{50};
    function<void(const string&, int)> onPeerDead;

    bool pingPeer(const string& ip, int port) const {
        const int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) return false;

        timeval timeout{};
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

        const bool alive = connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0;
        close(sock);
        return alive;
    }

    void heartbeatLoop() {
        while (running) {
            auto now = chrono::steady_clock::now(); {
                vector<pair<string, int>> deadPeers;
                lock_guard lock(peerMutex);
                for (auto& [peer, lastSeen] : peerLastSeen) {
                    if (now - lastSeen > PEER_TIMEOUT) {
                        if (!pingPeer(peer.first, peer.second)) {
                            deadPeers.push_back(peer);
                        } else {
                            peerLastSeen[peer] = now;
                        }
                    }
                }

                for (auto& peer : deadPeers) {
                    peerLastSeen.erase(peer);
                    if (onPeerDead) onPeerDead(peer.first, peer.second);
                    logger.warn("Removed dead peer: " + peer.first + ":" + to_string(peer.second));
                }
            }

            this_thread::sleep_for(HEARTBEAT_INTERVAL);
        }
    }

public:
    void setDeadPeerCallback(const function<void(const string&, int)> &callback) {
        onPeerDead = callback;
    }

    void start() {
        running = true;
        heartbeatThread = thread(&HeartbeatManager::heartbeatLoop, this);
    }

    void stop() {
        running = false;
        if (heartbeatThread.joinable()) heartbeatThread.join();
    }

    void updatePeer(const string& ip, int port) {
        lock_guard lock(peerMutex);
        peerLastSeen[{ip, port}] = chrono::steady_clock::now();
    }

    void removePeer(const string& ip, int port) {
        lock_guard lock(peerMutex);
        peerLastSeen.erase({ip, port});
        if (onPeerDead) onPeerDead(ip, port);
        logger.info("Removed stoped peer: " + ip + ":" + to_string(port));
    }

    set<pair<string, int>> getAlivePeers() const {
        lock_guard lock(peerMutex);
        set<pair<string, int>> alive;
        for (const auto &peer: peerLastSeen | views::keys) {
            alive.insert(peer);
        }
        return alive;
    }

    ~HeartbeatManager() {
        stop();
    }
};
