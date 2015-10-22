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
    gtk_window_set_title(GTK_WINDOW(window), "Meow");

    label = gtk_label_new(text);
    gtk_widget_show(label);

    entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), (char*) defaultText);
    g_signal_connect (entry, "activate",
                      G_CALLBACK(okay), entry);
    gtk_widget_show(entry);

    buttonOkay = gtk_button_new_with_label("Okay!");
    gtk_widget_show(buttonOkay);
    buttonCancel = gtk_button_new_with_label("Cancel");
    gtk_widget_show(buttonCancel);

    buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_box_pack_start(GTK_BOX(buttonBox), buttonCancel, true, true, 0);
    gtk_box_pack_start(GTK_BOX(buttonBox), buttonOkay, true, true, 0);
    g_signal_connect (buttonOkay, "clicked",
                      G_CALLBACK(okay), entry);
    gtk_widget_show(buttonBox);

    mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start(GTK_BOX(mainBox), label, true, true, 0);
    gtk_box_pack_start(GTK_BOX(mainBox), entry, true, true, 0);
    gtk_box_pack_start(GTK_BOX(mainBox), buttonBox, true, true, 0);
    gtk_widget_show(mainBox);

    g_signal_connect (window, "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);

    gtk_container_add(GTK_CONTAINER(window), mainBox);
    gtk_widget_show (window);

    gtk_main();

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
