/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets virtual Devboard Header C++ source file           //
//                                                                 //
/////////////////////////////////////////////////////////////////////
//                                                                 //
//    Copyright (C) 2025 Roa Logic BV - www.roalogic.com           //
//    Copyright (C) 2025 richard.herveille@roalogic.com            //
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

#include "wxWidgetsVdbHeader.hpp"

namespace RoaLogic {
    using namespace observer;
    using namespace dimensions;
namespace GUI {

    /**
     * @brief Construct a new wx widgets Header window
     * @details This is the constructor for the header window.
     */
    cWXVdbHeader::cWXVdbHeader(cVDBCommon* myVDBComponent, distancePoint position, wxWindow* windowParent, sVdbHeaderInformation* information, double angle) :
        cWXVdbBase(myVDBComponent, position, windowParent, information, GetDeviceSize(information), angle)
    {
        Connect(wxEVT_PAINT, wxPaintEventHandler(cWXVdbHeader::OnPaint));
    }

    
    /**
     * @brief Draw the Header
     * @details Draws an abstract Header shape based on information
     *          The origin (0,0) is the top-left corner, not pin1
     *          TODO: make pin1 the origin
     * 
     * @param event 
     */
    void cWXVdbHeader::OnPaint(wxPaintEvent& event)
    {
        const distanceSize size = GetDeviceSize();

        //Create Drawing Canvas
        NewDC();

        sVdbHeaderInformation* information = reinterpret_cast<sVdbHeaderInformation*>(GetInformation() );
        cDistance pitch = information->pitch;
        int  rows       = information->rows;
        int  columns    = information->columns;
        bool female     = int(information->type) & HDR_FEMALE;
        bool boxed      = int(information->type) & HDR_BOXED;

        wxColour  myColour;

        //Draw the header base
        myColour = wxColour(50,50,50);    //black
        SetPen(wxPen(wxColour(0,0,0),1));
        SetBrush(myColour);
        DrawRectangle(0,0,size.width,size.height);

        //Draw the contacts
        cDistance rowOffset;
        cDistance columnOffset;
        distancePoint pt;
        distanceSize  sz;


        //Draw Connectors
        sz.width  = pitch /4;
        sz.height = pitch /4;

        if (boxed)
        {
            columnOffset = 2.0  * pitch;
            rowOffset    = 0.75 * pitch + 1.25_mm;
        }
        else
        {
            columnOffset = 0.5 * pitch;
            rowOffset    = 0.5 * pitch;
        }
        columnOffset -= sz.width/2;
        rowOffset    -= sz.height/2;

        myColour = female ? wxColour(0,0,0) : colGold;
        SetPen(wxPen(wxColour(myColour),1));
        SetBrush(myColour);
        for (int column = 0; column < columns; column++)
        for (int row    = 0; row    < rows;    row++  )
        {
           pt.x = column*pitch + columnOffset;
           pt.y = row*pitch    + rowOffset;
           DrawRectangle(pt, sz);
        }

        //Destroy Drawing Canvas
        DeleteDC();
    }


    /**
     * @brief Return size of the header
     *        Returns 10x10 to indicate an unknown type
     */
    distanceSize cWXVdbHeader::GetDeviceSize() const
    {
        return GetDeviceSize( reinterpret_cast<sVdbHeaderInformation*>(GetInformation()) );
    }

    distanceSize cWXVdbHeader::GetDeviceSize(sVdbHeaderInformation* information) const
    {
        cDistance pitch = information->pitch;
        int  rows       = information->rows;
        int  columns    = information->columns;
        bool boxed      = int(information->type) & HDR_BOXED;

        distanceSize size;

        if (boxed)
        {
            size.width  = (columns +3.0) * pitch;
            size.height = (rows    +0.5) * pitch + 2.5_mm;
        }
        else
        {
            size.width  = columns * pitch;
            size.height = rows    * pitch;
        }
        return size;
    }
}}
