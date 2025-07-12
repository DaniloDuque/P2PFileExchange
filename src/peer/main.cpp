#include "orchestrator/Orchestrator.cpp"
#include "util.h"
#include "logger/Logger.h"
#include <csignal>

shared_ptr<Orchestrator> orchestrator;

void signalHandler(const int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        if (orchestrator) orchestrator->stop();
        exit(0);
    }
}

void showMenu() {
    cout << "\n=== P2P File Exchange ===\n";
    cout << "1. Search file\n";
    cout << "2. Download file\n";
    cout << "3. Exit\n";
    cout << "Choice: ";
}

int main(const int argc, char const *argv[]) {
    if (argc < 7) {
        logger.error("Usage: " + string(argv[0]) + " <server port> <client port> <Ip> <index Ip> <indexPort> <directory>");
        return -1;
    }
    
    const int server_port = stoi(argv[1]);
    const int client_port = stoi(argv[2]);
    const string local_ip = argv[3];
    const string index_ip = argv[4];
    const int index_port = stoi(argv[5]);
    const string shared_directory = argv[6];
    
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    orchestrator = make_shared<Orchestrator>(local_ip, server_port, client_port, index_ip, index_port, shared_directory, shared_directory);
    orchestrator->start();
    
    int choice;
    while (true) {
        showMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                string filename;
                cout << "Enter filename to search: ";
                cin >> filename;
                
                const auto [success, results] = orchestrator->search_file(filename);
                if (success && !results.values.empty()) {
                    print_file_info_table(results);
                } else {
                    cout << "No files found.\n";
                }
                break;
            }
            case 2: {
                ll hash1, hash2;
                size_t size;
                string name;
                cout << "Enter file hash1: "; cin >> hash1;
                cout << "Enter file hash2: "; cin >> hash2;
                cout << "Enter file size: "; cin >> size;
                cout << "Enter save name: "; cin >> name;
                
                const FileDescriptor fd(hash1, hash2, size);
                const auto file_info = orchestrator->request_file(fd);
                if (file_info) {
                    if (orchestrator->download_file(*file_info, name)) {
                        cout << "File downloaded successfully.\n";
                    } else {
                        cout << "Download failed.\n";
                    }
                } else {
                    cout << "File not found in network.\n";
                }
                break;
            }
            case 3:
                orchestrator->stop();
                return 0;
            default:
                cout << "Invalid choice.\n";
        }
    }
}
