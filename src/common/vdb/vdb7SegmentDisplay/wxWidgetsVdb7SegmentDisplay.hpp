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
        sVdb7SegInformation* _myInformation;
        uint8_t _value;

        static inline wxColour  colBackground = wxColour(138,150,168);            //Grey-blue
        static inline wxColour  colLedOn      = wxColour(255,0,0);                //Red
        static inline wxColour  colLedOff     = wxColour(255,223,223);            //light Red
        static inline cDistance deviceWidth   = 300_mils;                         //300mil wide
        static inline cDistance deviceHeight  = 500_mils;                         //500mil tall
        static inline cDistance ledWidth      =  30_mils;                         //30mil LED width
        static inline cDistance ledLength     = 165_mils;                         //165mil LED lenght
        static inline cDistance ledOffset     = (ledLength - 2*ledWidth) * 0.176; //led-length * tan(10)

        /**
         * @brief Returns true if bit @bit is set
         * @details This helper function returns true if bit @bit is set in @_value, false otherwise
         * @returns true if the bit is set, false otherwise
         */
        bool bitSet      (int   bit  )  const { return (_value >> bit) & 1; }

        /**
         * @brief Helper functions to scale the widget
         * @details Helper functions to scale the widget depending on the screen's DPI
         */
        int  scaleWidth  (int   width)  const { return width  * GetDPI().GetWidth()  /1000; }
        int  scaleWidth  (float width)  const { return width  * GetDPI().GetWidth()  /1000.0; }
        int  scaleHeight (int   height) const { return height * GetDPI().GetHeight() /1000; }
        int  scaleHeight (float height) const { return height * GetDPI().GetHeight() /1000.0; }

        /**
         * @brief Default widget size
         * @details Returns the default size for the widget.

            The optional @a win argument is new since wxWidgets 3.1.3 and allows to
            get a per-monitor DPI specific size.
        */
        wxSize GetDefaultSize(wxWindow* win = NULL)
        {
            return wxSize(deviceWidth.pix(GetDPI().GetWidth()),deviceHeight.pix(GetDPI().GetHeight()));
        }
        wxSize GetBestClientSize() { return GetDefaultSize(); }

        /**
         * @brief notify function from the vdb component
         * @details This function receives events from the component it is registered to.
         * @note this function runs in the verilated context.
         */
        void notify(eEvent aEvent, void* data);

	/**
         * @brief Handle the event
         * @details This function handles the wxEVT_7SegmentDisplay event
         * @note This function runs in the GUI thread
         */
        void onEvent(wxCommandEvent& event);

        public:
	/**
	 * @brief Constructor
	 */
        cWXVdb7SegmentDisplay(cVDBCommon* myVDBComponent, distancePoint position, wxWindow* windowParent, sVdb7SegInformation* information);

	/**
	 * @brief Destructor
	 */
        ~cWXVdb7SegmentDisplay();

        /**
	 * @brief Paint the widget
	 * @details This function paints the widget. The 'LEDs' are turned on when
	 *          the corresponding bits in @_value are set, otherwise the corresponding LEDs are turned off
	 */
        void OnPaint(wxPaintEvent& event);
    };

}}

#endif
