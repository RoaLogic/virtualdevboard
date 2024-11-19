/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//   wxWidgets virtual Devboard 7-Segment Display LED C++ src file //
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

#include "wxWidgetsVdb7SegmentDisplay.hpp"

// Define the wxEVT_7SegmentDisplay, which is special within this class
wxDEFINE_EVENT(wxEVT_7SegmentDisplay, wxCommandEvent);

namespace RoaLogic {
    using namespace observer;
namespace GUI {

    /**
     * @brief Construct a new wxWidgets 7-Segment Display window
     * @details 
     * This is the constructor for the 7-Segment Display window
     * Bind the onEvent function with the ID to a wxEVT_7SegmentDisplay event.
     * 
     */
    cWXVdb7SegmentDisplay::cWXVdb7SegmentDisplay(cVDBCommon* myVDBComponent, int id, wxEvtHandler* myEvtHandler, wxWindow* windowParent, wxPoint Position, int Size, char Color) :
        cGuiVDBComponent(myVDBComponent, id),
        wxWindow(windowParent, wxID_ANY, Position, wxSize(Size,Size), wxTRANSPARENT_WINDOW, Color),
        _color(color),
        _evtHandler(myEvtHandler)
    {

        Connect(wxEVT_PAINT, wxPaintEventHandler(cWXVdb7SegmentDisplay::OnPaint));

        // Use a specific Bind due to bug inside of wxWidgets, see 
        // https://stackoverflow.com/questions/38833116/conversion-in-derived-class-inaccessible-if-base-class-is-protected
        myEvtHandler->Bind(wxEVT_7SegmentDisplay, std::bind(&cWXVdb7SegmentDisplay::onEvent, this, std::placeholders::_1), id);
    }

    /**
     * @brief destructor
     */
    cWXVdb7SegmentDisplay::~cWXVdb7SegmentDisplay()
    {
    }

    /**
     * @brief notify function from the vdb component
     * @details This function receives events from the component
     * it is registered to. This shall be the cVdb7SegmentDisplay and the 
     * received event is 7SegmentDisplayUpdate.
     * 
     * The corresponding event is translated to an integer and 
     * placed in the wxCommandEvent. This is then posted to 
     * the GUI thread, which runs the onEvent function.
     * 
     * @note this function runs in the verilated context.
     */
    void cWXVdb7SegmentDisplay::notify(eEvent aEvent, void* data)
    {
        _value = *data;
        wxCommandEvent 7SegmentDisplayEvent{wxEVT_7SegmentDisplay, _myID};
        wxPostEvent(_evtHandler, 7SegmentDisplayEvent);
    }

    /**
     * @brief Handle the event
     * @details This function handles the wxEVT_7SegmentDisplay event
     * 
     * The event is sent when the 7-Segment Display has changed state.
     * The data is already in _value, so we simply refresh the widget
     * 
     * @note This function runs in the GUI thread
     */
    void cWXVdb7SegmentDisplay::onEvent(wxCommandEvent& event)
    {        
        Refresh();
    }

    
    /**
     * @brief 
     * 
     * @param event 
     */
    void cWXVdb7SegmentDisplay::OnPaint(wxPaintEvent& event)
    {
        const wxSize ppi = GetPPI();
        wxPen penLed;
        wxPaintDC dc(this);
        int x, y;

        //draw outline
        dc.SetBrush(wxColour(138,150,168)); //grey-blue
        dc.SetPen(wxPen(wxColour(230,250,230), 1));
        dc.DrawRectangle(0,0, deviceWidth * ppi.width /1000, deviceHeight * ppi.height /1000);

        //draw the LEDs
        //   ---           bit[0]
        //  |   |    bit[5]       bit[1]
        //   ---           bit[6]
        //  |   |    bit[4]       bit[2]
        //   ---  *        bit[3]       bit[7]
        //
        for (int bit=0; bit < 8; bit++)
        {
            switch (bit)
            {
                0: if (bitSet(bit)) {
                       penLed = wxPen(colLedOn, ledWidth * ppi.height /1000);
                   } else {
                       penLed = wxPen(colLedOff, ledWidth * ppi.height /1000);
                   }
                   dc.DrawLine((deviceWidth -ledLength/2 +15) * ppi.width /1000, 100 * ppi.height /1000,
                               (deviceWidth +ledLength/2 +15) * ppi.width /1000, 100 * ppi.height /1000);
                   break;

                1: if (bitSet(bit)) {
                       penLed = wxPen(colLedOn, ledWidth * ppi.width /1000);
                   } else {
                       penLed = wxPen(colLedOff, ledWidth * ppi.width /1000);
                   }
                   dc.DrawLine((deviceWidth +ledLength/2    ) * ppi.width /1000, 250 * ppi.height /1000,
                               (deviceWidth +ledLength/2 +15) * ppi.width /1000, 100 * ppi.height /1000);
                   break;

                2: if (bitSet(bit)) {
                       penLed = wxPen(colLedOn, ledWidth * ppi.width /1000);
                   } else {
                       penLed = wxPen(colLedOff, ledWidth * ppi.width /1000);
                   }
                   dc.DrawLine((deviceWidth +ledLength/2    ) * ppi.width /1000, 250 * ppi.height /1000,
                               (deviceWidth +ledLength/2 -15) * ppi.width /1000, 400 * ppi.height /1000);
                   break;

                3: if (bitSet(bit)) {
                       penLed = wxPen(colLedOn, ledWidth * ppi.height /1000);
                   } else {
                       penLed = wxPen(colLedOff, ledWidth * ppi.height /1000);
                   }
                   dc.DrawLine((deviceWidth -ledLength/2 -15) * ppi.width /1000, 400 * ppi.height /1000,
                               (deviceWidth +ledLength/2 -15) * ppi.width /1000, 400 * ppi.height /1000);
                   break;

                4: if (bitSet(bit)) {
                       penLed = wxPen(colLedOn, ledWidth * ppi.width /1000);
                   } else {
                       penLed = wxPen(colLedOff, ledWidth * ppi.width /1000);
                   }
                   dc.DrawLine((deviceWidth -ledLength/2    ) * ppi.width /1000, 250 * ppi.height /1000,
                               (deviceWidth -ledLength/2 -15) * ppi.width /1000, 400 * ppi.height /1000);
                   break;

                5: if (bitSet(bit)) {
                       penLed = wxPen(colLedOn, ledWidth * ppi.width /1000);
                   } else {
                       penLed = wxPen(colLedOff, ledWidth * ppi.width /1000);
                   }
                   dc.DrawLine((deviceWidth -ledLength/2    ) * ppi.width /1000, 250 * ppi.height /1000,
                               (deviceWidth -ledLength/2 +15) * ppi.width /1000, 100 * ppi.height /1000);
                   break;

                6: if (bitSet(bit)) {
                       penLed = wxPen(colLedOn, ledWidth * ppi.height /1000);
                   } else {
                       penLed = wxPen(colLedOff, ledWidth * ppi.height /1000);
                   }
                   dc.DrawLine((deviceWidth -ledLength/2) * ppi.width /1000, 250 * ppi.height /1000,
                               (deviceWidth +ledLength/2) * ppi.width /1000, 250 * ppi.height /1000;
                   break;

                7: if (bitSet(bit)) {
                       penLed = wxPen(colLedOn, ledWidth * ppi.height /1000);
                   } else {
                       penLed = wxPen(colLedOff, ledWidth * ppi.height /1000);
                   }
                   dc.DrawLine((deviceWidth +ledLength/2 +15 +ledWidth) * ppi.width /1000, 400 * ppi.height /1000,
                               (deviceWidth +ledLength/2 +15 -ledWidth) * ppi.width /1000, 400 * ppi.height /1000);
                   break;
            }
        }
    }
}}
