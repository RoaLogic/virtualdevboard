/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Virtual Devboard VGA Monitor Verilator C++ header file       //
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
 * @section vdbComponentVGA Virtual development VGA component
 *
 * VGA monitor text
 */


#include "vdbCommon.hpp"
#include <vector>

#ifndef VDB_VGA_HPP
#define VDB_VGA_HPP

using namespace RoaLogic::testbench;
using namespace RoaLogic::observer;

namespace RoaLogic
{
namespace vdb
{
    union uRGBValue
    {
        uint32_t asInt;
        struct
        {
            uint8_t blue;
            uint8_t green;
            uint8_t red;
            uint8_t alpha;
        };
    };

    struct sVgaData
    {
        uint32_t horizontalLines;
        uint32_t verticalLines;
        uRGBValue* dataArray;    
    };

    /**
     * @class cVdbVGAMonitor
     * @author Bjorn Schouteten
     * @brief VGA virtual development board component
     * 
     * @details
     * This class communicates with the verilator component, any
     * class that wants to listen to it should register itself through
     * the subject-observer pattern.
     * 
     * This class sents one event, the vgaDataReady event. It passes 
     * data according to the sVgaData structure. Which passes the number
     * of horizontal and vertical lines. The data array is build up as a single
     * array with a size of horizontal lines * vertical lines. All the horizontal
     * lines are appended after each other.
     * 
     */
    class cVdbVGAMonitor : public cVDBCommon
    {
        public:
        static const size_t cMaxHorizontalLines = 1024;
        static const size_t cMaxVerticalLines = 768;

        private:
        cTimeInterface* _timeInterface;   //!< Pointer to the time interface for retrieving the current time
        cClock* _pixelClock;              //!< Pointer to the pixel clock, which must be generated within this class
        simtime_t _previousVSyncTime;     //!< Previous time that a VSYNC occured
        uint8_t _currentSetting = 0xff;   //!< Current lookup table setting, 0xff means no element found
        sVgaData _myEventData;            //!< Event data element which is passed in any of the events

        //!< The VGA data array buffer allocated in the verilog code, used as reference to show the data on screen
        VlUnpacked<unsigned int, cMaxVerticalLines*cMaxHorizontalLines>& _myFramebuffer;

        // Function to call for going from static scope to class scope
        void verilatorCallback(uint32_t event);

        public:
        cVdbVGAMonitor(std::string scopeName, cTimeInterface* timeInterface, cClock* pixelClock,
                VlUnpacked<unsigned int, cMaxVerticalLines*cMaxHorizontalLines>& framebuffer);

        ~cVdbVGAMonitor();
    };

}
}

#endif