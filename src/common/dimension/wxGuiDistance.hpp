/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    wxWidgets distance class                                     //
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

#ifndef WX_GUI_DISTANCE_HPP
#define WX_GUI_DISTANCE_HPP

#include <wx/wxprec.h>
#include <wx/wx.h>
#include "distance.hpp"

using namespace RoaLogic::dimensions;

class wxGuiDistance
{
    public:
    static int scaleWidth  (int   width, wxWindow* window)  { return width  * window->GetDPI().GetWidth(); }
    static int scaleWidth  (float width, wxWindow* window)  { return width  * window->GetDPI().GetWidth(); }
    static int scaleHeight (int   height, wxWindow* window) { return height * window->GetDPI().GetHeight(); }
    static int scaleHeight (float height, wxWindow* window) { return height * window->GetDPI().GetHeight(); }

    static wxSize convertSize(distancePoint point, wxWindow* window)
    {
        wxSize newSize;

        newSize.SetWidth (point.x.pix(window->GetDPI().GetWidth ()));
        newSize.SetHeight(point.y.pix(window->GetDPI().GetHeight()));

        return window->FromDIP(newSize);
    }

    static wxPoint convertPoint(distancePoint point, wxWindow* window)
    {
        wxPoint newPoint;

        newPoint.x = point.x.pix(window->GetDPI().GetWidth ());
        newPoint.y = point.y.pix(window->GetDPI().GetHeight());

        return newPoint;
    }
};

#endif
