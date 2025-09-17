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
//    Copyright (C) 2024-2025 Roa Logic BV - www.roalogic.com      //
//    Copyright (C) 2024-2025 richard.herveille@roalogic.com       //
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

#include "wxWidgetsVdbButton.hpp"
#include "wxGuiDistance.hpp"
#include "distance.hpp"

// Define the wxEVT_BUTTON, which is special within this class
wxDEFINE_EVENT(wxEVT_BUTTON, wxCommandEvent);

namespace RoaLogic {
    using namespace observer;
    using namespace dimensions;
namespace GUI {

    /**
     * @brief Construct a new wx widgets Button window
     * @details 
     * This is the constructor for the Button window. Bind the 
     * onButtonEvent function with the ID to a wxEVT_BUTTON event.
     * 
     */
    cWXVdbButton::cWXVdbButton(cVDBCommon* myVDBComponent, distancePoint position, wxWindow* windowParent, sVdbLedInformation* information, double angle) :
        cWXVdbBase(myVDBComponent, position, windowParent, information, GetDeviceSize(information), angle)
    {
        Connect(wxEVT_PAINT, wxPaintEventHandler(cWXVdbButton::OnPaint));
    }

    /**
     * @brief Draws the Button
     * 
     * @param event 
     */
    void cWXVdbButton::OnPaint(wxPaintEvent& event)
    {
        //Based on 'type' draw the button
        sVdbButtonInformation* myInformation = reinterpret_cast<sVdbButtonInformation*>(GetInformation());

    }

    /**
     * @brief Draws a pushbutton
     */
    void cWXVdbButton::DrawPushButton();
    {
        const distanceSize size = GetDeviceSize();

        //Create new Drawing Canvas
        NewDC();

        //draw background
        wxColour Colour = wcColour(55,55,55);
        SetPen(wxPen(wxColour(0,0,0),1));
        SetBrush(ledColour);
        DrawRoundedRectange(0,0,6.5_mm, 6.5_mm,-10);

        //Destroy Drawing Canvas
        DeleteDC();
    }


    /**
     * @brief Draws a toggle button
     */

    /**
     * @brief Draws one-switch of a dip-switch
     */

}}
