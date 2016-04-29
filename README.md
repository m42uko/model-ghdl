model-ghdl
==========

A wrapper for GHDL to make it look like Mentor's ModelSim. Helpful for use with programs like Sigasi.


VCOM
----
### Original commands
* -work <top>: Set top level
* -workdir <dir>: Set working directory
* -87, -93, -93c, -2000, -2002, -2008: Set VHDL version

### Additional commands
* -ghdl <param>: Supply these arguments directly to GHDL (can be used multiple times)

### Notes
* cwd is expected to be the compile directory


VSIM
----
### Original commands
* -gui work.toplevel(RTL): Set toplevel and work library

### Additional commands
* -ghdl <param>: Supply these arguments directly to GHDL (can be used multiple times)
* -gtkwprefix: Prefix for the .gtkw save file. Path will be $cwd/$prefix$toplevel.gtkw

### Notes
* cwd is expected to be the source directory (used for .gtkw save path)
