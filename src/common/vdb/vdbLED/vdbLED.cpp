/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Virtual Devboard LED Verilator C++ wrapper                   //
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

#include "vdbLED.hpp"

using namespace RoaLogic::vdb;

//#define DBG_VDB_LED

/**
 * @brief LED-On DPI-C callback
 * @details This function gets called when the LED 
 * state in the design changes state to on.
 * 
 * It will pass it's event to the handleLedEvent so that it 
 * can be processed correctly within the virtual led class.
 * 
 * @attention This function runs in the verilator thread context
 */
void vdbLedOn(int id)
{
    #ifdef DBG_VDB_LED
    INFO << "LED: On event: " << svGetScope() << "\n";
    #endif
    // Get the scope of the current call and pass this into the processing function
    cVDBCommon::processVerilatorEvent(svGetScope(), static_cast<uint32_t>(cVdbLed::eVdbLedEvent::ledOn));
}

/**
 * @brief LED-Off DPI-C callback
 * @details This function gets called when the LED 
 * state in the design changes state to off.
 * 
 * It will pass it's event to the handleLedEvent so that it 
 * can be processed correctly within the virtual led class.
 * 
 * @attention This function runs in the verilator thread context
 */
void vdbLedOff(int id)
{
    #ifdef DBG_VDB_LED
    INFO << "LED: Off event: " << svGetScope() << "\n";
    #endif
    // Get the scope of the current call and pass this into the processing function
    cVDBCommon::processVerilatorEvent(svGetScope(), static_cast<uint32_t>(cVdbLed::eVdbLedEvent::ledOff));
}


namespace RoaLogic
{
    using namespace observer;
namespace vdb
{
    /**
     * @brief Construct a new cVdbLed object
     * @details This constructer creates a new cVdbLed object
     * 
     * First the scope is set according to the given name, where it is 
     * also checkd that the scope exists. When this is ok, it registers
     * this class for any verilator actions.
     * 
     * @param[in] scopeName     Scope of this LED
     * @param[in] id            Optional ID of this led (mainly used for debugging)
     */
    cVdbLed::cVdbLed(std::string scopeName, uint8_t id) :
        cVDBCommon(id)
    {
        // Get the scope according to the given name
        _myScope = svGetScopeFromName(scopeName.c_str());
        svSetScope(_myScope);

        #ifdef DBG_VDB_LED
        INFO << "LED: Create: ID " << id << " Scope: "<< svGetScope() << "\n";
        #endif

        // Make sure that the scope is not a nullpointer
        assert(_myScope != nullptr);

        // Store this class instance in the gloval class instances
        registerVdb(sVdbMap{_myScope, this});
    }

    /**
     * @brief destruct the cVdbLed object
     * 
     * Unregister this class so that it's not called anymore
     */
    cVdbLed::~cVdbLed()
    {
        unregisterVdb(sVdbMap{_myScope, this});
    }

    /**
     * @brief Callback function for a verilated LED event
     * @details This function handles the LED on and off events coming
     * from the verilated design. It checks if the event is alright and
     * then notifies all classes which are registered to it.
     * 
     * @param[in] event     The event which occured, shall be a eVdbLedEvent type
     */
    void cVdbLed::verilatorCallback(uint32_t event)
    {
        #ifdef DBG_VDB_LED
        INFO << "LED: Received led " << _myID << " event: " << event << "\n";
        #endif

        switch (static_cast<eVdbLedEvent>(event))
        {
        case eVdbLedEvent::ledOn:
            notifyObserver(eEvent::ledChangedOn, nullptr);
            break;
        case eVdbLedEvent::ledOff:
            notifyObserver(eEvent::ledChangedOff, nullptr);
            break;
        
        default:
            break;
        }
    }

}
}