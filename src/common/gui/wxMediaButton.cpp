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
#include "log.hpp"

/**************************************
 *
 * wxMediaButton class
 *
 */
wxMediaButton::wxMediaButton(wxWindow* parent, wxWindowID id,
    const wxString& label, const wxPoint& pos, const wxSize& size,
    long style, const wxValidator& validator, const wxString& name)			     
{
    //Create window
    Create(parent, id, pos, GetDefaultSize(), style, name);

    SetMinSize( GetDefaultSize() );
    scale = 0.8;

    //Bind window
    this->SetBackgroundStyle(wxBG_STYLE_PAINT);
    this->Bind(wxEVT_PAINT,        &wxMediaButton::OnPaint, this);
    this->Bind(wxEVT_ENTER_WINDOW, &wxMediaButton::EnterWindow, this);
    this->Bind(wxEVT_LEAVE_WINDOW, &wxMediaButton::LeaveWindow, this);
    this->Bind(wxEVT_LEFT_UP,      &wxMediaButton::MouseLeftUp, this);
}


/**
 * @brief Draw button
 * @details Empty method, must be overridden by child class 
 */
void wxMediaButton::Draw(wxDC& dc)
{
    dc.Clear();
}


/**
 * @brief OnPaint handler
 * @details Gets called upon Paint event
 */
void wxMediaButton::OnPaint(wxPaintEvent &evt)
{
    //call preHandler
    OnPaintPreHandle(evt);

    //repaint widget
    wxPaintDC dc(this);
    Draw(dc);

    //call postHandler
    OnPaintPostHandle(evt);
}


/**
 * @brief EnterWindow handler
 * @details Gets called upon EnterWindow event
 */
void wxMediaButton::EnterWindow(wxMouseEvent &evt)
{
    //call preHandle
    EnterWindowPreHandle(evt);

    //repaint widget
    scale = 1.0;
    wxClientDC dc(this);
    Draw(dc);

    //call postHandler
    EnterWindowPostHandle(evt);
}


/**
 * @brief LeaveWindow handler
 * @details Gets called upon LeaveWindow event
 */
void wxMediaButton::LeaveWindow(wxMouseEvent &evt)
{
    //call preHandle
    LeaveWindowPreHandle(evt);

    //repaint widget
    scale = 0.8;
    wxClientDC dc(this);
    Draw(dc);

    //call postHandle
    LeaveWindowPostHandle(evt);
}


/**
 * @brief LeftMouseButtonUp handler
 * @details Gets called upon LeftMouseButtonUp event
 */
void wxMediaButton::MouseLeftUp(wxMouseEvent &evt)
{
    //call preHandle
    MouseLeftUpPreHandle(evt);

    //repaint widget
    wxClientDC dc(this);
    Draw(dc);

    //call postHandle
    MouseLeftUpPostHandle(evt);

    //process event
    SendClickEvent(); 
}


/**
 * @brief SendClickEvent helper
 * @details Broadcasts button event
 */
bool wxMediaButton::SendClickEvent()
{
    wxCommandEvent event(wxEVT_BUTTON, GetId());
    event.SetEventObject(this);

    return ProcessWindowEvent(event);
}




/**************************************
 *
 * wxPlayPauseButton class
 *
 */
wxMediaPlayPauseButton::wxMediaPlayPauseButton(wxWindow* parent, wxWindowID id,
    const wxString& label, const wxPoint& pos, const wxSize& size,
    long style, const wxValidator& validator, const wxString& name) :
  wxMediaButton(parent, id, GetLabel(), pos, GetDefaultSize(), style, validator, name)
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
}


/**
 * @brief Returns label
 * @detail Returns either Play or Pause depending on the button state
 */
wxString wxMediaPlayPauseButton::GetLabel() const
{
  if (phase == PAUSE) return "Pause";
  else                return "Play";
}


/**
 * @brief Set label
 * @detail Set button label to Pause is label is Pause, otherwise sets label to Play
 */
void wxMediaPlayPauseButton::SetLabel(const wxString& label)
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

    //redraw button
    wxClientDC dc(this);
    Draw(dc);
}


/**
 * @brief Draw button
 * @details This method draws the button, scaled accordingly to 'scale' ensuring the button is always in the center of the widget 
 */
void wxMediaPlayPauseButton::Draw(wxDC& dc)
{
    dc.Clear();

    int width = GetClientSize().GetX();                   //unscaled widget width
    int offset = ((1.0-scale) * width)/2;

    if (phase == PLAY)
    {
        /** Draw PLAY button
        */
        //Draw background circle
        dc.SetBrush    (wxColor(44,179,29) );             //green
        dc.SetPen      (wxPen(wxColor(230,250,230),1));   //light line, 1 pixels thick
        dc.DrawCircle  (width/2, width/2, width/2*scale);

        //Draw triangle
        dc.SetBrush(wxColor(20,82,13) );
        dc.SetPen  (wxPen(wxColor(230,250,230),1));
        wxPointList *points = new wxPointList();
        wxPoint *pt1 = new wxPoint(width*0.3 +offset, width*0.2 +offset);
        wxPoint *pt2 = new wxPoint(width*0.8 -offset, width/2.0        );
        wxPoint *pt3 = new wxPoint(width*0.3 +offset, width*0.8 -offset);
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
        dc.SetBrush   (wxColor(2,2,252));                //Blue
        dc.SetPen     (wxPen(wxColor(230,250,230),1));   //light line, 1 pixels thick
        dc.DrawCircle (width/2, width/2, width/2*scale);

        //Draw double lines
        dc.SetPen  (wxPen(wxColor(1,1,74),5*scale));      //dark blue
        dc.DrawLine(width/2 -5*scale, width*0.3 +offset, width/2 -5*scale, width*0.7 -offset);
        dc.DrawLine(width/2 +5*scale, width*0.3 +offset, width/2 +5*scale, width*0.7 -offset);
    }
}


/**
    Called before the LeftMouseButtonUp event is handled
 */
void wxMediaPlayPauseButton::MouseLeftUpPreHandle(wxMouseEvent &evt)
{
    if (phase == PAUSE)
    {
        phase = PLAY;
    }
    else
    {
        phase = PAUSE;
    }
}




/**************************************
 *
 * wxStopButton class
 *
 */
wxMediaStopButton::wxMediaStopButton(wxWindow* parent, wxWindowID id,
    const wxString& label, const wxPoint& pos, const wxSize& size,
    long style, const wxValidator& validator, const wxString& name) :
  wxMediaButton(parent, id, GetLabel(), pos, GetDefaultSize(), style, validator, name)
{
}


/**
 * @brief Draw button
 * @details This method draws the button, scaled accordingly to 'scale' ensuring the button is always in the center of the widget 
 */
void wxMediaStopButton::Draw(wxDC& dc)
{
    dc.Clear();

    int width = GetClientSize().GetX();                   //unscaled widget width
    int offset = ((1.0-scale) * width)/2;

    //Draw background circle
    dc.SetBrush     (wxColor(173,5,5));                   //Dark Red
    dc.SetPen       (wxPen(wxColor(230,250,230),1));      //light line, 1 pixels thick
    dc.DrawCircle   (width/2, width/2, width/2*scale);

    //Draw box
    dc.SetBrush     (wxColor(107,2,2));                   //Dark dark red
    dc.SetPen       (wxPen(wxColor(230,250,230),1));
    dc.DrawRectangle(width/2 -12*scale, width/2 -12*scale, 25*scale, 25*scale);
}




/**************************************
 *
 * wxPowerButton class
 *
 */
wxMediaPowerButton::wxMediaPowerButton(wxWindow* parent, wxWindowID id,
    const wxString& label, const wxPoint& pos, const wxSize& size,
    long style, const wxValidator& validator, const wxString& name) :
  wxMediaButton(parent, id, GetLabel(), pos, GetDefaultSize(), style, validator, name)
{
}


/**
 * @brief Draw button
 * @details This method draws the button, scaled accordingly to 'scale' ensuring the button is always in the center of the widget 
 */
void wxMediaPowerButton::Draw(wxDC& dc)
{
    dc.Clear();

    int width = GetClientSize().GetX();                   //unscaled widget width

    //Draw background circle
    dc.SetBrush       (wxColor(164,28,237));               //Purple
    dc.SetPen         (wxPen(wxColor(230,250,230),1));     //light line, 1 pixels thick
    dc.DrawCircle     (width/2, width/2, width/2*scale);

    //Draw arc
    dc.SetPen         (wxPen(wxColor(250,140,5),3));
    dc.DrawEllipticArc(width/2 -12*scale, width/2 -12*scale, 25*scale, 25*scale, //rectangle that contains arc
		       135, 45);
    //Draw line
    dc.DrawLine       (width/2, width/2, width/2, width/2 -15*scale);
}

