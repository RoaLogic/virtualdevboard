/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Virtual Devboard Verilog wrapper for VGA monitor             //
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

/**
 * @brief VGA Timing
 * @details VGA timing per VESA specs
 *
 * |               |        |       Horizontal (pixels)     |      Vertical (Lines)         |
 * |               | Pixel  |-------------------------------|-------------------------------|
 * |    Format     | clock  | Active | Front | Sync | Back  | Active | Front | Sync | Back  |
 * |               | (MHz)  | video  | porch |      | porch | video  | porch |      | porch |
 * +---------------+--------+--------+-------+------+-------+--------+-------+------+-------+
 * |  640x480,60Hz | 25.175 |   640  |  16   |  96  |  48   |  480   |  11   |  2   |  31   |
 * |  640x480,72Hz | 31.500 |   640  |  24   |  40  | 128   |  480   |   9   |  3   |  28   |
 * |  640x480,75Hz | 31.500 |   640  |  16   |  96  |  48   |  480   |  11   |  2   |  32   |
 * |  640x480,85Hz | 36.000 |   640  |  32   |  48  | 112   |  480   |   1   |  3   |  25   |
 * |  800x600,56Hz | 38.100 |   800  |  32   | 128  | 128   |  600   |   1   |  4   |  14   |
 * |  800x600,60Hz | 40.000 |   800  |  40   | 128  |  88   |  600   |   1   |  4   |  23   |
 * |  800x600,72Hz | 50.000 |   800  |  56   | 120  |  64   |  600   |  37   |  6   |  23   |
 * |  800x600,75Hz | 49.500 |   800  |  16   |  80  | 160   |  600   |   1   |  2   |  21   |
 * |  800x600,85Hz | 56.250 |   800  |  32   |  64  | 152   |  600   |   1   |  3   |  27   |
 * | 1024x768,60Hz | 65.000 |  1024  |  24   | 136  | 160   |  768   |   3   |  6   |  29   |
 * | 1024x768,70Hz | 75.000 |  1024  |  24   | 136  | 144   |  768   |   1   |  6   |  29   |
 * | 1024x768,75Hz | 78.750 |  1024  |  16   |  96  | 176   |  768   |   1   |  3   |  28   |
 * | 1024x768,85Hz | 94.500 |  1024  |  48   |  96  | 208   |  768   |   1   |  3   |  36   |
 */

module vdbVGAMonitor
#(
  /** vdbVGA instance ID
   *  This helps the C++ GUI code to identify the VGA Monitor instance
   */ 
  parameter int ID = 1,

  /** Default horizontal timing
   */
  parameter int HOR_ACT  = 640,
  parameter int HOR_FP   = 16,
  parameter int HOR_SYNC = 96,
  parameter int HOR_BP   = 48,

  /** Default vertical timing
   */
  parameter int VERT_ACT  = 480,
  parameter int VERT_FP   = 11,
  parameter int VERT_SYNC = 2,
  parameter int VERT_BP   = 31
)
(
  input [7:0] r, g, b,
  input       hsync,
  input       vsync
);

  //-----------------------
  // Typedefs
  //
  typedef struct {
    bit [10:0] active;
    bit [ 7:0] front_porch,
               sync,
               back_porch;
  } sync_t;

  typedef struct packed {
    bit [7:0] r,g,b;
  } rgb_t;


  //-----------------------
  // DPI Functions
  //
  //import "DPI-C" context function void vdbVGAMonitorHSYNC(int ID);
  import "DPI-C" context function void vdbVGAMonitorVSYNC(int ID);

  export "DPI-C" task vdbVGAMonitorSetHorizontalTiming;
  task vdbVGAMonitorSetHorizontalTiming (input bit [10:0] pixels, input bit [7:0] fp, input bit [7:0] sync, input bit [7:0] bp);
      vertical.active        = pixels;
      horizontal.front_porch = fp;
      horizontal.sync        = sync;
      horizontal.back_porch  = bp;
$display("Horizontal (Verilog) pixels=%x, fp=%x, sync=%x, bp=%x", pixels, fp, sync, bp);
  endtask

  export "DPI-C" task vdbVGAMonitorSetVerticalTiming;
  task vdbVGAMonitorSetVerticalTiming (input bit [10:0] pixels, input bit [7:0] fp, input bit [7:0] sync, input bit [7:0] bp);
      vertical.active      = pixels;
      vertical.front_porch = fp;
      vertical.sync        = sync;
      vertical.back_porch  = bp;
  endtask


  export "DPI-C" function vdbVGAMonitorGetPixel;
  function rgb_t vdbVGAMonitorGetPixel(input int line, input int pixel);
    return framebuffer[line][pixel];
  endfunction


  //-----------------------
  // Constants
  //
  localparam int MAX_PIXELS   = 1024;
  localparam int MAX_LINES    = 768;
  localparam int TOTAL_PIXELS = MAX_LINES * MAX_PIXELS;
  localparam int PIXEL_BITS   = $bits(rgb_t) * TOTAL_PIXELS;
  localparam int PIXELS_LEN   = $clog2(TOTAL_PIXELS);


  //-----------------------
  // Variables
  //
  wire                   pixel_clk /*verilator public*/;

  logic                  hsync_dly, vsync_dly;
  logic                  hsync_trigger, vsync_trigger;

  sync_t                 horizontal;
  logic [           8:0] hblank_cnt;
  logic [          10:0] hactive_cnt;
  logic                  hactive_video;

  sync_t                 vertical;
  logic [           5:0] vblank_cnt;
  logic [           9:0] vactive_cnt;
  logic                  vactive_video;

  logic                  active_video;
  logic [PIXELS_LEN-1:0] pixel_cnt;

  rgb_t                  framebuffer [TOTAL_PIXELS] /*verilator public*/;

  //-----------------------
  // Module body
  //

  initial
  begin
      horizontal.active      = HOR_ACT;
      horizontal.front_porch = HOR_FP;
      horizontal.sync        = HOR_SYNC;
      horizontal.back_porch  = HOR_BP;

      vertical.active        = VERT_ACT;
      vertical.front_porch   = VERT_FP;
      vertical.sync          = VERT_SYNC;
      vertical.back_porch    = VERT_BP;
  end

  //Notify C++ HSYNC/VSYNC
  //always @(negedge hsync) vdbVGAMonitorHSYNC(ID);
  always @(negedge vsync) vdbVGAMonitorVSYNC(ID);


  //HSYNC/VSYNC triggers (end of HSYNC/VSYNC)
  always @(posedge pixel_clk)
    hsync_dly <= hsync;

  always @(posedge pixel_clk)
    vsync_dly <= vsync;

  assign hsync_trigger = ~hsync & hsync_dly;
  assign vsync_trigger = ~vsync & vsync_dly;

  
  /**
     Time keeping
  */
  assign active_video = vactive_video & hactive_video;

  always @(posedge pixel_clk)
    begin
        if (active_video) pixel_cnt <= pixel_cnt +1;

        /**
            Horizontal
        */
        if (hsync_trigger)
        begin
            hactive_cnt   <= horizontal.active;
            hactive_video <= 1'b0;
            hblank_cnt    <= horizontal.sync + horizontal.back_porch -1;

        end
        else
        begin
            if (hblank_cnt != 0)
            begin
                hblank_cnt <= hblank_cnt -1;
            end
            else if (hactive_cnt != 0)
            begin
                hactive_video <= 1'b1;
                hactive_cnt   <= hactive_cnt -1;
            end
            else
            begin
                hactive_video <= 1'b0;
            end
        end

        /**
            Vertical
        */
        if (vsync_trigger)
        begin
            vactive_cnt   <= vertical.active;
            vactive_video <= 1'b0;
            vblank_cnt    <= vertical.sync + vertical.back_porch -1;
            pixel_cnt     <= {PIXELS_LEN{1'b0}};
        end
        else if (hsync_trigger)
        begin
            if (vblank_cnt != 0)
            begin
                vblank_cnt <= vblank_cnt -1;
            end
            else if (vactive_cnt != 0)
            begin
                vactive_video <= 1'b1;
                vactive_cnt   <= vactive_cnt -1;
            end
            else
            begin
                vactive_video <= 1'b0;
            end
        end
    end


  //store RGB value in frame buffer
  always @(posedge pixel_clk)
    if (active_video) framebuffer[pixel_cnt] <= {r,g,b};
endmodule
