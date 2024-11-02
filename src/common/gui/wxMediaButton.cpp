/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets Play/Pause Button                                 //
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

#include "wxMediaButton.hpp"

/**
*/
wxPlayPauseButton::wxPlayPauseButton(wxWindow* parent, wxWindowID id,
    const wxString& label, const wxPoint& pos, const wxSize& size,
    long style, const wxValidator& validator, const wxString& name)			     
{
    //Determine button phase
    if (label.IsSameAs("pause"),false)
    {
      phase = PAUSE;
    }
    else 
    {
      phase = PLAY;
    }

    //Create window
    Create(parent, id, pos, GetDefaultSize(), style, name);

    SetMinSize( GetDefaultSize() );
    scale = 0.8;

    //Bind window
    this->SetBackgroundStyle(wxBG_STYLE_PAINT);
    this->Bind(wxEVT_PAINT, &wxPlayPauseButton::OnPaint, this);
    this->Bind(wxEVT_ENTER_WINDOW, &wxPlayPauseButton::EnterWindow, this);
    this->Bind(wxEVT_LEAVE_WINDOW, &wxPlayPauseButton::LeaveWindow, this);
    this->Bind(wxEVT_LEFT_UP, &wxPlayPauseButton::MouseLeftUp, this);
}


/**
*/
wxString wxPlayPauseButton::GetLabel() const
{
  if (phase == PAUSE) return "Pause";
  else                return "Play";
}


/**
*/
void wxPlayPauseButton::SetLabel(const wxString& label)
{
    //Determine button phase
    if (label.IsSameAs("pause"),false)
    {
      phase = PAUSE;
    }
    else
    {
      phase = PLAY;
    }

    wxClientDC dc(this);
    Draw(dc);
}


/**
*/
wxSize wxPlayPauseButton::GetDefaultSize(wxWindow* win)
{
    return wxSize(50,50);
}


/**
    Draw button
*/
void wxPlayPauseButton::Draw(wxDC& dc)
{
    dc.Clear();
    dc.SetUserScale(scale, scale);

    int d = GetClientSize().GetX();                   //diameter

    if (phase == PLAY)
    {
        /** Draw PLAY button
        */
        //Draw background circle
        dc.SetBrush    ( wxColor(44,179,29) );            //green
        dc.SetPen      ( wxPen (wxColor(230,250,230),1)); //light line, 1 pixels thick
        dc.DrawEllipse (0,0,d,d);

        //Draw triangle
        dc.SetBrush( wxColor(20,82,13) );
        dc.SetPen  ( wxPen( wxColor(230,250,230),1 ));
        wxPointList *points = new wxPointList();
        wxPoint *pt1 = new wxPoint(15,10);
        wxPoint *pt2 = new wxPoint(40,25);
        wxPoint *pt3 = new wxPoint(15,40);
        points->Append(pt1);
        points->Append(pt2);
        points->Append(pt3);
        dc.DrawPolygon(points);
    }
    else
    {
        /** Draw PAUSE button
        */
        //Draw background circle
        dc.SetBrush    ( wxColor(5,5,232) );              //Blue
        dc.SetPen      ( wxPen (wxColor(230,250,230),1)); //light line, 1 pixels thick
        dc.DrawEllipse (0,0,d,d);

        //Draw double lines
        dc.SetPen  ( wxPen( wxColor(1,1,74),5));     //dark blue
        dc.DrawLine((d-10)/2,15,(d-10)/2,35);
        dc.DrawLine((d+10)/2,15,(d+10)/2,35);
    }
}


/**
    Called upon Paint event
*/
void wxPlayPauseButton::OnPaint(wxPaintEvent &evt)
{
    wxPaintDC dc(this);
    Draw(dc);
}


/**
    Called upon EnterWindow event
*/
void wxPlayPauseButton::EnterWindow(wxMouseEvent &evt)
{
    wxClientDC dc(this);
    scale = 1.0;
    Draw(dc);
}


/**
    Called upon LeaveWindow event
*/
void wxPlayPauseButton::LeaveWindow(wxMouseEvent &evt)
{
    wxClientDC dc(this);
    scale = 0.8;
    Draw(dc);
}


/**
    Called upon LeftMouseButtonUp event
*/
void wxPlayPauseButton::MouseLeftUp(wxMouseEvent &evt)
{
    if (phase == PAUSE)
    {
        phase = PLAY;
    }
    else
    {
        phase = PAUSE;
    }

    wxClientDC dc(this);
    Draw(dc);

    SendClickEvent(); 
}


/**
     Helpers
*/
bool wxPlayPauseButton::SendClickEvent()
{
    wxCommandEvent event(wxEVT_BUTTON, GetId());
    event.SetEventObject(this);

    return ProcessWindowEvent(event);
}

