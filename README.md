model-ghdl
==========

A wrapper for GHDL to make it look like Mentor's ModelSim. Helpful for use with programs like Sigasi.

Compiling and use
-----------------
First install ghdl from [github](https://github.com/tgingold/ghdl/) and install GtkWave. Model-ghdl expects to find the ghdl and gtkwave executables in the system $PATH.

Then, to compile the wrapper, simply do the following:
```
git clone https://github.com/cclassic/model-ghdl
cmake .
make
```
The five modelsim applications should now be present in the current folder as symlinks along with a model-ghdl binary. Either install these into your path or point your IDE to the location of the files.

VCOM
----
### Original commands
* -work *top*: Set top level
* -workdir *dir*: Set working directory
* -87, -93, -93c, -2000, -2002, -2008: Set VHDL version

### Additional commands
* -ghdl *param*: Supply these arguments directly to GHDL (can be used multiple times)
* -no-precompile: Compile the files not until calling vsim. (Faster syntax check, but longer wait time for the simulation to start.)

### Notes
* *cwd* is expected to be the compile directory
* Recommended arguments: -ghdl --ieee=synopsys -ghdl -fexplicit -ghdl -frelaxed-rules -ghdl -P[/path/to/lib](https://github.com/tgingold/ghdl/tree/master/libraries/vendors)

VSIM
----
### Original commands
* -gui *work.toplevel(RTL)*: Set toplevel and work library

### Additional commands
* -ghdl *param*: Supply these arguments directly to GHDL (can be used multiple times)
* -gtkwprefix *prefix*: Prefix for the .gtkw save file. Path will be $cwd/$prefix$toplevel.gtkw
* -type *extension*: Set simulation output file type. Can be: ghw (default), vcd or fst

### Notes
* *cwd* is expected to be the source directory (used for .gtkw save path)
* Recommended arguments: -gtkwprefix gtkwave/ -ghdl --ieee=synopsys -ghdl -fexplicit -ghdl -frelaxed-rules -ghdl -P[/path/to/lib](https://github.com/tgingold/ghdl/tree/master/libraries/vendors)


VMAP
----
No function.


VLIB
----
No function.


VDEL
----
No function.
