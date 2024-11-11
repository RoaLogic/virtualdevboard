/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    altsyncram Verilator verilog model                           //
//    Functionality is currently very limited                      //
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

//Notes
//
// 1. for byteena width must be a multiple of 5,8,9, 10
//
// 2. Clocking mode
//    Single        all=clock0
//    Read/Write    write=clock0(portA), read=clock1(portB)
//    Input/Output  inputs=clock0, outputs=clock1
//    Independent   portA=clock0, portB=clock1
//
// *****************************************
// legal operations for all operation modes:
//      |  PORT A  |  PORT B  |
//      |  RD  WR  |  RD  WR  |
// BDP  |  x   x   |  x   x   |
// DP   |      x   |  x       |
// SP   |  x   x   |          |
// ROM  |  x       |          |
// *****************************************

//`define DEBUG

/*verilator lint_off UNUSEDPARAM*/
module altsyncram
#(
  parameter     operation_mode                     = "BIDIR",

  //PortA parameters
  parameter int width_a                            = 1,
  parameter int numwords_a                         = 0,
  parameter int widthad_a                          = $clog2(numwords_a),
  parameter int width_byteena_a                    = 1,
  parameter     outdata_reg_a                      = "UNREGISTERED",
  parameter     address_aclr_a                     = "NONE",
  parameter     indata_aclr_a                      = "NONE",
  parameter     outdata_aclr_a                     = "NONE",
  parameter     byteena_aclr_a                     = "NONE",


  //PortB parameters
  parameter int width_b                            = 1,
  parameter int numwords_b                         = 0,
  parameter int widthad_b                          = 1,//$clog2(numwords_b),
  parameter int width_byteena_b                    = 1,
  parameter     rdcontrol_reg_b                    = "CLOCK1",
  parameter     address_reg_b                      = "CLOCK1",
  parameter     outdata_reg_b                      = "UNREGISTERED",
  parameter     indata_reg_b                       = "CLOCK0",
  parameter     byteena_reg_b                      = "CLOCK0",
  parameter     wrcontrol_wraddress_reg_b          = "CLOCK0",
  parameter     address_aclr_b                     = "NONE",
  parameter     indata_aclr_b                      = "NONE",
  parameter     outdata_aclr_b                     = "NONE",
  parameter     rdcontrol_aclr_b                   = "NONE",
  parameter     wrcontrol_aclr_b                   = "NONE",
  parameter     byteena_aclr_b                     = "NONE",

  //Clock Enable parameter
  parameter     clock_enable_input_a               = "BYPASS",
  parameter     clock_enable_output_a              = "BYPASS",
  parameter     clock_enable_input_b               = "BYPASS",
  parameter     clock_enable_output_b              = "BYPASS",

  //Global parameters
  parameter     read_during_write_mode_port_a      = "OLD_DATA",
  parameter     read_during_write_mode_port_b      = "OLD_DATA",
  parameter     read_during_write_mode_mixed_ports = "DONT_CARE",
  parameter int byte_size                          = 8,
  parameter     ram_block_type                     = "AUTO",
  parameter     power_up_uninitialized             = "FALSE",
  parameter     init_file                          = "",
  parameter     init_file_layout                   = "UNUSED",
  parameter     intended_device_family             = "DONT_CARE"
)
(
  // clear inputs on both ports and here are their usage
  // Port A -- 1. all input registers can only be cleared by clear0 or none.
  //           2. output register can be cleared by either clear0, clear1 or none.
  // Port B -- 1. all input registers can be cleared by clear0, clear1 or none.
  //           2. output register can be cleared by either clear0, clear1 or none.
  input  logic                       aclr0,
  input  logic                       aclr1,

  // clock inputs on both ports and here are their usage
  // Port A -- 1. all input registers must be clocked by clock0.
  //           2. output register can be clocked by either clock0, clock1 or none.
  // Port B -- 1. all input registered must be clocked by either clock0 or clock1.
  //           2. output register can be clocked by either clock0, clock1 or none.
  input  logic                       clock0,
  input  logic                       clock1,

  // clock enable inputs and here are their usage
  // clocken0 -- can only be used for enabling clock0.
  // clocken1 -- can only be used for enabling clock1.
  // clocken2 -- as an alternative for enabling clock0.
  // clocken3 -- as an alternative for enabling clock1.
  input  logic                       clocken0,
  input  logic                       clocken1,
  input  logic                       clocken2,
  input  logic                       clocken3,

  input  logic [widthad_a      -1:0] address_a,
  input  logic [width_byteena_a-1:0] byteena_a,
  input  logic                       addressstall_a,
  input  logic                       wren_a,
  input  logic                       rden_a,
  input  logic [width_a        -1:0] data_a,
  output logic [width_a        -1:0] q_a,

  input  logic [widthad_b      -1:0] address_b,
  input  logic [width_byteena_b-1:0] byteena_b,
  input  logic                       addressstall_b,
  input  logic                       wren_b,
  input  logic                       rden_b,
  input  logic [width_b        -1:0] data_b,
  output logic [width_b        -1:0] q_b,

  output logic [                2:0] eccstatus
);

  //-----------------------
  // DPI Functions
  //
`ifdef VERILATOR
  export "DPI-C" function altsyncram_getWidth_a;
  function int altsyncram_getWidth_a();
    return width_a;
  endfunction


  export "DPI-C" function altsyncram_getNumwords_a;
  function int altsyncram_getNumwords_a();
    return numwords_a;
  endfunction


  export "DPI-C" function altsyncram_getMemory;
  function bit [width_a-1:0] altsyncram_getMemory(input int adr);
    return mem_array[adr];
  endfunction


  export "DPI-C" task altsyncram_setMemory;
  task altsyncram_setMemory(input int adr, input bit [width_a-1:0] data);
    mem_array[adr] = data;
`ifdef DEBUG
    $display("mem_array[%x]:%x",adr,data);
`endif
  endtask


  /**
     Initialize altsyncram
  */
  import "DPI-C" context function void altsyncram_initializeInstance(input string init_file, input string init_file_layout);
  initial if (init_file != "") altsyncram_initializeInstance(init_file, init_file_layout);
`endif



  //-----------------------
  // Variables
  //
  logic [width_a  -1:0] mem_array [numwords_a];

  logic                 clk_outdata_a;
  logic                 clken_outdata_a;
  logic                 clk_indata_b;
  logic                 clk_outdata_b;
  logic                 clken_outdata_b;

  logic [widthad_a-1:0] stalled_address_a, adr_a;
  logic [widthad_b-1:0] stalled_address_b, adr_b;
  logic [width_a  -1:0] q_a_int, q_a_reg;
  logic [width_b  -1:0] q_b_int, q_b_reg;

  logic [width_a/8-1:0] bena_a;
  logic [width_b/8-1:0] bena_b;


  //-----------------------
  // Assertions
  //

initial
begin
    PortANumwordsEqPortBNumwords:
    assert (numwords_a == numwords_b)
      else $error ("Numwords for portA and portB must be equal (%m)");


  //only 8bit granularity supported for now
end


  //-----------------------
  // Module Body
  //

  //
  // PortA
  //

  //clock assignments
  assign clk_outdata_a = (outdata_reg_a == "CLOCK1") ? clock1 : clock0;

  //clock enable assignments
  assign clken_outdata_a =    (clock_enable_output_a == "BYPASS"   )                                 ? 1'b1
                           : ((clock_enable_output_a == "ALTERNATE") && (outdata_reg_a == "CLOCK1")) ? clocken3
                           : ((clock_enable_output_a == "ALTERNATE") && (outdata_reg_a == "CLOCK0")) ? clocken2
                           :                                            (outdata_reg_a == "CLOCK1")  ? clocken1
                           :                                            (outdata_reg_a == "CLOCK0")  ? clocken0
                           : 1'b1;

  //byte enables
  assign bena_a = width_byteena_a == 1 ? {$bits(bena_a){byteena_a}} : byteena_a;


  //address
  always @(posedge clock0)
    if (!addressstall_a) stalled_address_a <= address_a;

  assign adr_a = addressstall_a ? stalled_address_a : address_a;

  
  //Write, yes blocking statement
  always @(posedge clock0)
    for (int b=0; b < (width_a + 7)/8; b++)
    if (wren_a && bena_a[b])
      /*verilator lint_off BLKSEQ */
      mem_array[adr_a][b*8+:8] = data_a[b*8+:8];
      /*verilator lint_on BLKSEQ */


  //Read
  always @(posedge clk_outdata_a)
    if (rden_a) q_a_int <= mem_array[adr_a];


  always @(posedge clk_outdata_a)
    if (clken_outdata_a) q_a_reg <= q_a_int;


  assign q_a = (outdata_reg_a == "UNREGISTERED") ? q_a_int : q_a_reg;


  //
  // PortB
  //
  //clock assignments
  assign clk_indata_b  =  (operation_mode != "BIDIR_DUAL_PORT" && operation_mode != "DUAL_PORT") ? clock0
                         :(address_reg_b == "CLOCK0"                                           ) ? clock0
			 :                                                                         clock1;

  assign clk_outdata_b = (outdata_reg_b == "CLOCK1") ? clock1 : clock0;

  //clock enable assignments
  assign clken_outdata_b =    (clock_enable_output_b == "BYPASS"   )                                 ? 1'b1
                           : ((clock_enable_output_b == "ALTERNATE") && (outdata_reg_b == "CLOCK1")) ? clocken3
                           : ((clock_enable_output_b == "ALTERNATE") && (outdata_reg_b == "CLOCK0")) ? clocken2
                           :                                            (outdata_reg_b == "CLOCK1")  ? clocken1
                           :                                            (outdata_reg_b == "CLOCK0")  ? clocken0
                           : 1'b1;

  //byte enables
  assign bena_b = width_byteena_b == 1 ? {$bits(bena_b){byteena_b}} : byteena_b;


  //address
  always @(posedge clk_indata_b)
    if (!addressstall_b) stalled_address_b <= address_b;

  assign adr_b = addressstall_b ? stalled_address_b : address_b;

generate
if (operation_mode == "BIDIR_DUAL_PORT")

  //Write, yes blocking statement
  always @(posedge clk_indata_b)
    for (int b=0; b < (width_b + 7)/8; b++)
    if (wren_b && bena_b[b])
      /*verilator lint_off BLKSEQ */
      mem_array[adr_b][b*8+:8] = data_b[b*8+:8];
      /*verilator lint_on BLKSEQ */
endgenerate


  //Read
  always @(posedge clk_outdata_b)
    if (rden_b) q_b_int <= mem_array[adr_b];


  always @(posedge clk_outdata_b)
    if (clken_outdata_b) q_b_reg <= q_b_int;


  assign q_b = (outdata_reg_b == "UNREGISTERED") ? q_b_int : q_b_reg;




  //
  // ECC
  //
  assign eccstatus = {$bits(eccstatus){1'b0}};
endmodule

/*verilator lint_on UNUSEDPARAM*/
