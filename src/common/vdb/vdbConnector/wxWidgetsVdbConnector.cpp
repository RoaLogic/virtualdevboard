/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets virtual Devboard Connector C++ source file        //
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

#include "wxWidgetsVdbConnector.hpp"

namespace RoaLogic {
    using namespace observer;
    using namespace dimensions;
namespace GUI {

    /**
     * @brief Construct a new wx widgets Connector window
     * @details This is the constructor for the IC window.
     */
    cWXVdbConnector::cWXVdbConnector(cVDBCommon* myVDBComponent, distancePoint position, wxWindow* windowParent, sVdbConnectorInformation* information) :
        cGuiVDBComponent(myVDBComponent, position),
        wxWindow(windowParent, wxID_ANY, wxPoint(wxDistancePoint(position, windowParent)), wxDefaultSize, wxTRANSPARENT_WINDOW),
        _myInformation(information)
    {
        SetInitialSize(GetDefaultSize());

        Connect(wxEVT_PAINT, wxPaintEventHandler(cWXVdbConnector::OnPaint));
    }

    
    /**
     * @brief Draw the IC
     * @details Draws an abstract Connector shape based on information
     *          The origin (0,0) is the top-left corner, not pin0
     * 
     * @param event 
     */
    void cWXVdbConnector::OnPaint(wxPaintEvent& event)
    {
        wxPaintDC dc(this);
        wxColour  myColour;

        wxSize  size = wxDistanceSize(_myInformation->width,_myInformation->height, this);
        wxSize  textSize;
        wxPoint textOrigin;
        double  angle;

        dc.SetLogicalOrigin(wxDistanceCoord(0,this),wxDistanceCoord(-6_mm,this));

        //Draw the connector (just a bunch of rectangles)
	//base
        myColour = wxColour(50,50,50);    //black
        dc.SetPen(wxPen(wxColour(myColour),1));
        dc.SetBrush(myColour);
        dc.DrawRectangle(wxPoint(0,0),size);

	//connector edge
        myColour = wxColour(43,43,43); //different black
        dc.SetBrush(myColour);
        dc.DrawRectangle(wxPoint(0,0),wxDistanceSize(_myInformation->width, 3_mm, this));
	//connector main
        dc.DrawRectangle(wxDistancePoint(7.3_mm,0,this),
                         wxDistanceSize(_myInformation->width - 14.6_mm,_myInformation->height,this));

        //metal sides (40x40)
        myColour = wxColour(172,189,193);
        dc.SetPen(wxPen(wxColour(myColour),1));
        dc.SetBrush(myColour);
        dc.DrawRectangle(wxDistancePoint(0.5_mm,3_mm,this),
                         wxDistanceSize(6.3_mm, 10_mm, this));
        dc.DrawRectangle(wxDistancePoint(_myInformation->width - 0.5_mm,3_mm,this),
                         wxDistanceSize(-6.3_mm, 10_mm, this));

        //metal connector
        dc.DrawRectangle(wxDistancePoint(7.3_mm,0,this),
                         wxDistanceSize(_myInformation->width - 15.6_mm, -6_mm, this));

        //metal bushings
        dc.DrawRectangle(wxDistancePoint(3.15_mm - 4.75_mm/2,0,this),
                         wxDistanceSize(4.75_mm,-4.75_mm,this));
        dc.DrawRectangle(wxDistancePoint(_myInformation->width -(3.15_mm - 4.75_mm/2),0,this),
                         wxDistanceSize(-4.75_mm,-4.75_mm,this));

        //Mounting holes
        myColour = wxColour(170,127,49); //bronze/gold
        dc.SetBrush(myColour);
        dc.DrawCircle(wxDistancePoint(3.15_mm,_myInformation->height/2,this),
                      wxDistanceCoord(1.6_mm,this));
        dc.DrawCircle(wxDistancePoint(_myInformation->width - 3.15_mm,_myInformation->height/2,this),
                      wxDistanceCoord(1.6_mm,this));

    }
}}
