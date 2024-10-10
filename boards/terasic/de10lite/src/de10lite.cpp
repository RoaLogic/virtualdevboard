/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    DE10-Lite Verilator C++ wrapper                              //
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

#include <de10lite.hpp>

using namespace RoaLogic;
using namespace common;
using namespace testbench::clock::units;
using namespace testbench::tasks;


/**
 * @brief Constructor
 * @details Creates class and assigns all signals/ports
 */
cDE10Lite::cDE10Lite(VerilatedContext* context, bool traceActive) :
  cTestBench<Vde10lite_verilator_wrapper>(context, traceActive),
  key(_core->KEY)
{
    /*
      define clocks
     */
    clk_50  = addClock(_core->CLK_50, 20.0_ns);
    clk2_50 = addClock(_core->CLK2_50, 20.0_ns);
    clk_adc_10 = addClock(_core->CLOCK_ADC_10, 100.0_ns);

    /*
      KEY
     */
    key = 0x3;        //KEY has pull-up
}


/**
 * @brief Destructor
 */
cDE10Lite::~cDE10Lite()
{
}


/**
 * @brief Generate reset
 * @details This is a coroutine function that generates the main reset
 *
 * @return The coroutine handle for this function
 */
sCoRoutineHandler<bool> cDE10Lite::Reset()
{
    INFO << "Resetting FPGA\n";

    //KEY[0] is used as asynchronous active low signal

    //wait a while
    for (uint8_t i=0; i<5; i++)
    {
        waitPosEdge(clk_50);
    }


    INFO << "Assert reset\n";
    bitClr8(key,0);
    waitPosEdge(clk_50);

    INFO << "Negate reset\n";
    bitSet8(key,0);

    co_return true;    
}


/**
 * @brief Run testbench
 *
 */
int cDE10Lite::run()
{
    //Reset core
    sCoRoutineHandler reset = Reset();

    long tick_cnt = 0;

    //Run testbench
    while (!finished() && tick_cnt++ < 1600000 *2)
    {
        tick();
    }

    return 0;
}


