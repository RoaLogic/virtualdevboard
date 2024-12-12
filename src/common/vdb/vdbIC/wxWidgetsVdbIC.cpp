/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets virtual Devboard IC C++ source file               //
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

#include "wxWidgetsVdbIC.hpp"
//#include "wxGuiDistance.hpp"
//#include "distance.hpp"

namespace RoaLogic {
    using namespace observer;
    using namespace dimensions;
namespace GUI {

    /**
     * @brief Construct a new wx widgets IC window
     * @details This is the constructor for the IC window.
     */
    cWXVdbIC::cWXVdbIC(cVDBCommon* myVDBComponent, distancePoint position, wxWindow* windowParent, sVdbICInformation* information) :
        cGuiVDBComponent(myVDBComponent, position),
        wxWindow(windowParent, wxID_ANY, wxDistancePoint(position, windowParent), wxDefaultSize, wxTRANSPARENT_WINDOW),
        _myInformation(information)
    {
        SetInitialSize(GetDefaultSize());
    }

    
    /**
     * @brief Draw the IC
     * @details Draws an IC shape based on information.type and size given by GetDeviceSize.
     *          The origin (0,0) is the top-left corner, not pin0
     * 
     * @param event 
     */
    void cWXVdbIC::OnPaint(wxPaintEvent& event)
    {
        const int x = GetDeviceSize().width.pix(GetDPI().GetWidth());
        const int y = GetDeviceSize().height.pix(GetDPI().GetHeight());
        wxDistancePoint point = wxDistancePoint(this);

        wxPaintDC dc(this);

        //First Draw pins, if visible
        wxColour PinColour = wxColour(188,198,204); //shiny silver (apparently)
        dc.SetPen(wxPen(wxColour(PinColour),1));
        dc.SetBrush(PinColour);
	switch (_myInformation->type)
        {
            case eVdbICType::SIP   :
                for (int n=0; n < _myInformation->pinCount; n++)
                {
                    dc.DrawCircle(cDistance(_myInformation->padPitch/2 + n*_myInformation->padPitch).pix(GetDPI().GetWidth()),
                                  y/2,
                                  cDistance(_myInformation->padPitch/2).pix(GetDPI().GetWidth()));
                }
                break;

            case eVdbICType::DIP   :
                for (int n=0; n < _myInformation->pinCount/2; n++)
                {
                    //top row
                    dc.DrawRectangle(cDistance(_myInformation->padPitch/2 + n*_myInformation->padPitch).pix(GetDPI().GetWidth()),
                                     0,
                                     cDistance(_myInformation->padPitch   + n*_myInformation->padPitch).pix(GetDPI().GetWidth()),
                                     -cDistance(25_mils).pix(GetDPI().GetHeight()));
                    //bottom row
                    dc.DrawRectangle(cDistance(_myInformation->padPitch/2 + n*_myInformation->padPitch).pix(GetDPI().GetWidth()),
                                     y,
                                     cDistance(_myInformation->padPitch   + n*_myInformation->padPitch).pix(GetDPI().GetWidth()),
                                     cDistance(GetDeviceSize().height + 25_mils).pix(GetDPI().GetHeight()));
                }
                break;

            case eVdbICType::QIP   :
            case eVdbICType::SOP   :
            case eVdbICType::TSOP  :
            case eVdbICType::TSOP2 :
                {
                    const cDistance padPitch  = 0.229_mm;
                    const cDistance padWidth  = 0.131_mm;
                    const cDistance padHeight = 0.334_mm;
                }
                break;

            case eVdbICType::SOJ   :
            case eVdbICType::SOIC  : 
            case eVdbICType::QFN   :
            default                : ;
	}

        //Then Draw the IC (just a rectangle)
        wxColour ICColour  = wxColour(36,36,36);    //antracit black
        dc.SetPen(wxPen(wxColour(ICColour),1));
        dc.SetBrush(ICColour);
        dc.DrawRectangle(0,0,x,y);
    }


    /**
     * @brief Return size of the IC
     *        Returns a ridiculously large number to indicate an unknown type without breaking functionality
     */
    distanceSize cWXVdbIC::GetDeviceSize()
    {
        switch (_myInformation->type)
	{
            case eVdbICType::SIP   :
            case eVdbICType::DIP   :
            case eVdbICType::QIP   :
            case eVdbICType::SOP   :
            case eVdbICType::TSOP  :
            case eVdbICType::TSOP2 :
            case eVdbICType::SOJ   :
            case eVdbICType::SOIC  : return distanceSize(_myInformation->padPitch * (_myInformation->pinCount/2 +1),
                                                         _myInformation->bodyHeight);
            case eVdbICType::QFN   : return distanceSize(_myInformation->padPitch * (_myInformation->pinCount/4 +1),
                                                         _myInformation->padPitch * (_myInformation->pinCount/4 +1));
            case eVdbICType::PGA   : 
            case eVdbICType::BGA   :
            case eVdbICType::CSP   : return distanceSize(_myInformation->bodyWidth, _myInformation->bodyHeight);
            default                : return distanceSize(100_mm, 100_mm);
	}
    }
}}
