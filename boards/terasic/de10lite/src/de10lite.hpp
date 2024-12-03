/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    DE10-Lite Verilator C++ header file                          //
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

//for std::unique_ptr
#include <memory>

//for assertions
#include <cassert>

//testbench class
#include <testbench.hpp>

#include "gui_interface.hpp"

//model header, generated by verilator
#include "Vde10lite_verilator_wrapper.h"
#include "Vde10lite_verilator_wrapper__Dpi.h"
#include "Vde10lite_verilator_wrapper_de10lite_verilator_wrapper.h"
#include "Vde10lite_verilator_wrapper_vdbVGAMonitor.h"

//altsyncram class
#include "altsyncram.hpp"

#include "vdbVGAMonitor.hpp"
#include "vdbLED.hpp"
#include "vdb7SegmentDisplay.hpp"


using namespace RoaLogic;
using namespace testbench;
using namespace tasks;
using namespace clock;
using namespace GUI;
using namespace observer;
using namespace vdb;

enum class eRunState
{
    completed,
    restart
};

/**
 * @class cDE10Lite
 * @author Richard Herveille, Bjorn Schouteten
 * @brief Verilog C++ wrapper for DE10-Lite
 * @version 0.1
 * @date 24-sept-2024
 *
 * @details This class wraps around the DE10-Lite verilog wrapper and provides the C++ functions
 * 
 * This object controls the DE10Lite wrapper in a specific way. It has knowledge of all the
 * high level connections and how to control the DUT. Tests can be added and run without the context 
 * of this class. 
 * 
 * It is derived from the cTestBench to have a general testbench control
 */

class cDE10Lite : public cTestBench<Vde10lite_verilator_wrapper>, public cObserver
{
    private:
        static const uint8_t _cNumLed = 10;
        static const uint8_t _cNum7Seg = 6;
        cGuiInterface* _myGUI = nullptr;
        //DE10-Lite ports. Standard ports are of type uint8_t
        cClock* clk_50;
        cClock* clk2_50;
        cClock* clk_adc_10;
        cClock* clk_vga;
        uint8_t& key;

        cVdbVGAMonitor* _vgaController;
        cVdbLed* _ledInstances[_cNumLed];
        cVdb7SegmentDisplay* _7segInstances[_cNum7Seg];

        std::atomic<eRunState> _returnState = eRunState::completed;
        std::atomic<eSystemState> _myState = eSystemState::idle;
        atomic_bool doReset = false;

        void setupGUI();

    protected:

        sCoRoutineHandler<bool> Reset();

        void notify(eEvent aEvent, void* data);

    public:
        cDE10Lite(VerilatedContext* context, bool traceActive, cGuiInterface* aGUI);
        ~cDE10Lite();

        inline void bitSet8(uint8_t& signal, uint8_t bit){ signal |=  (1 << bit); }

        inline void bitClr8(uint8_t& signal, uint8_t bit){ signal &= ~(1 << bit); }

        eRunState run();
        eRunState run(uint32_t numMilliSeconds);
};
