#include <PeerGUI.h>
#include <util.h>

PeerGUI::PeerGUI(shared_ptr<Orchestrator> orch) : orchestrator(orch) {
}

void PeerGUI::on_search_clicked(GtkWidget *widget, gpointer data) {
    PeerGUI *gui = static_cast<PeerGUI *>(data);
    gui->search_files();
}

void PeerGUI::on_download_clicked(GtkWidget *widget, gpointer data) {
    PeerGUI *gui = static_cast<PeerGUI *>(data);
    gui->download_file();
}

void PeerGUI::on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data) {
    PeerGUI *gui = static_cast<PeerGUI *>(data);
    gui->fill_download_form(path);
}

void PeerGUI::search_files() {
    const char *filename = gtk_entry_get_text(GTK_ENTRY(search_entry));
    if (!filename || strlen(filename) == 0) return;

    gtk_label_set_text(GTK_LABEL(status_label), "Searching network for files...");
    gtk_list_store_clear(results_store);

    auto [success, results] = orchestrator->search_file(string(filename));

    if (success && !results.values.empty()) {
        for (const auto &[name, desc]: results.values) {
            GtkTreeIter iter;
            gtk_list_store_append(results_store, &iter);
            gtk_list_store_set(results_store, &iter,
                               0, name.c_str(),
                               1, (gint64) desc.size,
                               2, (gint64) desc.hash1,
                               3, (gint64) desc.hash2, -1);
        }
        gtk_label_set_text(GTK_LABEL(status_label), ("Found " + to_string(results.values.size()) + " files in network").c_str());
    } else {
        gtk_label_set_text(GTK_LABEL(status_label), "No files found in network");
    }
}

void PeerGUI::fill_download_form(GtkTreePath *path) {
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(results_store), &iter, path)) {
        gchar *name;
        gint64 size, hash1, hash2;
        gtk_tree_model_get(GTK_TREE_MODEL(results_store), &iter,
                           0, &name, 1, &size, 2, &hash1, 3, &hash2, -1);

        gtk_entry_set_text(GTK_ENTRY(hash1_entry), to_string(hash1).c_str());
        gtk_entry_set_text(GTK_ENTRY(hash2_entry), to_string(hash2).c_str());
        gtk_entry_set_text(GTK_ENTRY(size_entry), to_string(size).c_str());
        gtk_entry_set_text(GTK_ENTRY(name_entry), name);
        g_free(name);
    }
}

void PeerGUI::download_file() {
    const char *hash1_str = gtk_entry_get_text(GTK_ENTRY(hash1_entry));
    const char *hash2_str = gtk_entry_get_text(GTK_ENTRY(hash2_entry));
    const char *size_str = gtk_entry_get_text(GTK_ENTRY(size_entry));
    const char *name = gtk_entry_get_text(GTK_ENTRY(name_entry));

    if (!hash1_str || !hash2_str || !size_str || !name) return;

    gtk_label_set_text(GTK_LABEL(status_label), "Downloading file from peers...");

    ll hash1 = stoll(hash1_str), hash2 = stoll(hash2_str);
    size_t size = stoull(size_str);

    FileDescriptor fd(hash1, hash2, size);
    auto file_info = orchestrator->request_file(fd);

    if (file_info && orchestrator->download_file(*file_info, string(name))) {
        gtk_label_set_text(GTK_LABEL(status_label), "Download completed successfully");
    } else {
        gtk_label_set_text(GTK_LABEL(status_label), "Download failed - Please try again");
    }
}

void PeerGUI::apply_dark_theme() {
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css =
            "window { background-color: #1e1e1e; color: #ffffff; }"
            "entry { background-color: #2d2d2d; color: #ffffff; border: 1px solid #4a90e2; border-radius: 4px; padding: 8px 12px; }"
            "entry:focus { border-color: #5ba0f2; }"
            "button { background: linear-gradient(to bottom, #4a90e2, #357abd); color: #ffffff; border: none; border-radius: 4px; padding: 8px 16px; font-weight: 500; }"
            "button:hover { background: linear-gradient(to bottom, #5ba0f2, #4a90e2); }"
            "treeview { background-color: #2d2d2d; color: #ffffff; }"
            "treeview header button { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555; font-weight: 500; }"
            "treeview:selected { background-color: #4a90e2; color: #ffffff; }"
            "scrolledwindow { border: 1px solid #555; border-radius: 4px; }"
            "label { color: #ffffff; }"
            ".title-label { color: #4a90e2; font-size: 18px; font-weight: bold; }"
            ".status-label { background-color: #3a3a3a; color: #ffffff; padding: 8px 12px; border-radius: 4px; border: 1px solid #555; }"
            ".search-frame, .download-frame { background-color: #2a2a2a; border: 1px solid #555; border-radius: 6px; padding: 16px; margin: 8px; }"
            "frame > label { color: #4a90e2; font-weight: 600; font-size: 14px; }";

    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

void PeerGUI::create_window() {
    apply_dark_theme();

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "P2P File Exchange");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 20);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);

    // Title
    GtkWidget *title = gtk_label_new("P2P File Exchange System");
    gtk_widget_set_name(title, "title-label");
    gtk_widget_set_halign(title, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(main_vbox), title, FALSE, FALSE, 10);

    // Search section with frame
    GtkWidget *search_frame = gtk_frame_new("File Search");
    gtk_widget_set_name(search_frame, "search-frame");
    GtkWidget *search_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(search_vbox), 15);

    GtkWidget *search_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Enter filename to search...");
    gtk_widget_set_size_request(search_entry, -1, 35);

    GtkWidget *search_btn = gtk_button_new_with_label("Search Network");
    gtk_widget_set_size_request(search_btn, 120, 35);
    g_signal_connect(search_btn, "clicked", G_CALLBACK(on_search_clicked), this);

    gtk_box_pack_start(GTK_BOX(search_hbox), search_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(search_hbox), search_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(search_vbox), search_hbox, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(search_frame), search_vbox);
    gtk_box_pack_start(GTK_BOX(main_vbox), search_frame, FALSE, FALSE, 0);

    // Results section
    GtkWidget *results_frame = gtk_frame_new("Search Results");
    results_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_INT64, G_TYPE_INT64, G_TYPE_INT64);
    results_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(results_store));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(results_view), GTK_TREE_VIEW_GRID_LINES_BOTH);

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *col1 = gtk_tree_view_column_new_with_attributes("Filename", renderer, "text", 0, NULL);
    GtkTreeViewColumn *col2 = gtk_tree_view_column_new_with_attributes("Size (bytes)", renderer, "text", 1, NULL);
    gtk_tree_view_column_set_resizable(col1, TRUE);
    gtk_tree_view_column_set_resizable(col2, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(results_view), col1);
    gtk_tree_view_append_column(GTK_TREE_VIEW(results_view), col2);

    g_signal_connect(results_view, "row-activated", G_CALLBACK(on_row_activated), this);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scroll), results_view);
    gtk_widget_set_size_request(scroll, -1, 200);
    gtk_container_add(GTK_CONTAINER(results_frame), scroll);
    gtk_box_pack_start(GTK_BOX(main_vbox), results_frame, TRUE, TRUE, 0);

    // Download section with frame
    GtkWidget *download_frame = gtk_frame_new("File Download");
    gtk_widget_set_name(download_frame, "download-frame");
    GtkWidget *download_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(download_vbox), 15);

    GtkWidget *download_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(download_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(download_grid), 10);

    hash1_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(hash1_entry), "Hash 1");
    gtk_widget_set_size_request(hash1_entry, -1, 35);

    hash2_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(hash2_entry), "Hash 2");
    gtk_widget_set_size_request(hash2_entry, -1, 35);

    size_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(size_entry), "File Size");
    gtk_widget_set_size_request(size_entry, -1, 35);

    name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Save as...");
    gtk_widget_set_size_request(name_entry, -1, 35);

    GtkWidget *download_btn = gtk_button_new_with_label("Download");
    gtk_widget_set_size_request(download_btn, 100, 35);
    g_signal_connect(download_btn, "clicked", G_CALLBACK(on_download_clicked), this);

    gtk_grid_attach(GTK_GRID(download_grid), hash1_entry, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(download_grid), hash2_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(download_grid), size_entry, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(download_grid), name_entry, 3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(download_grid), download_btn, 4, 0, 1, 1);

    gtk_box_pack_start(GTK_BOX(download_vbox), download_grid, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(download_frame), download_vbox);
    gtk_box_pack_start(GTK_BOX(main_vbox), download_frame, FALSE, FALSE, 0);

    // Status section
    status_label = gtk_label_new("Ready - Connected to P2P Network");
    gtk_widget_set_name(status_label, "status-label");
    gtk_label_set_xalign(GTK_LABEL(status_label), 0.5);
    gtk_box_pack_start(GTK_BOX(main_vbox), status_label, FALSE, FALSE, 10);

    gtk_widget_show_all(window);
}

void PeerGUI::run() {
    gtk_main();
}
