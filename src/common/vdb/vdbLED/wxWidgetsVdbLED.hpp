/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets virtual Devboard LED C++ header file              //
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

#ifndef WX_WIDGETS_VDB_LED_HPP
#define WX_WIDGETS_VDB_LED_HPP

#include <wx/wxprec.h>
#include <wx/wx.h>
#include "wx/event.h"
#include <wx/graphics.h>

#include "gui_interface.hpp"
#include "wxGuiDistance.hpp"
#include "vdbLED.hpp"

wxDECLARE_EVENT(wxEVT_LED, wxCommandEvent);

namespace RoaLogic {
    using namespace observer;
    using namespace vdb;
namespace GUI {

    /**
     * @class cWXVdbLed
     * @author Bjorn Schouteten, Richard Herveille
     * @brief LED virtual development board component
     * 
     * @details
     * This class subscribes to it's VDB component and awaits any events.
     * Corresponding events are received in the notify function, which runs in
     * the verilated context. GUI adjustments have to be done in the GUI thread,
     * so a wxEvent is sent and following the thread switch then processed in the
     * onLedEvent function.
     * 
     * The LED is drawn in the OnPaint function, which also defines how the LED looks like
     * 
     * @todo: Adjust the LED layout
     */
    class cWXVdbLed : public cGuiVDBComponent, public wxWindow
    {
        private:
        sVdbLedInformation* _myInformation;
        bool _status = false;

        /**
         * @brief notify function from the vdb component
         * @details This function receives events from the component it is registered to.
         * @note this function runs in the verilated context.
         */
        void notify(eEvent aEvent, void* data);

	/**
         * @brief Handle the event
         * @details This function handles the wxEVT_LED event
         * @note This function runs in the GUI thread
         */
        void onEvent(wxCommandEvent& event);

        public:
	/**
	 * @brief Constructor
	 */
        cWXVdbLed(cVDBCommon* myVDBComponent, distancePoint position, wxWindow* windowParent, sVdbLedInformation* ledInformation);

	/**
	 * @brief Destructor
	 */
        ~cWXVdbLed() {}

        /**
         * @brief LED size
         * @details Returns the size of the LED
         */
        distanceSize GetDeviceSize();

        /**
         * @brief Default component size
         * @details Returns the default size for the component.
	 *
            The optional @a win argument is new since wxWidgets 3.1.3 and allows to
            get a per-monitor DPI specific size.
         */
        wxSize GetDefaultSize(wxWindow* win = NULL)
        {
            if (win == NULL) { win = this; }
            return wxDistanceSize(GetDeviceSize(), win);
        }

        /**
	 * @brief Paint the widget
	 * @details This function paints the widget. The 'LED' is turned on when _status=TRUE,
	 *          otherwise the LED is turned off
	 */
        void OnPaint(wxPaintEvent& event);
    };

}}

#endif
