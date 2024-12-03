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


