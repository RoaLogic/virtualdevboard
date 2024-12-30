# virtualdevboard
## Graphical Virtual SoC Development Board Builder using Verilator
virtualdevboard is a framework for creating virtual development boards.
A virtual development board is an FPGA/ASIC development board that is entirely emulated on a PC.
The FPGA/ASIC logic is converted into cycle accurate C++ models using Verilator.
The peripherals (other components) on the board are emulated using graphical components (e.g. LEDs, 7-segment Display, VGA), abstracted (e.g. SDRAM memories), or by accessing the host PCs hardware and OS resources directly (e.g. UART, ethernet, ...)

# Prerequisites
* Verilator
* gmake
* C++20 capable compiler
* wxWidgets 3.2.6 or newer
* Doxygen
* Graphviz

# Software design
The software consists out of two different components, one is the FPGA/ASIC logic, which is translated into C++ code with verilator. The
second part is the GUI with the virtual components. The verilated design and the GUI run in a different thread and have different methods 
to communicate between the threads, it is possible to run the design without the use of the GUI.

Full software documentation can be found [here](https://roalogic.github.io/virtualdevboard/).

## Verilated design

All FPGA/ASIC logic is converted into C++ code with verilator, this is then controlled from the virtual development board. The design
is controlled through the main testbench and holds the complete timing interface of the design. It cycles step by step through the
design by changing the clocks when needed. This means that it evaluates the next clock edge, toggles the clock and evaluates the 
full design. Which is then repeated untill a finish is received or the simulation is stopped. 

## GUI design

The verilated design communicates with the GUI through a specific interface. This interface is introduced so that it's possible to change the 
underlaying GUI framework. In the main design of the GUI the general control is instantiated, with this comes the start, pause and stop button.

## Peripherals

Peripherals are created as virtual components. Those components can be connected with the verilated design and/or shown on the GUI, this is 
specific to the peripheral and the usage of it. Within the initialization file it is determined where a component is placed in the GUI, which
is always relative to the top left corner.

# Folder structure
