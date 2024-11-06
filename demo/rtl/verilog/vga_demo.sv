/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    TerAsic DE10-Lite Demo for VirtualDevBoard - VGA             //
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

/*
            _______________  front-porch  sync  back-porch
  ---------X active video  X-------------|    |---------
            ---------------              |____|
*/

module vga_demo
#(
  parameter int HWIDTH  = 640,  //horizontal visible pixels
  parameter int HFPORCH = 16,   //horizontal front porch
  parameter int HSYNC   = 96,   //horizontal sync
  parameter int HBPORCH = 48,   //horizontal back porch

  parameter int VWIDTH  = 480,  //vertical visible pixels
  parameter int VFPORCH = 11,   //vertical front porch
  parameter int VSYNC   = 2,    //vertical sync
  parameter int VBPORCH = 31,   //vertical back porch

  parameter int RADIUS = 30     //circle radius
)
(
  //Reset
  input  wire         rst_n,    //async.active low reset
  //Clocks
  input  wire         clk,      //pixel clock

  //VGA
  output       [ 3:0] VGA_R,
                      VGA_G,
                      VGA_B,
  output logic        VGA_HS,
  output logic        VGA_VS
);
  //-------------------------------
  // constants
  //
  localparam int HTOTAL = HWIDTH + HFPORCH + HSYNC + HBPORCH;
  localparam int VTOTAL = VWIDTH + VFPORCH + VSYNC + VBPORCH;
  localparam int HTOTAL_LEN = $clog2(HTOTAL);
  localparam int VTOTAL_LEN = $clog2(VTOTAL);


  //-------------------------------
  // VGA timing
  //
  logic [HTOTAL_LEN-1:0] hcnt;
  logic [VTOTAL_LEN-1:0] vcnt;

  //definitely not efficient, but explains principle
  always @(posedge clk, negedge rst_n)
    if (!rst_n)
    begin
        hcnt   <= {HTOTAL_LEN{1'b0}};
        vcnt   <= {VTOTAL_LEN{1'b0}};
        VGA_HS <= 1'b1;
        VGA_VS <= 1'b1;
    end
    else
    begin
        //horizontal/vertical counters
        if (hcnt == HTOTAL -1)
        begin
            hcnt <= {HTOTAL_LEN{1'b0}};

            if (vcnt == VTOTAL -1) vcnt <= {VTOTAL_LEN{1'b0}};
            else                   vcnt <= vcnt +1;
        end
        else
        begin
            hcnt <= hcnt +1;
        end

        //control signals
        VGA_HS <= ~((hcnt >= HWIDTH + HFPORCH) &&
                    (hcnt <  HWIDTH + HFPORCH + HSYNC)
                   );

        VGA_VS <= ~((vcnt >= VWIDTH + VFPORCH) &&
                    (vcnt <  VWIDTH + VFPORCH + VSYNC)
                   );
    end


  //-------------------------------
  // Draw moving circle
  //
  localparam COLOUR_BLACK  = 16'h0000; //Black
  localparam COLOUR_CIRCLE = 16'h2FE0; //Bright Green
  localparam COLOUR_BORDER = 16'h029F; //Blue

  //16bit rgb
  logic [15:0] rgb;

  //position of the circle (centre)
  logic [HTOTAL_LEN-1:0] circle_x;
  logic [VTOTAL_LEN-1:0] circle_y;

  //circle movement direction
  logic direction_x; //0=left-to-right, 1=right-to-left
  logic direction_y; //0=top-to-bottom, 1=bottom-to-top

  //update direction on vsync
  logic vsync_dly;
  logic update_circle_pos;
  logic draw_circle;

  always @(posedge clk)
    vsync_dly <= VGA_VS;

  assign update_circle_pos = VGA_VS & ~vsync_dly;


  //should we draw the circle?
  assign draw_circle = (circle_x - hcnt) * (circle_x - hcnt) +
                       (circle_y - vcnt) * (circle_y - vcnt) <= RADIUS * RADIUS;

  //Draw
  always @(posedge clk, negedge rst_n)
    if (!rst_n)
    begin
        circle_x    <= RADIUS;
        circle_y    <= RADIUS;
        direction_x <= 1'b0;
        direction_y <= 1'b0;
    end
    else
    begin
        // Should we draw the border?
        if      (vcnt == 0  ) rgb <= COLOUR_BORDER;
        // Should we draw the circle?
        else if (draw_circle) rgb <= COLOUR_CIRCLE;
        // Draw background
        else                  rgb <= COLOUR_BLACK;

        //move circle
        if (update_circle_pos)
        begin
            circle_x <= direction_x ? circle_x -'h5 : circle_y + 'h5;
            circle_y <= direction_y ? circle_y -'h5 : circle_y + 'h5;
        end

       //check boundaries
       if (!direction_x) //moving left-to-right
       begin
           //if we're at the right edge, start moving right-to-left
           if ((circle_x + RADIUS) >= HWIDTH-1) direction_x <= 1'b1;
       end
       else
       begin
           //if we're at the left edge, start moving left-to-right
           if ((circle_x - RADIUS) <= 0) direction_x <= 1'b0;
       end

       if (!direction_y) //moving top-to-bottom
       begin
           //if we're at the bottom edge, start moving bottom-to-top
           if ((circle_y + RADIUS) >= VWIDTH-1) direction_y <= 1'b1;
       end
       else
       begin
           //if we're at the top edge, start moving top-to-bottom
           if ((circle_y - RADIUS) <= 0) direction_y <= 1'b0;
       end
    end


  //assign R,G,B
  //RGB16={R[4:0],G[5:0],B[4:0]}
  assign VGA_R = {rgb[11+:5],3'h0} >> 4;
  assign VGA_G = {rgb[5 +:6],2'h0} >> 4;
  assign VGA_B = {rgb[0 +:5],3'h0} >> 4;
endmodule : vga_demo
