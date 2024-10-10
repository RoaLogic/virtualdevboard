/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    DE10-Lite Verilator Verilog wrapper                          //
//                                                                 //
/////////////////////////////////////////////////////////////////////
//                                                                 //
//    Copyright (C) 2024 Roa Logic BV - www.roalogic.com           //
//    Copyright (C) 2024 richard.herveille@roalogic.com            //
//                                                                 //
//     Redistribution and use in source and binary forms, with     //
//   or without modification, are permitted provided that the      //
//   following conditions are met:                                 //
//   1. Redistributions of source code must retain the above       //
//      copyright notice, this list of conditions and the          //
//      following disclaimer.                                      //
//   2. Redistributions in binary form must reproduce the above    //
//      copyright notice, this list of conditions and the          //
//      following disclaimer in the documentation and/or other     //
//      materials provided with the distribution.                  //
//   3. Neither the name of the copyright holder nor the names     //
//      of its contributors may be used to endorse or promote      //
//      products derived from this software without specific       //
//      prior written permission.                                  //
//                                                                 //
//     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND      //
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,   //
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF      //
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE      //
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR         //
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  //
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT  //
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;  //
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)      //
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     //
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE     //
//   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS       //
//   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  //
//                                                                 //
/////////////////////////////////////////////////////////////////////

module de10lite_verilator_wrapper
#(
  parameter int OCRAM_SIZE    = 64, //in kB
  parameter     INIT_FILE     = "../../bench/blinky_noirq.mif"
)
(
  //Clocks
  input         CLK_50,
  input         CLK2_50,
  input         CLOCK_ADC_10,

  //Arduino
  output        ARDUINO_RESET_N,
  inout  [15:0] ARDUINO_IO,

  //Accelerometer
  output        G_SENSOR_SCLK,
  output        G_SENSOR_CS_N,
  output        G_SENSOR_SDO,
  input         G_SENSOR_SDI,
  input  [ 2:1] G_SENSOR_INT,

  //GPIO
  inout  [35:0] GPIO,

  //7-Segment display
  output [ 7:0] HEX0,
                HEX1,
                HEX2,
                HEX3,
                HEX4,
                HEX5,

  //Key
  //KEY[0] is used as async system reset
  input  [ 1:0] KEY,

  //LED
  output [ 9:0] LEDR,

  //SDRAM
  output        DRAM_CLK,
  output        DRAM_CKE,
  output        DRAM_CS_N,
  output        DRAM_RAS_N,
  output        DRAM_CAS_N,
  output        DRAM_WE_N,
  output [ 1:0] DRAM_BA,
  output [12:0] DRAM_ADDR,
  output        DRAM_UDQM,
  inout  [15:0] DRAM_DQ,

  //Switches
  input  [ 9:0] SW,

  //VGA
  output [ 3:0] VGA_R,
                VGA_G,
                VGA_B,
  output        VGA_HS,
  output        VGA_VS
);

  //-------------------------------
  // Local parameters
  //


  //-------------------------------
  // Variables
  //


  //-------------------------------
  // Hookup DE10Lite Design
  //
  de10lite #(
  .OCRAM_SIZE        ( OCRAM_SIZE   ),
  .INIT_FILE         ( INIT_FILE    ),

  .Zicsr_ENA         ( 1'b1         ),
  .Zicntr_ENA        ( 1'b1         ),
  .Zmmul_ENA         ( 1'b1         ),
  .Zba_ENA           ( 1'b1         ),
  .Zbb_ENA           ( 1'b1         ),
  .Zbs_ENA           ( 1'b1         ),

  .TARGET_AREA       ( 1'b1         ),
  .FAST_SHIFT        ( 1'b1         ))
  de10lite_inst (
    //Clocks
    .CLK_50          ( CLK_50       ),
    .CLK2_50         ( CLK2_50      ),
    .CLOCK_ADC_10    ( CLOCK_ADC_10 ),

    //Arduino
    .ARDUINO_RESET_N (),
    .ARDUINO_IO      (),

    //Accelerometer
    .G_SENSOR_SCLK   (),
    .G_SENSOR_CS_N   (),
    .G_SENSOR_SDO    (),
    .G_SENSOR_SDI    (),
    .G_SENSOR_INT    (),

    //GPIO
    .GPIO            (),

    //7-Segment display
    .HEX0            (),
    .HEX1            (),
    .HEX2            (),
    .HEX3            (),
    .HEX4            (),
    .HEX5            (),

    //Key
    //KEY[0] is used as async system reset
    .KEY             ( KEY ),

    //LED
    .LEDR            (),

    //SDRAM
    .DRAM_CLK        (),
    .DRAM_CKE        (),
    .DRAM_CS_N       (),
    .DRAM_RAS_N      (),
    .DRAM_CAS_N      (),
    .DRAM_WE_N       (),
    .DRAM_BA         (),
    .DRAM_ADDR       (),
    .DRAM_UDQM       (),
    .DRAM_DQ         (),

    //Switches
    .SW              (),

    //VGA
    .VGA_R           (),
    .VGA_G           (),
    .VGA_B           (),
    .VGA_HS          (),
    .VGA_VS          ());



endmodule : de10lite_verilator_wrapper