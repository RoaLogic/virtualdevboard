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

#include <vector>
//include Dpi headers, required to link verilator model to C++
#include "vdb__Dpi.h"

// #include <wx/wx.h>
// #include "wx/event.h"
// #include "wx/bitmap.h"

#include "log.hpp"
#include "testbench.hpp"

#ifndef VDB_VGA_HPP
#define VDB_VGA_HPP

//wxDECLARE_EVENT(wxEVT_VGA, wxCommandEvent);

using namespace RoaLogic::testbench;

enum class eVgaEvent
{
    vsync,
    hsync
};

class cVdbVGA
{
    private:
    struct sVdbVGAMap
    {
        svScope scope;
        cVdbVGA* reference;
    };

    union uRGBValue
    {
        uint32_t asInt;
        struct
        {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t unused;
        };
    };

    static std::vector<sVdbVGAMap> _referencePointers;
    static void registerVirtualVGA(sVdbVGAMap reference);

    public:
    // Function to call for going from static scope to class scope
    static void processVGAEvent(svScope scope, eVgaEvent event);

    private:
    cTimeInterface* _timeInterface;
    cClock* _pixelClock;
    svScope _myScope;
    size_t _numHsync = 0;
    simtime_t _previousVSyncTime;
    simtime_t _timeBetweenVsync;
    bool _settingFound = false;
    uint8_t _currentSetting = 0xff;
    //wxFrame* _myFrame;
    //wxBitmap _myBitmap;
    // wxStaticBitmap* _myStaticBitmap;
    // wxImage newImage;
    // uint32_t currentRow = 0;

    public:
    //cVdbVGA(std::string scopeName, wxEvtHandler* aParent, int id);
    cVdbVGA(std::string scopeName, cTimeInterface* timeInterface, cClock* pixelClock);
    ~cVdbVGA();

    void show(bool show);
    void handleVsync();
    void handleHsync();
};

#endif