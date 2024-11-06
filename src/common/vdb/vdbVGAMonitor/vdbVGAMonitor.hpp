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
     * There are two events sent by this class, the vgaData and the 
     * vgaDataReady event. vgaData is sent at the moment a HSYNC is received,
     * where the vgaDataReady is sent when a VSYNC is received. The vgaDataReady
     * shall be used to redraw the image, where the vgaData shall be used to store
     * the data in the image.
     * 
     * The data in the event is the sVgaData structure. It holds the number of 
     * horizontal and vertical lines, as well as a pointer to the data. Do note that
     * the data is deleted at the end of the function, so it should be copied and
     * not re-used.
     * 
     * 
     * @todo: Handle the front/back porch and sync period within this class when sending data
     * @todo: Should the current line be added into the data structure?
     * 
     */
    class cVdbVGAMonitor : public cVDBCommon
    {
        private:
        static const size_t cMaxHorizontalLines = 1024;
        static const size_t cMaxVerticalLines = 768;

        struct sVdbVGAMap
        {
            svScope scope;
            cVdbVGAMonitor* reference;
        };

        static std::vector<sVdbVGAMap> _referencePointers;
        static void registerVirtualVGA(sVdbVGAMap reference);

        public:    
        enum class eVgaEvent
        {
            vsync,
            hsync
        };

        // Function to call for going from static scope to class scope
        static void processVGAEvent(svScope scope, eVgaEvent event);

        private:
        static const size_t cMaxNumVertical = 768;
        static const size_t cMaxHorizontal = 1024;
        cTimeInterface* _timeInterface;   //!< Pointer to the time interface for retrieving the current time
        cClock* _pixelClock;              //!< Pointer to the pixel clock, which must be generated within this class
        svScope _myScope;                 //!< The scope of the verilated context
        simtime_t _previousVSyncTime;     //!< Previous time that a VSYNC occured
        uint8_t _currentSetting = 0xff;   //!< Current lookup table setting, 0xff means no element found
        sVgaData _myEventData;            //!< Event data element which is passed in any of the events
        size_t _numHsync = 0;             //!< Counter for the number of HSYNC in a single VSYNC period

        #ifdef VlUnpackedSingle_Array
        VlUnpacked<unsigned int, cMaxNumVertical*cMaxHorizontal>& _myFramebuffer;
        #endif
        #ifdef VlUnpacked2D_Array
        VlUnpacked<VlUnpacked<unsigned int, cMaxHorizontal>, cMaxNumVertical>& _myFramebuffer;
        #endif
        #ifdef VlWide_Array
        VlWide<589824>& _myFramebuffer;
        #endif

        public:
        cVdbVGAMonitor(std::string scopeName, cTimeInterface* timeInterface, cClock* pixelClock,
                #ifdef VlUnpackedSingle_Array
                VlUnpacked<unsigned int, cMaxNumVertical*cMaxHorizontal>& framebuffer);
                #endif
                #ifdef VlUnpacked2D_Array
                VlUnpacked<VlUnpacked<unsigned int, cMaxHorizontal>, cMaxNumVertical>& framebuffer);
                #endif
                #ifdef VlWide_Array
                VlWide<589824>& framebuffer);
                #endif

        ~cVdbVGAMonitor();

        void handleVsync();
        void handleHsync();
    };

}
}

#endif