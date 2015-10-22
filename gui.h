#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

int gui_init(int *argc, char ***argv);
int showMessage(int message_type, char *text, char *defaultText, char **reply);

// Slots
static void okay( GtkWidget *widget, gpointer data );

#endif // GUI_H
