#pragma once
#include <util.h>
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
#include <constants.h>
using namespace std;

class HeartbeatManager {
    map<PeerDescriptor, chrono::steady_clock::time_point> peerLastSeen;
    mutable mutex peerMutex;
    atomic<bool> running{false};
    thread heartbeatThread;
    const chrono::seconds HEARTBEAT_INTERVAL{INDEX_HEARTBEAT_INTERVAL};
    const chrono::seconds PEER_TIMEOUT{INDEX_PEER_TIMEOUT};
    function<void(const PeerDescriptor &)> onPeerDead;

    bool ping_peer(const PeerDescriptor &descriptor) const {
        const int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) return false;

        timeval timeout{};
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(descriptor.port);
        inet_pton(AF_INET, descriptor.ip.c_str(), &addr.sin_addr);

        const bool alive = connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0;
        close(sock);
        return alive;
    }

    void heartbeat_loop() {
        while (running) {
            auto now = chrono::steady_clock::now(); {
                vector<PeerDescriptor> deadPeers;
                unique_lock lock(peerMutex);
                for (auto &[peer, lastSeen]: peerLastSeen) {
                    if (now - lastSeen <= PEER_TIMEOUT) continue;
                    if (!ping_peer(peer)) deadPeers.push_back(peer);
                    else peerLastSeen[peer] = now;
                }

                for (auto &peer: deadPeers) {
                    peerLastSeen.erase(peer);
                    if (onPeerDead) onPeerDead(peer);
                    logger.warn("Removed dead peer: " + peer.ip + ":" + to_string(peer.port));
                }
            }

            this_thread::sleep_for(HEARTBEAT_INTERVAL);
        }
    }

public:
    void set_dead_peer_callback(const function<void(const PeerDescriptor &)> &callback) {
        onPeerDead = callback;
    }

    void start() {
        running = true;
        heartbeatThread = thread(&HeartbeatManager::heartbeat_loop, this);
    }

    void stop() {
        running = false;
        if (heartbeatThread.joinable()) heartbeatThread.join();
    }

    void update_peer(const PeerDescriptor &descriptor) {
        unique_lock lock(peerMutex);
        peerLastSeen[descriptor] = chrono::steady_clock::now();
    }

    void remove_peer(const PeerDescriptor &descriptor) {
        unique_lock lock(peerMutex);
        peerLastSeen.erase(descriptor);
        if (onPeerDead) onPeerDead(descriptor);
        logger.info("Removed stoped peer: " + descriptor.ip + ":" + to_string(descriptor.port));
    }

    ~HeartbeatManager() {
        stop();
    }
};
