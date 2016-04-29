model-ghdl
==========

A wrapper for GHDL to make it look like Mentor's ModelSim. Helpful for use with programs like Sigasi.

Compiling and use
-----------------
First install ghdl from [ghdl-updates](https://sourceforge.net/projects/ghdl-updates/). Model-ghdl expects to find the ghdl executable in the $PATH.

Then, to compile the wrapper, simply do the following:
```
git clone <git-url>
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

### Notes
* *cwd* is expected to be the compile directory


VSIM
----
### Original commands
* -gui *work.toplevel(RTL)*: Set toplevel and work library

### Additional commands
* -ghdl *param*: Supply these arguments directly to GHDL (can be used multiple times)
* -gtkwprefix *prefix*: Prefix for the .gtkw save file. Path will be $cwd/$prefix$toplevel.gtkw

### Notes
* *cwd* is expected to be the source directory (used for .gtkw save path)


VMAP
----
No function.


VLIB
----
No function.


VDEL
----
No function.
