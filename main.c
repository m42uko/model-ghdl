#include <stdio.h>
#include <string.h>
#include "gui.h"

#ifndef PROGRAM_REVISION
#define PROGRAM_REVISION "#unknown"
#endif

#define PROG_VCOM 0
#define PROG_VSIM 1
#define PROG_UNKNOWN 255

#define K *1024
#define M K K
#define G M K

#define ISOPT(cmd)  (strcmp(argv[i], cmd) == 0)
#define GETOPT(cmd) (strcmp(argv[i], cmd) == 0) && (++i < argc)

const char *laststrstr (const char *__haystack, const char *__needle);
int get_application(const char *call);
int vsim(int argc, char **argv);
int vcom(int argc, char **argv);
int run_ghdl(char *command, ...);
char* append_string(char **dest, const char *src);

int run_ghdl(char *command, ...) {
    FILE *proc;
    char buf[1 K];
    char cmd[1 K];

    char *arr[4];
    char *start;
    char *ptr;
    int arrc;

    va_list argptr;
    va_start(argptr, command);
    vsprintf(cmd, command, argptr);
    va_end(argptr);

    printf("RUN: %s\n", cmd);
    proc = popen(cmd, "r");

    if (proc == NULL) {
        printf("Error: Could not invoke GHDL/GtkWave.\n");
        return 1;
    }

    // ../blink/src/top.vhd:32:19: no declaration for "counter_i2"
    //                            v
    // ** Error: /tmp/filename.vhd(32): (vcom-1136) Unknown identifier "counter_i2".

    while(fgets(buf, sizeof(buf), proc)!=NULL){
        ptr = buf;
        start = buf;
        arrc = 0;

        do { // Search for EOL
            if (arrc < 4 && (*ptr == ':' || *ptr == '\0')) {
                *ptr++ = 0;
                arr[arrc++] = start;
                start = ptr;
            }
        } while (*ptr++ != '\0');

        if (arrc == 4) {
            printf("** Error: %s(%s): (ghdl) %s", arr[0], arr[1], arr[3]);
        }
        else {
            printf("** ghdl: %s", buf);
        }
    }
    printf("\n");

    return pclose(proc);
}

int vsim(int argc, char **argv)
{
    int ret;
    char *text = NULL;

    gui_init(&argc, &argv);
    ret = showMessage(0, "Enter the simulation time: ", "100 ns", &text);
    printf("%d: %p: %s\n", ret, text, text);
}

int vcom(int argc, char **argv)
{
    int i;
    int slen = 0;
    char workdir[1 K];
    char *params = NULL;
    char *work;
    char *files = NULL;
    char vhdlver[16] = "";

    if (!getcwd(workdir, sizeof(workdir))) { // Default compile dir is cwd
        fprintf(stderr, "Error: Could not invoke GHDL!\n");
        return 1;
    }

    for (i=1; i < argc; ++i) {
        if (GETOPT("-work")) {
            work = argv[i];
        }
        else if (GETOPT("-workdir")) {
            strcpy(workdir, argv[i]);
        }
        else if (ISOPT("-87")) {
            strcpy(vhdlver, "--std=87");
        }
        else if (ISOPT("-93")) {
            strcpy(vhdlver, "--std=93");
        }
        else if (ISOPT("-93c")) {
            strcpy(vhdlver, "--std=93c");
        }
        else if (ISOPT("-2000")) {
            strcpy(vhdlver, "--std=00");
        }
        else if (ISOPT("-2002")) {
            strcpy(vhdlver, "--std=02");
        }
        else if (ISOPT("-2008")) {
            strcpy(vhdlver, "--std=08");
        }
        else if (GETOPT("-ghdl")) {
            append_string(&params, argv[i]);
        }
        else if (argv[i][0] != '-'){ // VHDL file
            slen += strlen(argv[i]) + 1;
            files = realloc(files, slen * sizeof(char));
            strcat(files, " ");
            strcat(files, argv[i]);
        }
    }

    if (!params)
        append_string(&params, "");

    run_ghdl("ghdl -i --work=%s --workdir=%s %s %s %s 2>&1",
             work, workdir, vhdlver, files, params);
    run_ghdl("ghdl -s --work=%s --workdir=%s %s %s %s 2>&1",
             work, workdir, vhdlver, files, params);
    free(files);

    printf("DONE.\n");
}


char* append_string(char **dest, const char *src) {
    if (*dest == NULL) {
        *dest = malloc(strlen(src) * sizeof(char));
        if (*dest == NULL)
            return NULL;
        *dest[0] = 0;
    }
    else {
        *dest = realloc(*dest, (strlen(*dest) + strlen(src)) * sizeof(char));
    }

    strcat(*dest, src);
    return *dest;
}

int main(int argc, char **argv)
{
    int app;
    printf ("model-ghdl revision %s, compiled on %s.\n", PROGRAM_REVISION, __DATE__);

    app = get_application(argv[0]);

    if (app == PROG_VCOM) {
        return vcom(argc, argv);
    }
    else if (app == PROG_VSIM) {
        return vsim(argc, argv);
    }

    return 255;
}

// Detects which function to call depending on the program name in argv[0]
int get_application(const char *call) {
    char *pos;
    pos = (char*) laststrstr(call, "/") + 1;
    if (strcmp(pos, "vcom") == 0) {
        return PROG_VCOM;
    }
    else if (strcmp(pos, "vsim") == 0) {
        return PROG_VCOM;
    }
    else {
        fprintf(stderr, "[E] Program not recognized: %s\n", pos);
        return PROG_UNKNOWN;
    }
}

// Returns the string after the last occurence of __needle
const char *laststrstr (const char *__haystack, const char *__needle) {
    char *pos, *realPos;
    char *haystack;
    haystack = (char*) __haystack;
    pos = (char*) __haystack;
    while (pos != NULL) {
        realPos = pos;
        pos = strstr(haystack, __needle);
        haystack = pos + 1;
    }
    return realPos;
}
