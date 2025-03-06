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
#include "wxGuiDistance.hpp"

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
    cWXVdb7SegmentDisplay::cWXVdb7SegmentDisplay(cVDBCommon* myVDBComponent, distancePoint position, wxWindow* windowParent, sVdb7SegInformation* information, double angle) :
        cWXVdbBase(myVDBComponent, position, windowParent, information, distanceSize(deviceWidth, deviceHeight), angle)
    {
        Connect(wxEVT_PAINT, wxPaintEventHandler(cWXVdb7SegmentDisplay::OnPaint));

        // Use a specific Bind due to bug inside of wxWidgets, see 
        // https://stackoverflow.com/questions/38833116/conversion-in-derived-class-inaccessible-if-base-class-is-protected
        windowParent->Bind(wxEVT_7SegmentDisplay, std::bind(&cWXVdb7SegmentDisplay::onEvent, this, std::placeholders::_1), myVDBComponent->getID());
    }

    /**
     * @brief notify function from the vdb component
     * @details This function receives events from the component
     * it is registered to. This shall be the cVdb7SegmentDisplay and the 
     * received event is sevenSegmentDisplayUpdate.
     * 
     * The corresponding event is translated to an integer and 
     * placed in the wxCommandEvent. This is then posted to 
     * the GUI thread, which runs the onEvent function.
     * 
     * @note this function runs in the verilated context.
     */
    void cWXVdb7SegmentDisplay::notify(eEvent aEvent, void* data)
    {
        _value = *reinterpret_cast<uint8_t*>(data);
        wxCommandEvent sevenSegmentDisplayEvent{wxEVT_7SegmentDisplay, getIntID()};
        wxPostEvent(this, sevenSegmentDisplayEvent);
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
     * @brief Paint the widget
     * @details This function paints the widget. The 'LEDs' are turned on when
     *          the corresponding bits in @_value are set, otherwise the corresponding LEDs are turned off
     */
    void cWXVdb7SegmentDisplay::OnPaint(wxPaintEvent& event)
    {
        const cDistance xMin      = (deviceWidth - ledLength)/2;
        const cDistance xMax      = (deviceWidth + ledLength)/2;
        const int       dpiWidth  = ToPhys(FromDIP(GetDPI().GetWidth ()));
        const int       dpiHeight = ToPhys(FromDIP(GetDPI().GetHeight()));

        wxPen penLed;

        //Create new Drawing Canvas
        NewDC();

        //draw outline
        SetBrush(wxColour(138,150,168)); //grey-blue
        SetPen(wxPen(wxColour(230,250,230), 1));
        DrawRectangle(0,0, deviceWidth, deviceHeight);

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
                case 0: penLed = bitSet(bit) ? wxPen(colLedOn , ledWidth.pix(dpiHeight))
                                             : wxPen(colLedOff, ledWidth.pix(dpiHeight));
                        SetPen(penLed);
                        DrawLine(xMin + ledOffset + ledWidth, 100_mils, xMax + ledOffset - ledWidth, 100_mils);
                        break;

                case 1: penLed = bitSet(bit) ? wxPen(colLedOn , ledWidth.pix(dpiWidth))
                                             : wxPen(colLedOff, ledWidth.pix(dpiWidth));
                        SetPen(penLed);
                        DrawLine(xMax + ledOffset, 100_mils + ledWidth, xMax, 250_mils - ledWidth);
                        break;

                case 2: penLed = bitSet(bit) ? wxPen(colLedOn , ledWidth.pix(dpiWidth))
                                             : wxPen(colLedOff, ledWidth.pix(dpiWidth));
                        SetPen(penLed);
                        DrawLine(xMax, 250_mils + ledWidth, xMax - ledOffset, 400_mils - ledWidth);
                        break;

                case 3: penLed = bitSet(bit) ? wxPen(colLedOn , ledWidth.pix(dpiHeight))
                                             : wxPen(colLedOff, ledWidth.pix(dpiHeight));
                        SetPen(penLed);
                        DrawLine(xMin - ledOffset + ledWidth, 400_mils, xMax - ledOffset - ledWidth, 400_mils);
                        break;

                case 4: penLed = bitSet(bit) ? wxPen(colLedOn , ledWidth.pix(dpiWidth))
                                             : wxPen(colLedOff, ledWidth.pix(dpiWidth));
			SetPen(penLed);
                        DrawLine(xMin, 250_mils + ledWidth, xMin - ledOffset, 400_mils - ledWidth);
                        break;

                case 5: penLed = bitSet(bit) ? wxPen(colLedOn , ledWidth.pix(dpiWidth))
                                             : wxPen(colLedOff, ledWidth.pix(dpiWidth));
                        SetPen(penLed);
                        DrawLine(xMin + ledOffset, 100_mils + ledWidth, xMin, 250_mils - ledWidth);
                        break;

                case 6: penLed = bitSet(bit) ? wxPen(colLedOn , ledWidth.pix(dpiHeight))
                                             : wxPen(colLedOff, ledWidth.pix(dpiHeight));
			SetPen(penLed);
                        DrawLine(xMin + ledWidth, 250_mils, xMax - ledWidth, 250_mils);
                        break;

                case 7: penLed = bitSet(bit) ? wxPen(colLedOn , ledWidth.pix(dpiHeight))
                                             : wxPen(colLedOff, ledWidth.pix(dpiHeight));
                        SetPen(penLed);
                        DrawCircle(xMax + ledOffset, 400_mils, ledWidth/2);
                        break;
            }
        }

        //Destroy Drawing Canvas
        DeleteDC();
    }
}}
