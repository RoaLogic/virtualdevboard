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
The virtual development board has three main software components. First is the FPGA/ASIC logic, which is translated into C++ code with Verilator. The verilated design can then be controlled by our C++ application, which is done by the testbench, see @ref CTestbench for more information about this part.

Second part are the virtual development board (vdb) components, those are C++ components with specific logic to connect with the verilated design. Those translate the verilated signals into real components, like LED's, 7 segment displays or VGA data. See @ref cVDBCommon for more information.

Last part is the GUI, which is optional and has a specific interface. This interface is added so that it is possible to use different frameworks or eventual just a terminal. The GUI exists out of the main design/main frame and vdb component implementations. Every vdb component shall have it's own GUI implementation. See @ref vdbComponent_1 

# Folder structure
