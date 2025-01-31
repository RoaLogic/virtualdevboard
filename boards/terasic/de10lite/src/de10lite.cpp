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
cDE10Lite::cDE10Lite(VerilatedContext* context, bool traceActive, cGuiInterface* aGUI) :
  cTestBench<designName>(context, traceActive),
  key(_core->KEY),
  _myGUI(aGUI)
{
    /*
      define clocks
     */
    clk_50  = addClock(_core->CLK_50, 20.0_ns);
    clk2_50 = addClock(_core->CLK2_50, 20.0_ns);
    clk_adc_10 = addClock(_core->CLOCK_ADC_10, 100.0_ns);
    clk_vga = addClock(_core->de10lite_verilator_wrapper->vgaMonitor_inst->pixel_clk, 100.0_ns, false);

    /*
      KEY
     */
    key = 0x3;        //KEY has pull-up

    // As last setup the GUI
    if(aGUI)
    {
        setupGUI();
    }
}

/**
 * @brief Destructor
 */
cDE10Lite::~cDE10Lite()
{
    if(_myGUI)
    {
        _myGUI->removeObserver(this);
    }
}

void cDE10Lite::setupGUI()
{
    const cDistance boardWidth  = 97.54_mm;
    const cDistance boardHeight = 80.01_mm;

    if(_myGUI)
    {
        // First register ourself to the GUI so that we receive all the events from the GUI
        _myGUI->registerObserver(this);

        // // Setup the GUI itself
        // _myGUI->setupGui("DE10lite virtual demo board",     // Name of the application
        //             "DE10lite",                             // About title information
        //             "This is a virtual development board for the DE10lite", // About text
        //             distanceSize(boardWidth, boardHeight),  // Size of the board
        //             sRGBColor(0, 75, 128) );                // Background colour of the board

        // LEDs
        for(size_t i = 0; i < _cNumLed; i++)
        {
            // Create a new led instance and map it through the scope with the verilated component
            _ledInstances[i] = new cVdbLed("TOP.de10lite_verilator_wrapper.gen_vdbLED[" + 
                                                std::to_string(i) + 
                                                "].LED_inst", i);

            // Map the LED instance to a LED on the virtual board
            _myGUI->addVdbComponent(eVdbComponentType::vdbLed,                      // VDB component type LED
                                    _ledInstances[i],                               // Verilated linked component
                                    distancePoint((48.0_mm + 5.0_mm*i), 66.0_mm),   // Placement on the board
                                    // LED specific information, in this case a SMD3.5x2.0mm LED, with a full red colour
                                    new sVdbLedInformation(eVdbLedType::SMD3520, {255, 0, 0}));
        }

        // 7-Segment Displays
        for(size_t i = 0; i < _cNum7Seg; i++)
        {
            // Create a new 7 segment instance and map it through the scope with the verilated component
            _7segInstances[i] = new cVdb7SegmentDisplay("TOP.de10lite_verilator_wrapper.gen_vdb7SegmentDisplay[" + 
                                                std::to_string(i) + 
                                                "].hex_inst", i);

            // Map the 7 segment instance to a 7 segment on the virtual board. The type of 7 segment is a commonAnode, with a RGB colour of pure RED.
            // The placement on the board is determined through the distancePoint.
            _myGUI->addVdbComponent(eVdbComponentType::vdb7SegmentDisplay,          // VDB component type 7 segment
                                    _7segInstances[i],                              // Verilated linked component
                                    distancePoint((1_mm + 300_mils*i), boardHeight - (1_mm + 500_mils)),  // Placement on the board
                                    // 7 segment specific information, in this case a common anode 7 segment, with a full red colour
                                    new sVdb7SegInformation(eVdb7SegType::commonAnode, {255, 0, 0}));
        }

        // SDRAM
        _myGUI->addVdbComponent(eVdbComponentType::vdbIC, // VDB component type IC
                                nullptr,                  // No verilated content
                                distancePoint(9_mm,37_mm),
                                new sVdbICInformation(875_mils,400_mils,"SDRAM"));

        // MAX-II (GUI only element)
        _myGUI->addVdbComponent(eVdbComponentType::vdbIC, // VDB component type IC
                                nullptr,                  // No verilated content
                                distancePoint(19.5_mm, 18.5_mm),
                                new sVdbICInformation(14_mm,14_mm,"MAX-II"));

        // MAX10 (FPGA)
        _myGUI->addVdbComponent(eVdbComponentType::vdbIC, // VDB component type IC
                                nullptr,                  // No verilated content
                                distancePoint(39_mm,26.5_mm),
                                new sVdbICInformation(23_mm, 23_mm,"FPGA"));


        // Create the VGA connector
        _myGUI->addVdbComponent(eVdbComponentType::vdbConnector, // VDB connector component for VGA
                                nullptr,                         // No verilated content
                                distancePoint(81_mm,7.5_mm),
                                new sVdbConnectorInformation(eVdbConnectorType::DSUB,30.8_mm,16.2_mm,"VGA"));

        // Create a new VGA instance and map it through the scope with the verilated component
        _vgaController = new cVdbVGAMonitor("TOP.de10lite_verilator_wrapper.vgaMonitor_inst", this, clk_vga,
                                            _core->de10lite_verilator_wrapper->vgaMonitor_inst->framebuffer);

        // Create a new VGA component on the virtual board
        _myGUI->addVdbComponent(eVdbComponentType::vdbVGA,          // VDB component type VGA
                                _vgaController,                     // Verilated linked component
                                distancePoint(50.0_mm, 100.0_mm),   // Placement on the board
                                nullptr);                           // Doesn't take any information
    }
}

/**
 * @brief Generate reset
 * @details This is a coroutine function that generates the main reset
 * 
 * It is generates the first reset after 5 clock cycles. Following that
 * it is suspended until a resume() is called on it. This is done so that
 * in a running design another reset could be triggered in the system. 
 * 
 * No processing power is used when the reset function is in a suspended state.
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

    do
    {
        INFO << "Assert reset\n";
        bitClr8(key,0);
        waitPosEdge(clk_50);

        INFO << "Negate reset\n";
        bitSet8(key,0);
        co_await std::suspend_always{};
    } while (!finished());

    co_return true;    
}


/**
 * @brief Run testbench
 *
 */
eRunState cDE10Lite::run()
{
    //Reset core
    sCoRoutineHandler reset = Reset();

    //Run testbench
    while(!finished())
    {
        if(_myState == eSystemState::running)
        {
            tick();
        }

        if(doReset)
        {
            reset.resume();
            doReset = false;
        }
    }

    reset.resume(); // End the reset coroutine at this point
    INFO << "Simulation ended\n";

    return _returnState;
}

eRunState cDE10Lite::run(uint32_t numMilliSeconds)
{
    //Reset core
    sCoRoutineHandler reset = Reset();

    //Run testbench
    while(!finished())
    {
        tick();

        if(numMilliSeconds != 0)
        {
            if(getTime().ms() > numMilliSeconds)
            {
                finish();
                INFO << "Time passed, end simulation\n";
            }
        }
    }

    reset.resume(); // End the reset coroutine at this point
    INFO << "Simulation ended\n";

    return _returnState;

}

void cDE10Lite::notify(eEvent aEvent, void* data)
{
    switch(aEvent)
    {
        case eEvent::close:
            finish();
        break;

        case eEvent::stop:
            _returnState = eRunState::restart;
            finish();
        break;

        case eEvent::reset:
            doReset = true;
            break;

        case eEvent::stateChange:
            switch (_myState)
            {
            case eSystemState::paused :
                [[fallthrough]];
            case eSystemState::idle :
                _myState = eSystemState::running;
                break;
            case eSystemState::running:
                [[fallthrough]];
            default:
                _myState = eSystemState::paused;
                break;
            }
        break;

        default:
            break;
    }
}
