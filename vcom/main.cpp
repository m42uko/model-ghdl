#include <iostream>
#include <string>
#include <unistd.h>

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

#define ISOPT(cmd) (i < argc) && (string(argv[i]) == cmd)

int main(int argc, char **argv)
{
    int i;
    string work = ""; // Current library
    string vhdl = ""; // Input VHDL files
    char tempdir[256] = ""; // Compile dir

    getcwd(tempdir, sizeof(tempdir));

    for (i=1; i < argc; ++i) {
        if (ISOPT("-work")) {
            work = argv[i+1];
            ++i;
        }
        else if (ISOPT("-2008")) {
            cerr << "WARN: VHDL 2008 is not yet supported by GHDL." << endl;
        }
        else if (ISOPT("-modelsimini")) {
            // Not used
            ++i; // But skip param
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


    cout << "\n\nVHDL SOURCE DETECTED:" << endl;
    cout << "\twork=" << work << endl;
    cout << "\tvhdl=" << vhdl << endl;
    cout << "\ttempdir=" << tempdir << endl;

    cout << "\n\nCALLING GHDL for syntax checking:" << endl;

    string args;
    args = "ghdl -s --ieee=synopsys --warn-no-vital-generic --workdir=" + string(tempdir) + " --work=" + work + " " + vhdl;

    cout << "\t" << args << endl;
    return 0;
}

