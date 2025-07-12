#pragma once
#include <gtk/gtk.h>
#include <memory>
#include <string>
#include <orchestrator/Orchestrator.h>

using namespace std;

class PeerGUI {
    shared_ptr<Orchestrator> orchestrator;
    GtkWidget *window, *search_entry, *results_view, *hash1_entry, *hash2_entry, *size_entry, *name_entry, *
            status_label;
    GtkListStore *results_store;

public:
    PeerGUI(shared_ptr<Orchestrator> orch);

    void create_window();

    void run();

private:
    void apply_dark_theme();

    void search_files();

    void download_file();

    void fill_download_form(GtkTreePath *path);

    static void on_search_clicked(GtkWidget *widget, gpointer data);

    static void on_download_clicked(GtkWidget *widget, gpointer data);

    static void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data);
};
