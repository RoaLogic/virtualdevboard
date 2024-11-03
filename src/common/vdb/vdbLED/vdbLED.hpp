/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Virtual Devboard LED Verilator C++ header file               //
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

#ifndef VDB_LED_HPP
#define VDB_LED_HPP

#include <vector>

//include Dpi headers, required to link verilator model to C++
#include "vdb__Dpi.h"

#include <wx/wx.h>
#include "wx/event.h"

#include "log.hpp"

wxDECLARE_EVENT(wxEVT_LED, wxCommandEvent);

/**
 * @class cVirtualLed
 * @author Bjorn Schouteten
 * @brief Virtual GUI LED controlled by verilog instance
 * @version 0.1
 * @date 13-oct-2024
 *
 * @details This class creates a virtual LED on a GUI, where it's state is controlled by
 * the verilated design. 
 * 
 * The verilated design must use the DPI functions defined in vdbLED.sv. Those events are
 * captured in static C++ functions, void vdbLedOff(int id) and void vdbLedOn(int id). From
 * those functions we can at that point post an event to the GUI thread, where we can say what 
 * happend (LED on or LED off). From the verilog design it also passes the ID of the LED,
 * which must translate to a LED in the GUI. If this is not the case a notification will
 * pop up.
 * 
 * First part of the class definition is static, this is used from the verilated context.
 * It will construct an event to the GUI class, which is handled in the OnLedEvent(). This
 * will send the received event to the corresponding virtual led and handle it correspondingly.
 * This is constructed in the last part of the class.
 * 
 * The OnLedFunction will traverse all the reference pointers and check to see if it finds the corresponding ID. 
 * When this ID is found it will call the SetStatus() function to change the LED state.
 * 
 * @attention Make sure that the verilated instance is running in a seperate thread,
 *            class internally this is properly handled.
 */
class cVirtualLed : public wxWindow
{
    private:
    struct sVirtualLedMap
    {
        int ID;
        cVirtualLed* ledReference;
    };

    struct sEventData : public wxClientData
    {
        int id;
        bool on;
    };

    static wxEvtHandler* _parent;
    static std::vector<sVirtualLedMap> _referencePointers;
    static void registerVirtualLed(sVirtualLedMap map, wxEvtHandler* aParent);

    public:
    
    static void SendLedEvent(int ledID, bool state); // Function to push the event from verilator thread
    static void OnLedEvent(wxCommandEvent& event); // Function to retrieve event and handle it to the GUI

    private:
    wxWindow* _windowParent;
    char _color;
    int x0,y0,x1,y1,D1,D2;
    bool FlagStatus = false;

    public:
    cVirtualLed(wxEvtHandler* aParent, int id, wxWindow *parent, wxPoint Position, int Size, char color);

    void OnPaint(wxPaintEvent& event);
    void SetColor(char color);
    void SetStatus(bool status);
};

#endif
