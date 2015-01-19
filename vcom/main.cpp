#include <iostream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <fstream>
//#include <regex>

//#define DEBUG_EN DEBUG_EN


/*
 *
> /tmp/SigasiCompileCache822149657706169838/blink/vcom > /home/markus/Downloads/Libero/install/Model/modeltech/linuxacoem/vcom -2008 -work design -modelsimini /tmp/SigasiCompileCache822149657706169838/blink/vcom/modelsim.ini /home/markus/workspaceSigasi/blink/src/top.vhd
Model Technology ModelSim Microsemi vcom 10.3a Compiler 2014.04 Apr 15 2014
Start time: 19:57:50 on Jan 18,2015
vcom -2008 -work design -modelsimini /tmp/SigasiCompileCache822149657706169838/blink/vcom/modelsim.ini /home/markus/workspaceSigasi/blink/src/top.vhd
-- Loading package STANDARD
-- Loading package TEXTIO
-- Loading package std_logic_1164
-- Loading package NUMERIC_STD
-- Compiling entity top
-- Compiling architecture RTL of top
** Error: /home/markus/workspaceSigasi/blink/src/top.vhd(32): (vcom-1136) Unknown identifier "counter_i2".
** Error: /home/markus/workspaceSigasi/blink/src/top.vhd(33): VHDL Compiler exiting
End time: 19:57:50 on Jan 18,2015, Elapsed time: 0: 0: 0
Errors: 2, Warnings: 0



COMPILATION:
> ghdl -i --ieee=synopsys --warn-no-vital-generic --workdir=. --work=design ../blink/src/ *.vhd
> ghdl -m --ieee=synopsys --warn-no-vital-generic --workdir=. --work=work top
../blink/src/top.vhd:32:19: no declaration for "counter_i2"


SYNTAX CHECKING:
> ghdl -s --ieee=synopsys --warn-no-vital-generic --workdir=. --work=design ../blink/src/ *.vhd
../blink/src/top.vhd:32:19: no declaration for "counter_i2"


echo "EXTERNAL: $# $@"

vars=`echo "$@" | sed "s/-work /-work=/g"`;

*/


using namespace std;

#define ISOPT(cmd) (string(argv[i]) == cmd)
#define GETOPT(cmd) (string(argv[i]) == cmd) && (++i < argc)

int run(string args);

int run(string args) {
    FILE *proc;
    char buf[512];
    vector < string > result;

    proc = popen(args.c_str(), "r");

    if (proc == NULL) {
        cerr << "Error: Could not invoke GHDL." << endl;
        return 1;
    }

    while(fgets(buf, sizeof(buf), proc)!=NULL){
        //cout << buf;
        string temp = "";
        char *ptr = buf;

        result.clear();

        while (*ptr != '\0') {
            if (*ptr == ':' || *ptr == '\0') {
                result.push_back(temp);
                temp = "";
            }
            else {
                temp.append(" ");
                temp[temp.length()-1] = *ptr;
            }
            ptr++;
        }
        result.push_back(temp);

        if (result.size() == 4) {
            cout << "** Error: " << result[0] << "(" << result[1] << "):" << result[3];
        }
        else {
            cout << buf;
        }
    }
    pclose(proc);
    return 0;
}

int main(int argc, char **argv)
{
    int i;
    string work = ""; // Current library
    string vhdl = ""; // Input VHDL files
    char tempdir[256] = ""; // Compile dir

    if (!getcwd(tempdir, sizeof(tempdir))) {
        cerr << "Error getting current working dir!" << endl;
        return 1;
    }

    for (i=1; i < argc; ++i) {
        if (GETOPT("-work")) {
            work = argv[i];
            //++i;
        }
        else if (ISOPT("-93")) {

        }
        else if (ISOPT("-2008")) {
            cerr << "WARN: VHDL 2008 is not yet supported by GHDL." << endl;
        }
        else if (GETOPT("-modelsimini")) {
            // Not used
           // ++i; // But skip param
        }
        else {
            if (argv[i][0] == '-') {
                cerr << "INFO: Unknown command line opt: " << argv[i] << endl;
            }
            else {
                break; // Rest are VHDL input files
            }
        }
    }

    for (; i < argc; ++i) {
        vhdl.append(argv[i]);
        vhdl.append(" ");
    }

#ifdef DEBUG_EN
    cout << "\n\nVCOM CALL PARSED:" << endl;
    cout << "\twork=" << work << endl;
    cout << "\tvhdl=" << vhdl << endl;
    cout << "\ttempdir=" << tempdir << endl;
#endif

    if (work == "" || vhdl == "" || string(tempdir) == "") {
        cerr << "Error: Incomplete/Unsupported vcom call." << endl;
        return 2;
    }

    // Write temp path to /tmp
    ofstream myfile;
    myfile.open ("/tmp/workdir-ghdl");
    myfile << tempdir;
    myfile.flush();
    myfile.close();

    string cargs = "ghdl -i --ieee=synopsys --warn-no-vital-generic --workdir=" + string(tempdir) + " --work=" + work + " " + vhdl + " 2>&1";
    string sargs = "ghdl -s --ieee=synopsys --warn-no-vital-generic --workdir=" + string(tempdir) + " --work=" + work + " " + vhdl + " 2>&1";

    // Launch GHDL
    if (run(cargs) || run(sargs)) {
        cerr << "** Error: Error in model-ghdl." << endl;
    }
    return 0;
}

