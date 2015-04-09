model-ghdl
==========

A wrapper for GHDL to make it look like Mentor's ModelSim. Helpful for use with programs like Sigasi.

## Tips
Add the following arguments in Sigasi:

### vsim
* ```-ghdl "--std=08"``` to enable VHDL 2008 support (if any problems occur)
* ```-gtkwave "--save=/path/to/your/savefile"``` to automatically load a specific wave layout
