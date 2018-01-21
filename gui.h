#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

// #define MESSAGE_INFO 0 // NYI!
#define MESSAGE_ERROR 1
#define MESSAGE_INPUT 32
#define MESSAGE_IS_INPUT(msg) ((msg >= 32) && (msg < 64))

int gui_init(int *argc, char ***argv);
int showMessage(int message_type, char *text, char *defaultText, char **reply);

// Slots
static void okay(GtkWidget *widget, gpointer data);
static void cancel(GtkWidget *widget, gpointer data);

#endif // GUI_H
