/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets virtual Devboard Base C++ header file             //
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

#ifndef WX_WIDGETS_VDB_BASE_HPP
#define WX_WIDGETS_VDB_BASE_HPP

#include <wx/wxprec.h>
#include <wx/wx.h>
#include "wx/event.h"
#include <wx/graphics.h>

#include "gui_interface.hpp"
#include "wxGuiDistance.hpp"

namespace RoaLogic {
    using namespace observer;
    using namespace vdb;
namespace GUI {

    /**
     * @class cWXVdbBase
     * @author Richard Herveille
     * @brief virtual development board component base class
     * 
     * @details
     * This class is the base for all VDB GUI components
     */
    class cWXVdbBase : public cGuiVDBComponent, public wxWindow
    {
        private:
        void*          _information;                    //Device specific information
        distanceSize   _size;                           //default size
        double         _angle;                          //Rotation angle
        double         _sinAngle = 0.0;                 //sin(_angle)
        double         _cosAngle = 1.0;                 //cos(_angle)
        distancePoint  _origin;
        wxPaintDC*     _dc = nullptr;                   //Drawing Canvas


        protected:
        /**
         * @brief notify function from the vdb component
         * @details This function receives events from the component it is registered to.
         * @note this function runs in the verilated context.
         */
        virtual void notify(eEvent aEvent, void* data) {}

	/**
         * @brief Handle the event
         * @details This function handles the events
         * @note This function runs in the GUI thread
         */
        virtual void onEvent(wxCommandEvent& event) {}

        public:
	/**
	 * @brief Constructor
	 */
        cWXVdbBase(cVDBCommon* myVDBComponent, distancePoint position, wxWindow* windowParent, void* Information, const distanceSize& size=distanceSize(5_mm,5_mm), double angle=0) :
            cGuiVDBComponent(myVDBComponent, position),
            wxWindow(windowParent, wxID_ANY, wxDistancePoint(position, windowParent), wxDefaultSize, wxTRANSPARENT_WINDOW),
            _information(Information), _size(size), _angle(angle)
        {
            SetAngle(angle);
            SetSize(DoGetBestSize());
        }

	/**
	 * @brief Destructor
	 */
        ~cWXVdbBase() {}

        /**
         * @brief Returns void* to information
         */
        void* GetInformation() const { return _information; }

        /**
         * @brief Component size
         * @details Overload this function to return the size of the component
         */
        virtual distanceSize GetDeviceSize() const { return _size; }

        /**
         * @brief Default component size
         * @details Returns the default size for the component.
	 *
            The optional @a win argument is new since wxWidgets 3.1.3 and allows to
            get a per-monitor DPI specific size.
         */
        virtual wxSize GetDefaultSize(wxWindow* win = NULL) const
        {
//            if (win == NULL) { win = this; }
            return wxRotate(GetDeviceSize());
        }


        /**
         * @brief Returns best size for custom control. Same size as client
         */
        virtual wxSize DoGetBestSize() const { return DoGetBestClientSize(); }

        /**
         * @brief Returns best size for custom control based on client size
         */
        virtual wxSize DoGetBestClientSize() const { return GetDefaultSize(); }


        /**
	 * @brief Paint the widget
	 * @details This function paints the widget.
	 */
        virtual void OnPaint(wxPaintEvent& event) {}


       /**
        * Helper Routines for Drawing Canvas Routines
        */

       /**
        * @brief Create new drawing canvas object
        */
       virtual void NewDC()
       {
           DeleteDC();
           _dc = new wxPaintDC(this);
       }

       /**
        * @brief Delete drawing canvas object
        */
       virtual void DeleteDC()
       {
           if (_dc != nullptr)
           {
               delete _dc;
               _dc = nullptr;
           }
       }

       /**
        * @brief Sets angle
        */
       virtual void SetAngle(double angle)
       {
           double radAngle;

           while (angle >=  360) angle -= 360;
           while (angle <= -360) angle += 360;
           _angle = angle;

           //XY: X increases left-to-right, Y increases bottom-to-top
           //Video: X increases left-to-right, Y increases top-to-bottom
           //Invert angle to ensure counter-clockwise rotation
           angle = -_angle;

           radAngle = AngleToRadians(angle);

           _sinAngle = std::sin(radAngle);
           _cosAngle = std::cos(radAngle);
       }


       /**
        * @brief Convert angle to radians
        */
       double AngleToRadians(double angle) const { return (2.0 * std::numbers::pi * angle / 360.0); }

       /**
        * @brief Returns size rotated by angle
        */
       distanceSize Rotate(const distanceSize size) const
       {
           distanceSize newSize;
           double absSin = (_sinAngle < 0) ? -_sinAngle : _sinAngle;
           double absCos = (_cosAngle < 0) ? -_cosAngle : _cosAngle;

           //transpose width and height
           newSize.width  = (absCos * size.width ) + (absSin * size.height);
           newSize.height = (absCos * size.height) + (absSin * size.width );

           return newSize;
       }
       wxSize wxRotate(const distanceSize sz) const
       {
           return wxDistanceSize(Rotate(sz),this);
       }


       /**
        * @brief Rotates point by angle around the center of the component
        */
       distancePoint Rotate(const cDistance x, const cDistance y) const
       {
           //normalize around rotation point (center of logical canvas) before rotation
           cDistance x_offset = (x - _origin.x) - GetDeviceSize().width/2.0;
           cDistance y_offset = (y - _origin.y) - GetDeviceSize().height/2.0;

           distancePoint newPoint;

           //transpose x and y
           //Canvas size changed due to rotation, move to center of resized canvas
           newPoint.x = (x_offset * _cosAngle) - (y_offset * _sinAngle) + Rotate(GetDeviceSize()).width/2.0;
           newPoint.y = (x_offset * _sinAngle) + (y_offset * _cosAngle) + Rotate(GetDeviceSize()).height/2.0;

           return newPoint;
       }
       distancePoint Rotate(const distancePoint pt) const
       {
           return Rotate(pt.x, pt.y);
       }
       wxPoint wxRotate(const distancePoint pt) const
       {
           return wxDistancePoint(Rotate(pt),this);
       }

       /**
        * Drawing Canvas Routines
        */

       /**
        * @brief Change the offset used for translating coordinates
        */
       void SetLogicalOrigin(const distancePoint& pt)
       {
           _origin = pt;
       }
       void SetLogicalOrigin(cDistance x, cDistance y)
       {
           SetLogicalOrigin( distancePoint(x,y) );
       }

       /**
        * @brief Sets the current brush
        */
       void SetBrush(const wxBrush& brush)
       {
           assert(_dc != nullptr);
           _dc->SetBrush(brush);
       }

       /**
        * @brief Sets the current pen
        */
       void SetPen(const wxPen& pen)
       {
           assert(_dc != nullptr);
           _dc->SetPen(pen);
       }

       /**
        * @brief Draws a rectangle
        * @details Draws a rectangle using DrawPolygon to support rotatation
        */
       void DrawRectangle(cDistance x, cDistance y, cDistance width, cDistance height)
       {
           assert(_dc != nullptr);
           wxPointList *points = new wxPointList();
           wxPoint *pt1 = new wxPoint( wxRotate(distancePoint(x      ,y       )) );
           wxPoint *pt2 = new wxPoint( wxRotate(distancePoint(x+width,y       )) );
           wxPoint *pt3 = new wxPoint( wxRotate(distancePoint(x+width,y+height)) );
           wxPoint *pt4 = new wxPoint( wxRotate(distancePoint(x      ,y+height)) );
           points->Append(pt1);
           points->Append(pt2);
           points->Append(pt3);
           points->Append(pt4);
           _dc->DrawPolygon(points);
       }
       void DrawRectangle(const distancePoint& pt, const distanceSize& sz)
       {
           DrawRectangle(pt.x, pt.y, sz.width, sz.height);
       }

       /**
        * @brief Draws a circle
        */
       void DrawCircle(const distancePoint& pt, cDistance radius)
       {
           assert(_dc != nullptr);
           _dc->DrawCircle( wxRotate(pt), wxDistanceCoord(radius,this) );
       }
       void DrawCircle(cDistance x, cDistance y, cDistance radius)
       {
           DrawCircle(distancePoint(x,y),radius);
       }

       /**
        * @brief Draws a line
        */
       void DrawLine(const distancePoint& pt1, const distancePoint& pt2)
       {
           assert(_dc != nullptr);
           _dc->DrawLine( wxRotate(pt1), wxRotate(pt2) );
       }
       void DrawLine(cDistance x1, cDistance y1, cDistance x2, cDistance y2)
       {
           DrawLine(distancePoint(x1,y1),distancePoint(x2,y2));
       }


       /**
        * @brief Sets the font
        */
       bool SetFont(const wxFont& font)
       {
           assert(_dc != nullptr);
           _dc->SetFont(font);
           return true;
       }
/*
       void SetFont(const wxFont& font)
       {
           assert(_dc != nullptr);
           _dc->SetFont(font);
       }
*/
       /**
        * @brief Sets the text background colour
        */
       void SetTextBackground (const wxColour& colour)
       {
           assert(_dc != nullptr);
           _dc->SetTextBackground(colour);
       }

       /**
        * @brief Sets the text foreground colour
        */
       void SetTextForeground (const wxColour& colour)
       {
           assert(_dc != nullptr);
           _dc->SetTextForeground(colour);
       }

       /**
        * @brief Draws rotated text
        * @details Draws the text rotated by angle degrees. Positive angles are counterclockwise. The full angle is 360 degrees
        */
       void DrawRotatedText (const wxString& text, const distancePoint& point, double angle)
       {
           assert(_dc != nullptr);
           _dc->DrawRotatedText(text, wxRotate(point), angle + _angle);
       }
       void DrawRotatedText (const wxString& text, cDistance x, cDistance y, double angle)
       {
           DrawRotatedText(text, distancePoint(x,y), angle);
       }

    };

}}

#endif
