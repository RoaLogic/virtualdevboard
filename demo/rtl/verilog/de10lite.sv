/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    TerAsic DE10-Lite Demo for VirtualDevBoard                   //
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

module de10lite
(
  //Clocks
  input  wire         CLK_50,
  input  wire         CLK2_50,
  input  wire         CLOCK_ADC_10,

  //Arduino
  output              ARDUINO_RESET_N,
  inout        [15:0] ARDUINO_IO,

  //Accelerometer
  output              G_SENSOR_SCLK,
  output              G_SENSOR_CS_N,
  output              G_SENSOR_SDO,
  input               G_SENSOR_SDI,
  input        [ 2:1] G_SENSOR_INT,

  //GPIO
  inout        [35:0] GPIO,

  //7-Segment display
  output logic [ 7:0] HEX0,
                      HEX1,
                      HEX2,
                      HEX3,
                      HEX4,
                      HEX5,

  //Key
  //KEY[0] is used as async system reset
  input  wire  [ 1:0] KEY,

  //LED
  output logic [ 9:0] LEDR,

  //SDRAM
  output              DRAM_CLK,
  output              DRAM_CKE,
  output              DRAM_CS_N,
  output              DRAM_RAS_N,
  output              DRAM_CAS_N,
  output              DRAM_WE_N,
  output       [ 1:0] DRAM_BA,
  output       [12:0] DRAM_ADDR,
  output              DRAM_UDQM,
  inout        [15:0] DRAM_DQ,

  //Switches
  input        [ 9:0] SW,

  //VGA
  output       [ 3:0] VGA_R,
                      VGA_G,
                      VGA_B,
  output              VGA_HS,
  output              VGA_VS
);
  //-------------------------------
  // Key[0] is used as async active low reset
  //
  wire rst_n;
  assign rst_n = KEY[0];


  //-------------------------------
  // 2.5Hz trigger for LEDs
  //
  logic [24:0] ena2p5_cnt;
  logic        ena2p5;

  always @(posedge CLK_50, negedge rst_n)
    if      (!rst_n  ) ena2p5_cnt <= 25'd20_000;
    else if ( ena2p5 ) ena2p5_cnt <= 25'd20_000;
    else               ena2p5_cnt <= ena2p5_cnt -'h1;

  assign ena2p5 = ~|ena2p5_cnt;


  //-------------------------------
  // LEDs
  // Simply toggle the LEDs
  //
  always @(posedge CLK_50, negedge rst_n)
    if      (!rst_n ) LEDR <= 'h0;
    else if ( ena2p5) LEDR <= LEDR + 9'h1;



  //-------------------------------
  // TODO
  // Dummy altsyncram to make DPI bindings happy
  //
  wire [7:0] top_mem_q;
  altsyncram #(
    .numwords_a (1),
    .width_a    (8))
  top_mem (
    .clock0 (CLK_50),
    .data_a ( top_mem_q ),
    .q_a    ( top_mem_q )
  );

endmodule : de10lite
