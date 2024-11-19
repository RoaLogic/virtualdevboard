/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Virtual Devboard 7-Segment Display Verilator C++ wrapper     //
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

#include "vdb7SegmentDisplay.hpp"

using namespace RoaLogic::vdb;

/**
 * @brief 7-Segment Display Update DPI-C callback
 * @details This function gets called when the 7-Segment Display
 * state in the design changes.
 * 
 * It will pass it's event to the handleLedEvent so that it 
 * can be processed correctly within the virtual 7-Segment Display class.
 * 
 * @attention This function runs in the verilator thread context
 */
void vdb7SegmentDisplayUpdate(int id, const svBitVecVal* val)
{
    #ifdef DBG_VDB_7SEGMENT
    INFO << "7-Segment display: Update event: " << svGetScope() << "\n";
    #endif
    // Get the scope of the current call and pass this into the processing function
    // Pass the value of the 7-Segment Display alongside it
    cVDBCommon::processVerilatorEvent(svGetScope(), *val);
}


namespace RoaLogic
{
    using namespace observer;
namespace vdb
{
    /**
     * @brief Construct a new cVdb7SegmentDisplay object
     * @details This constructer creates a new cVdb7SegmentDisplay object
     *
     * First the scope is set according to the given name, where it is
     * also checked that the scope exists. When this is ok, it registers
     * this class for any verilator actions.
     *
     * @param[in] scopeName     Scope of this 7-Segment Display
     * @param[in] id            Optional ID of this led (mainly used for debugging)
     */
    cVdb7SegmentDisplay::cVdb7SegmentDisplay(std::string scopeName, uint8_t id) :
        _myID(id)
    {
        // Get the scope according to the given name
        _myScope = svGetScopeFromName(scopeName.c_str());
        svSetScope(_myScope);

        #ifdef DBG_VDB_7SEGMENT
        INFO << "7-Segment: Create: ID " << id << " Scope: "<< svGetScope() << "\n";
        #endif

        // Make sure that the scope is not a nullpointer
        assert(_myScope != nullptr);

        // Store this class instance in the gloval class instances
        registerVdb(sVdbMap{_myScope, this});
    }

    /**
     * @brief destruct the cVdb7SegmentDisplay object
     *
     * Unregister this class so that it's not called anymore
     */
    cVdb7SegmentDisplay::~cVdb7SegmentDisplay()
    {
        unregisterVdb(sVdbMap{_myScope, this});
    }

    /**
     * @brief Callback function for a verilated 7-Segment Display event
     * @details This function handles the 7-Segment Display update events coming
     * from the verilated design. It checks if the event is alright and
     * then notifies all classes which are registered to it.
     *
     * @param[in] event The new value of the 7-Segment Display
     */
    void cVdb7SegmentDisplay::verilatorCallback(uint32_t event)
    {
        #ifdef DBG_VDB_7SEGMENT
        INFO << "7Segment: Received ID " << _myID << " event: " << event << "\n";
        #endif

        notifyObserver(eEvent::sevenSegmentUpdate, &event);
    }

}
}
