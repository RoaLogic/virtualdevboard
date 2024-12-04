/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets virtual Devboard LED C++ source file              //
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

#include "wxWidgetsVdbLED.hpp"
#include "wxGuiDistance.hpp"

// Define the wxEVT_LED, which is special within this class
wxDEFINE_EVENT(wxEVT_LED, wxCommandEvent);

namespace RoaLogic {
    using namespace observer;
namespace GUI {

    /**
     * @brief Construct a new wx widgets LED window
     * @details 
     * This is the constructor for the LED window. Bind our 
     * onLedEvent function with the ID to a wxEVT_LED event.
     * 
     */
    cWXVdbLed::cWXVdbLed(cVDBCommon* myVDBComponent, distancePoint position, wxWindow* windowParent, int Size, char color) :
        cGuiVDBComponent(myVDBComponent, position),
        wxWindow(windowParent, wxID_ANY, wxGuiDistance::convertPoint(position, windowParent), wxSize(Size,Size), wxTRANSPARENT_WINDOW, color),
        _color(color)
    {

        Connect(wxEVT_PAINT, wxPaintEventHandler(cWXVdbLed::OnPaint));

        // Use a specific Bind due to bug inside of wxWidgets, see 
        // https://stackoverflow.com/questions/38833116/conversion-in-derived-class-inaccessible-if-base-class-is-protected
        windowParent->Bind(wxEVT_LED, std::bind(&cWXVdbLed::onEvent, this, std::placeholders::_1), myVDBComponent->getID());
        //myEvtHandler->Bind(wxEVT_LED, &cWXVdbLed::onLEDEvent, this, wxID_ANY);
    }

    /**
     * @brief notify function from the vdb component
     * @details This function receives events from the component
     * it is registered to. This shall be the cVdbLed and the 
     * received events are ledChangedOn and ledChangedOff. 
     * 
     * The corresponding event is translated to an integer and 
     * placed in the wxCommandEvent. This is then posted to 
     * the GUI thread, which runs the onLEDEvent function.
     * 
     * @note this function runs in the verilated context.
     */
    void cWXVdbLed::notify(eEvent aEvent, void* data)
    {
        wxCommandEvent ledEvent{wxEVT_LED, getIntID()};
        ledEvent.SetInt(static_cast<int>(aEvent));
        wxPostEvent(this, ledEvent);
    }

    /**
     * @brief Handle the LED event
     * @details This function handles the wxEVT_LED event
     * 
     * The event is sent when the LED has changes state.
     * The passed int is of the eEvent and can therefore
     * be directly casted. According to this state we update
     * our flag status and redraw the component.
     * 
     * @note This function runs in the GUI thread
     */
    void cWXVdbLed::onEvent(wxCommandEvent& event)
    {
        _status = static_cast<eEvent>(event.GetInt()) == eEvent::ledChangedOn ? true : false;
        Refresh();
    }

    
    /**
     * @brief 
     * 
     * @param event 
     */
    void cWXVdbLed::OnPaint(wxPaintEvent& event)
    {
        const int x = scaleWidth(mm2mil(deviceWidth));
        const int y = scaleHeight(mm2mil(deviceHeight));

        wxPaintDC dc(this);
        wxColour ledColour = _status ? wxColour(255,0,0) : wxColour(255,223,223);

        dc.SetPen(wxPen(wxColour(0,0,0),1));
        dc.SetBrush(ledColour);
        dc.DrawRectangle(0,0,x,y);
    }

    void cWXVdbLed::SetColor(char color)
    {
        _color = color;
        Refresh();
    }

}}
