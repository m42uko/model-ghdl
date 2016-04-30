#include <stdio.h>
#include <string.h>
#include <fcntl.h>

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
int run_simulation(char *command, ...);
int run_gtkwave(char *toplevel, char *command, ...);
char* append_string(char **dest, const char *src);
pid_t system2(const char * command, int * infp, int * outfp);

// Thanks GreenScape
// http://stackoverflow.com/questions/22802902/how-to-get-pid-of-process-executed-with-system-command-in-c
pid_t system2(const char * command, int * infp, int * outfp)
{
    int p_stdin[2];
    int p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) == -1)
        return -1;

    if (pipe(p_stdout) == -1) {
        close(p_stdin[0]);
        close(p_stdin[1]);
        return -1;
    }

    pid = fork();

    if (pid < 0) {
        close(p_stdin[0]);
        close(p_stdin[1]);
        close(p_stdout[0]);
        close(p_stdout[1]);
        return pid;
    } else if (pid == 0) {
        close(p_stdin[1]);
        dup2(p_stdin[0], 0);
        close(p_stdout[0]);
        dup2(p_stdout[1], 1);
        dup2(open("/dev/null", O_RDONLY), 2);
        /// Close all other descriptors for the safety sake.
        for (int i = 3; i < 4096; ++i)
            close(i);

        setsid();
        execl("/bin/sh", "sh", "-c", command, NULL);
        _exit(1);
    }
}

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

    //printf("RUN_GHDL: %s\n", cmd);
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

int run_simulation(char *command, ...) {
    FILE *proc;
    char buf[1 K];
    char cmd[1 K];

    va_list argptr;
    va_start(argptr, command);
    vsprintf(cmd, command, argptr);
    va_end(argptr);

    //printf("RUN_SIM: %s\n", cmd);
    proc = popen(cmd, "r");

    if (proc == NULL) {
        printf("[E] Could not start the simulation.\n");
        return 1;
    }

    while(fgets(buf, sizeof(buf), proc)!=NULL){
        printf("** sim: %s", buf);
    }
    printf("\n");

    return pclose(proc);
}

int run_gtkwave(char *toplevel, char *command, ...) {
    FILE *fp;
    pid_t pid;
    char cmd[1 K];
    char lockpath[1 K];
    va_list argptr;

    sprintf(lockpath, "/tmp/model-ghdl-gtkw-%s.lock", toplevel);

    fp = fopen(lockpath,"r");
    if (fp) {
        fgets(cmd, sizeof(cmd), fp); // lets (ab)use the cmd variable here
        pid = atoi(cmd);
        fclose(fp);

        if (kill(pid, 0)) { // Check if the process still lives
            pid = -1;
        }
        /*else {
            printf("GtkWave is already running.\n");
        }*/
    }
    else {
        pid = -1;
    }

    if (pid < 0) {
        va_start(argptr, command);
        vsprintf(cmd, command, argptr);
        va_end(argptr);

        //printf("RUN_SIM: %s\n", cmd);

        pid = system2(cmd, NULL, NULL);
        //printf("--> PID=%d\n", pid);

        // Prevent gtkw from starting again each time
        fp = fopen(lockpath,"w");
        if (fp) {
            fprintf(fp, "%d", pid);
            fclose(fp);
        }
        else {
            printf("[W] Could not create temp file %s! Ignoring...", lockpath);
        }
    }

    return 0;
}


int vsim(int argc, char **argv)
{
    int ret;
    char *text = NULL;
    char *work = NULL;
    char *toplevel = NULL;
    char *gtkwPrefix = NULL;

    int i;
    char *ptr = NULL;
    char *lastPtr;
    char workdir[1 K];
    char sourcedir[1 K];
    char *params = NULL;
    char *simtime = NULL;

    FILE *fp;

    append_string(&params,"");

    gui_init(&argc, &argv);

    if (!getcwd(sourcedir, sizeof(sourcedir))) { // Default compile dir is cwd
        sourcedir[0] = 0;
        printf("[W] Could not get cwd!\n");
    }

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
        else if (GETOPT("-gtkwprefix")) {
            gtkwPrefix = argv[i];
        }
        else {

        }
    }

    chdir(workdir);

    if (gtkwPrefix == NULL) {
        append_string(&gtkwPrefix, "");
    }

    printf("[I] Compiling...\n");
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

            printf("[I] Simulating...\n");
            if (run_simulation("%s/%s --stop-time=%s --wave=%s.ghw", workdir, toplevel, simtime, toplevel)) {
                fprintf(stderr, "[E] Simulation failed!");
                showMessage(MESSAGE_ERROR, "Error! Simulation failed.", NULL, NULL);
            }
            else {
                if (run_gtkwave(toplevel, "gtkwave %s/%s.ghw --save=\"%s/%s%s.gtkw\"", workdir, toplevel, sourcedir, gtkwPrefix, toplevel)) { // TODO: PATH FOR Savefile
                    fprintf(stderr, "[E] Could not open GtkWave!");
                    showMessage(MESSAGE_ERROR, "Error! Could not open GtkWave!", NULL, NULL);
                }
                printf("[I] DONE.\n");
            }
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

    printf ("[I] Emulating vcom.\n");

    if (!getcwd(workdir, sizeof(workdir))) { // Default compile dir is cwd
        fprintf(stderr, "[E] Could not get cwd!\n");
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
            append_string(&params, " ");
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
        printf("[W] Could not create temp file /tmp/model-ghdl-vcom! Ignoring...");
    }

    run_ghdl("ghdl -i --work=%s --workdir=%s %s %s %s 2>&1",
             work, workdir, vhdlver, params, files);
    run_ghdl("ghdl -s --work=%s --workdir=%s %s %s %s 2>&1",
             work, workdir, vhdlver, params, files);


    free(files);

    printf("[I] DONE.\n");
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
