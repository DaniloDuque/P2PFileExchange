#include <gtk/gtk.h>
#include <memory>
#include <ui/PeerGUI.h>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 7) return -1;

    gtk_init(&argc, &argv);

    const auto orchestrator = make_shared<Orchestrator>(argv[3], stoi(argv[1]), stoi(argv[2]), argv[4], stoi(argv[5]),
                                                        argv[6], argv[6]);
    orchestrator->start();

    PeerGUI gui(orchestrator);
    gui.create_window();
    gui.run();

    orchestrator->stop();
    return 0;
}
