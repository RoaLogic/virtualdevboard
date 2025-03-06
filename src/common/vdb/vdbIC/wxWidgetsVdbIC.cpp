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

namespace RoaLogic {
    using namespace observer;
    using namespace dimensions;
namespace GUI {

    /**
     * @brief Construct a new wx widgets IC window
     * @details This is the constructor for the IC window.
     */
    cWXVdbIC::cWXVdbIC(cVDBCommon* myVDBComponent, distancePoint position, wxWindow* windowParent, sVdbICInformation* information, double angle) :
        cWXVdbBase(myVDBComponent, position, windowParent, information, distanceSize(information->width,information->height), angle)
    {
//        SetSize(GetDefaultSize());
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
        //Create New Drawing Canvas
        NewDC();

        distanceSize size = GetDeviceSize();

        wxSize        wxTextSize;
        distanceSize  textSize;
        distancePoint textOrigin;
        double        textAngle;

        //Draw the IC (just a rectangle)
        wxColour ICColour  = wxColour(55,50,45);    //black
        SetPen(wxPen(wxColour(ICColour),1));
        SetBrush(ICColour);
        DrawRectangle(distancePoint(0,0),size);

        //add light lines for 3D effect
        SetPen(wxPen(wxColour(150,140,130),2));
        DrawLine(0,0,size.width,0);
        DrawLine(0,0,0,size.height);

        //add label
        wxString label = reinterpret_cast<sVdbICInformation*>(GetInformation())->label;
        SetFont(*wxNORMAL_FONT);
        SetTextForeground(*wxWHITE);
        wxTextSize = GetTextExtent(label);

        textSize.width = 1_inch * wxTextSize.GetWidth() / GetDPI().GetWidth();
        textSize.height = 1_inch * wxTextSize.GetHeight() / GetDPI().GetHeight();
        if (size.height > size.width)
        {
            textAngle = 90;
            textOrigin.x = (size.width - textSize.height)/2;
            textOrigin.y = (size.height + textSize.width)/2;
        }
        else
        {
            textAngle = 0;
            textOrigin.x = (size.width - textSize.width)/2;
            textOrigin.y = (size.height - textSize.height)/2;
        }

        DrawRotatedText(label, textOrigin, textAngle);

        //Destroy Drawing Canvas
        DeleteDC();
    }
}}
