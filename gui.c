#include "gui.h"

#define true 1
#define false 0

int gui_init(int *argc, char ***argv) {
    gtk_init (argc, argv);
    return 0;
}

int showMessage(int message_type, char *text, char *defaultText, char **reply) {
    GtkWidget *window;
    GtkWidget *entry;
    GtkWidget *buttonOkay;
    GtkWidget *buttonCancel;
    GtkWidget *buttonBox;
    GtkWidget *mainBox;
    GtkWidget *label;
    char *entryText;

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "model-ghdl");

    label = gtk_label_new(text);
    gtk_widget_show(label);

    entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), (char*) defaultText);
    g_signal_connect (entry, "activate",
                      G_CALLBACK(okay), entry);

    buttonOkay = gtk_button_new_with_label("Okay!");
    gtk_widget_show(buttonOkay);

    if (MESSAGE_IS_INPUT(message_type)) {
        buttonCancel = gtk_button_new_with_label("Cancel");
        g_signal_connect (buttonCancel, "clicked",
                          G_CALLBACK(cancel), entry);
        gtk_widget_show(buttonCancel);

        gtk_widget_show(entry);
    }

    buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    if (MESSAGE_IS_INPUT(message_type)) {
        gtk_box_pack_start(GTK_BOX(buttonBox), buttonCancel, true, true, 0);
    }
    gtk_box_pack_start(GTK_BOX(buttonBox), buttonOkay, true, true, 0);
    g_signal_connect (buttonOkay, "clicked",
                      G_CALLBACK(okay), entry);
    gtk_widget_show(buttonBox);

    mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start(GTK_BOX(mainBox), label, true, true, 10);
    if (MESSAGE_IS_INPUT(message_type)) {
        gtk_box_pack_start(GTK_BOX(mainBox), entry, true, true, 0);
    }
    gtk_box_pack_start(GTK_BOX(mainBox), buttonBox, false, false, 0);
    gtk_widget_show(mainBox);

    g_signal_connect (window, "destroy",
                      G_CALLBACK (cancel), NULL);

    gtk_container_add(GTK_CONTAINER(window), mainBox);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 320, 10);
    gtk_widget_show (window);

    gtk_main();

    if (MESSAGE_IS_INPUT(message_type)) {
        entryText = (char*) gtk_entry_get_text(GTK_ENTRY(entry));
        if (entryText[0] == 0) {
            return false;
        }
        else {
            *reply = realloc(*reply, sizeof(char)*strlen(entryText));
            if (*reply != NULL)
                strcpy(*reply, entryText);
            return true;
        }
    }

    gtk_widget_destroy(window);
}

static void okay( GtkWidget *widget,
                  gpointer   data )
{
    //g_print ("TEXT = %s\n", gtk_entry_get_text(GTK_ENTRY(data)));
    gtk_main_quit();
}

static void cancel( GtkWidget *widget,
                    gpointer   data )
{
    gtk_entry_set_text(GTK_ENTRY(data), "");
    gtk_main_quit();
}
