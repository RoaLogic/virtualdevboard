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

class wxDistanceCoord : public cDistance
{
    private:
    wxWindow* window;

    public:
    /**
     * @brief constructor
     */
    wxDistanceCoord (wxWindow* window) : window(window), cDistance(){}
    wxDistanceCoord (long double val, wxWindow* window) : window(window), cDistance(val){}

    /**
     * @brief destructor
     */
    ~wxDistanceCoord(){}

    /**
     * @brief Conversion to wxCoord
     * @details Converts distance to wxCoord assuming horizontal and vertical conversion factor is the same
     */
    operator wxCoord() const { return window->ToPhys( window->FromDIP( inch() * window->GetDPI().GetWidth() ) ); }
};


class wxDistanceSize
{
    private:
    wxWindow *window;
    distanceSize size;

    public:
    /**
     * @brief constructor
     */
    wxDistanceSize (wxWindow* window) : window(window){}
    wxDistanceSize (distanceSize size, wxWindow* window) : window(window), size(size){}
    wxDistanceSize (cDistance width, cDistance height, wxWindow* window) : window(window) {
      SetWidth(width);
      SetHeight(height);
    }

    /**
     * @brief destructor
     */
    ~wxDistanceSize(){}

    /**
     * @brief sets width
     */
    void SetWidth(cDistance width) { size.width = width; }

    /**
     * @brief sets height
     */
    void SetHeight(cDistance height) { size.height = height; }

    /**
     * @brief GetWidth() returns width
     */
    cDistance GetWidth() const { return size.width; }

    /**
     * @brief GetHeight() returns height
     */
    cDistance GetHeight() const { return size.height; }

    /**
     * @brief conversion to distanceSize
     */
    operator distanceSize() const { return size; }

    /**
     * @brief Conversion to wxSize
     * @details Converts width and height distance to wxSize
     */
    operator wxSize() const {
        return window->ToPhys(window->FromDIP(
                   wxSize( size.width.inch() * window->GetDPI().GetWidth(),
                           size.height.inch() * window->GetDPI().GetHeight() )
               ));
    }
};


class wxDistancePoint
{
    private:
    wxWindow *window;

    public:
    cDistance x;
    cDistance y;

    /**
     * @brief constructor
     */
    wxDistancePoint (wxWindow* window) : window(window){}
    wxDistancePoint (distancePoint point, wxWindow* window) : window(window), x(point.x), y(point.y){}
    wxDistancePoint (cDistance x, cDistance y, wxWindow* window) : window(window), x(x), y(y) {}

    /**
     * @brief destructor
     */
    ~wxDistancePoint(){}

    /**
     * @brief conversion to distancePoint
     */
    operator distancePoint() const { return distancePoint(x,y); }

    /**
     * @brief Conversion to wxPoint
     * @details Converts (x,y) to wxPoint
     */
    operator wxPoint() const {
        return window->ToPhys(window->FromDIP(
                   wxPoint( x.inch() * window->GetDPI().GetWidth(),
                            y.inch() * window->GetDPI().GetHeight() )
               ));
      }
};



class wxGuiDistance
{
    public:

    static wxCoord convertCoord(cDistance x, wxWindow* window)
    {
      wxCoord newCoord;

      newCoord = x.inch() * window->GetDPI().GetWidth();
      newCoord = window->FromDIP(newCoord);

      return window->ToPhys(newCoord);
    }

    static wxSize convertSize(distanceSize size, wxWindow* window)
    {
        wxSize newSize;

        newSize.SetWidth (size.width.pix(window->GetDPI().GetWidth ()));
        newSize.SetHeight(size.height.pix(window->GetDPI().GetHeight()));

        return window->ToPhys(window->FromDIP(newSize));
    }

    static wxPoint convertPoint(distancePoint point, wxWindow* window)
    {
        wxPoint newPoint;

        newPoint.x = point.x.pix(window->GetDPI().GetWidth ());
        newPoint.y = point.y.pix(window->GetDPI().GetHeight());

        return window->ToPhys(newPoint);
    }
};

#endif
