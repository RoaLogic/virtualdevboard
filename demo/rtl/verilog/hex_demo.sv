/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    TerAsic DE10-Lite Demo for VirtualDevBoard - HEX             //
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

/** 7-Segment clock HH.MM.SS demo
 *
 * 7-segment mapping
 *
 *   ---           in[0]
 *  |   |    in[5]       in[1]
 *   ---           in[6]
 *  |   |    in[4]       in[2]
 *   ---  *        in[3]       in[7]
 *
 *
 *  examples
 *
 *      |
 *
 *      |
 *        in=8'b00000110=8'h06
 *
 *   --- 
 *      |
 *   ---
 *  |
 *   ---  in=8'b01011011=8'h5b
 *
 *   ---
 *      |
 *   ---
 *      |
 *   ---  in=8'b01001111=8'h4f
 */

module hex_demo
(
  input  wire       rst_ni,
  //Clocks
  input  wire       CLK_50,  //50MHz clock

  //7-segment displays
  output reg [ 7:0] HEX0,
                    HEX1,
                    HEX2,
                    HEX3,
                    HEX4,
                    HEX5
);
  //-------------------------------
  // constants
  //
  localparam int HZ          = 26'd50_000; //_000 -1; //50 million ticks per second 
  localparam int SEC_PER_DAY = 60*60*24; 

  //-------------------------------
  // Functions
  //
  function automatic [7:0] hex2_7segment (input [3:0] hex);
    case (hex)
      4'd0 : hex2_7segment = 8'b00111111;
      4'd1 : hex2_7segment = 8'b00000110;
      4'd2 : hex2_7segment = 8'b01011011;
      4'd3 : hex2_7segment = 8'b01001111;
      4'd4 : hex2_7segment = 8'b01100110;
      4'd5 : hex2_7segment = 8'b01101101;
      4'd6 : hex2_7segment = 8'b01111101;
      4'd7 : hex2_7segment = 8'b00000111;
      4'd8 : hex2_7segment = 8'b01111111;
      4'd9 : hex2_7segment = 8'b01101111;
      4'd10: hex2_7segment = 8'b01110111;
      4'd11: hex2_7segment = 8'b01111100;
      4'd12: hex2_7segment = 8'b00111001;
      4'd13: hex2_7segment = 8'b01011110;
      4'd14: hex2_7segment = 8'b01111001;
      4'd15: hex2_7segment = 8'b01110001;
    endcase
  endfunction

  //-------------------------------
  // Variables
  //
  logic [25:0] ena_1sec_cnt;
  logic        ckena_1sec;

  logic [ 7:0] seconds;
  logic [ 7:0] minutes;
  logic [ 7:0] hours;


  //-------------------------------
  // clock body
  //
  assign ckena_1sec = ~|ena_1sec_cnt;

  always @(posedge CLK_50, negedge rst_ni)
    if      (!rst_ni    ) ena_1sec_cnt <= HZ;
    else if ( ckena_1sec) ena_1sec_cnt <= HZ;
    else                  ena_1sec_cnt <= ena_1sec_cnt -1;

  always @(posedge CLK_50, negedge rst_ni)
    if      (!rst_ni    ) seconds <= 'h0;
    else if ( ckena_1sec)
      if (seconds == 59)  seconds <= 'h0;
      else                seconds <= seconds +'h1;


  always @(posedge CLK_50, negedge rst_ni)
    if      (!rst_ni    ) minutes <= 'h0;
    else if ( ckena_1sec   &&
              seconds == 59)
      if (minutes == 59)  minutes <= 'h0;
      else                minutes <= minutes +'h1;


  always @(posedge CLK_50, negedge rst_ni)
    if      (!rst_ni    ) hours <= 'h0;
    else if ( ckena_1sec    &&
              seconds == 59 &&
              minutes == 59 )
      if (hours   == 23)  hours <= 'h0;
      else                hours <= hours +'h1;


  //BCD
  always @(posedge CLK_50)
    begin
        HEX0 <= hex2_7segment(seconds[3:0]);
        HEX1 <= hex2_7segment(seconds[7:4]);
        HEX2 <= hex2_7segment(minutes[3:0]);
        HEX3 <= hex2_7segment(minutes[7:4]);
        HEX4 <= hex2_7segment(hours  [3:0]);
        HEX5 <= hex2_7segment(hours  [7:4]);
    end
endmodule : hex_demo
