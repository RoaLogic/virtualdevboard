/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    wxWidgets virtual Devboard 7-Segment Display C++ header file //
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

#ifndef WX_WIDGETS_VDB_7SEGMENT_HPP
#define WX_WIDGETS_VDB_7SEGMENT_HPP

#include <wx/wxprec.h>
#include <wx/wx.h>
#include "wx/event.h"
#include <wx/graphics.h>

#include "gui_interface.hpp"
#include "vdb7SegmentDisplay.hpp"

wxDECLARE_EVENT(wxEVT_7Segment, wxCommandEvent);

namespace RoaLogic {
    using namespace observer;
    using namespace vdb;
namespace GUI {

    /**
     * @class cWXVdb7Segment
     * @author Richard Herveille
     * @brief 7-Segment Display virtual development board component
     * 
     * @details
     * This class subscribes to it's VDB component and awaits any events.
     * Corresponding events are received in the notify function, which runs in
     * the verilated context. GUI adjustments have to be done in the GUI thread,
     * so a wxEvent is sent and following the thread switch then processed in the
     * on7SegmentEvent function.
     * 
     * The 7-Segment Display is drawn in the OnPaint function, which also defines
     *  what the 7-Segment Display looks like
     */
    class cWXVdb7SegmentDisplay : public cGuiVDBComponent, public wxWindow
    {
        private:
        wxEvtHandler* _evtHandler;                  //!< The event handler of this frame
        char _color;
        uint8_t _value;

        static inline wxColour colBackground = wxColour(138,150,168); //Grey-blue
        static inline wxColour colLedOn      = wxColour(255,0,0);     //Red
        static inline wxColour colLedOff     = wxColour(255,223,223); //light Red
        static int deviceWidth   = 300;                               //300mil wide
        static int deviceHeight  = 500;                               //500mil tall
        static int ledWidth      =  15;                               //15mil LED width
        static int ledLength     = 165;                               //165mil LED lenght

        void notify(eEvent aEvent, void* data);
        void onEvent(wxCommandEvent& event);

        bool bitSet(int bit) const { return (_value >> bit) & 1; }

        public:
        cWXVdb7SegmentDisplay(cVDBCommon* myVDBComponent, int id, wxEvtHandler* myEvtHandler, wxWindow* windowParent, wxPoint Position, int Size, char Color);
        ~cWXVdb7SegmentDisplay();

        void OnPaint(wxPaintEvent& event);
    };

}}

#endif
