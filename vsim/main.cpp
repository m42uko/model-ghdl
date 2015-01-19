#include <iostream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <vector>

#define DEBUG_EN DEBUG_EN

using namespace std;

// Source: vsim -gui design.top(RTL)
// Target: ghdl -m --ieee=synopsys --warn-no-vital-generic --workdir=simu --work=work testb_file
//         ./testb_file --stop-time=500ns --vcdgz=testb_file.vcdgz
//         gunzip --stdout testb_file.vcdgz | gtkwave --vcd

int run(string args);

int run(string args) {
    FILE *proc;
    char buf[512];
    vector < string > result;

    proc = popen(args.c_str(), "r");

    if (proc == NULL) {
        cerr << "Error: Could not invoke GHDL/GtkWave." << endl;
        return 1;
    }

    while(fgets(buf, sizeof(buf), proc)!=NULL){
        cout << buf;
    }
    pclose(proc);
    return 0;
}

#define ISOPT(cmd) (string(argv[i]) == cmd)
#define GETOPT(cmd) (string(argv[i]) == cmd) && (++i < argc)

int main(int argc, char **argv) {
    string top = "";
    string work = "";
    int i;
    char tempdir[256] = ""; // Compile dir

    if (!getcwd(tempdir, sizeof(tempdir))) {
        cerr << "Error getting current working dir!" << endl;
        return 1;
    }

    for (i=1; i < argc; ++i) {
        if (ISOPT("-gui")) {

        }
        else {
            if (argv[i][0] == '-') {
                cerr << "INFO: Unknown command line opt: " << argv[i] << endl;
            }
            else {
                break;
            }
        }
    }

    for (; i < argc; ++i) {
        top.append(argv[i]);
        top.append(" ");
    }


    // -gui work.toplevel(RTL)
    string temp = "";
    for (unsigned int i=0; i < top.length(); ++i) {
        if (top.at(i) == '.') {
            work = temp;
            temp = "";
        }
        else if (top.at(i) == '(') {
            top = temp;
            temp = "";
            break;
        }
        else {
            temp.append(" ");
            temp[temp.length()-1] = top.at(i);
        }
    }

#ifdef DEBUG_EN
    cout << "\n\nVSIM CALL PARSED:" << endl;
    cout << "\twork=" << work << endl;
    cout << "\ttop=" << top<< endl;
    cout << "\ttempdir=" << tempdir << endl;
#endif

    if (work == "" || top == "" || string(tempdir) == "") {
        cerr << "Error: Incomplete/Unsupported vsim call." << endl;
        return 2;
    }

    string cargs = "ghdl -m --ieee=synopsys --warn-no-vital-generic --workdir=" + string(tempdir) + " --work=" + work + " " + top;
    if (!run(cargs)) {
        cerr << "Error: Compilation failed." << endl;
    }
    else {
        cargs = "";
        // ./testb_file --stop-time=500ns --vcdgz=testb_file.vcdgz
        if (run("./" + top + " --stop-time=500ns --vcdgz=" + top + ".vcdgz")) {
            cerr << "Error: Simulation failed." << endl;
        }
    }

    return 0;
}

