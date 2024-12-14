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
        wxWindow(windowParent, wxID_ANY, wxPoint(wxDistancePoint(position, windowParent)), wxDefaultSize, wxTRANSPARENT_WINDOW),
        _myInformation(information)
    {
        SetInitialSize(GetDefaultSize());

        Connect(wxEVT_PAINT, wxPaintEventHandler(cWXVdbIC::OnPaint));
    }

    
    /**
     * @brief Draw the IC
     * @details Draws an abstract IC shape based on information.size
     *          The origin (0,0) is the top-left corner, not pin0
     * 
     * @param event 
     */
    void cWXVdbIC::OnPaint(wxPaintEvent& event)
    {
        wxPaintDC dc(this);

        wxSize  size = wxDistanceSize(_myInformation->width,_myInformation->height, this);
        wxSize  textSize;
        wxPoint textOrigin;
        double  angle;

        //Draw the IC (just a rectangle)
        wxColour ICColour  = wxColour(55,50,45);    //black
        dc.SetPen(wxPen(wxColour(ICColour),1));
        dc.SetBrush(ICColour);
        dc.DrawRectangle(wxPoint(0,0),size);

        //add light lines for 3D effect
        dc.SetPen(wxPen(wxColour(150,140,130),2));
        dc.DrawLine(0,0,size.GetWidth(),0);
        dc.DrawLine(0,0,0,size.GetHeight());

        //add label
        dc.SetFont(*wxNORMAL_FONT);
        dc.SetTextForeground(*wxWHITE);
        textSize = GetTextExtent(_myInformation->label);
        if (size.GetHeight() > size.GetWidth())
        {
            angle = 90;
            textOrigin.x = (size.GetWidth() - textSize.GetHeight())/2;
            textOrigin.y = (size.GetHeight() + textSize.GetWidth())/2;
        }
        else
        {
            angle = 0;
            textOrigin.x = (size.GetWidth() - textSize.GetWidth())/2;
            textOrigin.y = (size.GetHeight() - textSize.GetHeight())/2;
        }
        dc.DrawRotatedText(_myInformation->label, textOrigin, angle);
    }
}}
