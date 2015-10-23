#include <stdio.h>
#include <string.h>
#include "gui.h"

#ifndef PROGRAM_REVISION
#define PROGRAM_REVISION "#unknown"
#endif

#define PROG_VCOM 0
#define PROG_VSIM 1
#define PROG_VLIB 2
#define PROG_VMAP 3
#define PROG_VDEL 4
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
    char *work = NULL;
    char *toplevel = NULL;

    int i;
    char *ptr = NULL;
    char *lastPtr;
    char workdir[1 K];
    char *params = NULL;
    char *simtime = NULL;

    FILE *fp;

    gui_init(&argc, &argv);

    fp = fopen("/tmp/model-ghdl-vsim","r");
    if (fp) {
        fgets(workdir, sizeof(workdir), fp); // lets (ab)use the workdir variable here
        append_string(&simtime, workdir);
        fclose(fp);
    }
    else {
        append_string(&simtime, "100ns");
    }

    fp = fopen("/tmp/model-ghdl-vcom","r");
    if (fp) {
        fgets(workdir, sizeof(workdir), fp);
        fclose(fp);
    }
    else {
        printf("[E] Could not read temp file /tmp/model-ghdl-vcom! Aborting...");
    }

    printf ("[I] Emulating vsim.\n");
    // -gui work.toplevel(RTL)
    for (i=1; i < argc; ++i) {
        if (ptr == NULL && GETOPT("-gui")) { // only allow once
            append_string(&ptr, argv[i]);
            lastPtr = ptr;
            for (; *ptr != 0; ptr++) {
                if (*ptr >= 'A' && *ptr <= 'Z')
                    *ptr = *ptr - ('A'-'a'); // convert to lower case

                if (*ptr == '.') {
                    *ptr++ = 0;
                    work = lastPtr;
                    lastPtr = ptr;
                }
                else if (*ptr == '(') {
                    *ptr++ = 0;
                    toplevel = lastPtr;
                    lastPtr = ptr;
                }
            }
            // free(ptr); DO NOT FREE, we still need it.
            // ptr = NULL;
        }
        else if (GETOPT("-ghdl")) {
            append_string(&params, " ");
            append_string(&params, argv[i]);
        }
        else {

        }
    }

    chdir(workdir);

    printf("Compiling...\n");
    if (run_ghdl("ghdl -m --work=%s --workdir=\"%s\" %s %s", work, workdir, params, toplevel)) {
        fprintf(stderr, "[E] Compilation failed!");
        showMessage(MESSAGE_ERROR, "Error! Compilation failed.", NULL, NULL);
    }
    else {
        if (ret = showMessage(MESSAGE_INPUT, "Enter the simulation time: ", simtime, &text)) {
            free(simtime);
            simtime = NULL;
            append_string(&simtime, text);
            fp = fopen("/tmp/model-ghdl-vsim","w");
            if (fp) {
                fprintf(fp, "%s", simtime);
                fclose(fp);
            }
            printf("Simulating...\n");
            // TODO: Exec program
            // TODO: Exec GtkWave
        }
        return 0;
    }

    free(ptr); // Now we can free it

    return 255;
}

int vcom(int argc, char **argv)
{
    int i;
    int slen = 0;
    char workdir[1 K];
    char *params = NULL;
    char *work = NULL;
    char *files = NULL;
    char vhdlver[16] = "";
    FILE *fp = NULL;

    printf ("[I] Emulating vsim.\n");

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
    if (!work)
        append_string(&work, "work");

    if (!files) {
        fprintf(stderr, "[E] No input files specified.\n");
        return 2;
    }

    // Info for vsim later on
    fp = fopen("/tmp/model-ghdl-vcom","w");
    if (fp) {
        fprintf(fp, "%s", workdir);
        fclose(fp);
    }
    else {
        printf("Could not create temp file /tmp/model-ghdl-vcom! Ignoring...");
    }

    run_ghdl("ghdl -i --work=%s --workdir=%s %s %s %s 2>&1",
             work, workdir, vhdlver, files, params);
    run_ghdl("ghdl -s --work=%s --workdir=%s %s %s %s 2>&1",
             work, workdir, vhdlver, files, params);


    free(files);

    printf("DONE.\n");
    return 0;
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
    else if (app == PROG_VMAP || app == PROG_VLIB || app == PROG_VDEL) {
        return 0;
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
        return PROG_VSIM;
    }
    else if (strcmp(pos, "vlib") == 0) {
        return PROG_VLIB;
    }
    else if (strcmp(pos, "vmap") == 0) {
        return PROG_VMAP;
    }
    else if (strcmp(pos, "vdel") == 0) {
        return PROG_VDEL;
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
