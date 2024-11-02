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

#ifndef WXPLAYPAUSEBUTTON_HPP
#define WXPLAYPAUSEBUTTON_HPP

#include <wx/wx.h>
#include <wx/graphics.h>
#include <wx/sizer.h>
#include <wx/dcbuffer.h>

class wxPlayPauseButton : public wxWindow
{
    private:
    /**
       Button phase, play=false, pause=true
    */
    bool phase;
    const bool PAUSE = true;
    const bool PLAY  = false;
    double scale = 0.8;


    public:
    /**
        Default ctor
    */
    wxPlayPauseButton();


    /**
        Constructor, creating and showing the button

        Same parameter as wxButton for compatibility reasons
    */
    wxPlayPauseButton(wxWindow* parent, wxWindowID id,
                      const wxString& label = wxEmptyString,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxButtonNameStr);
    /**
        Returns @true if an authentication needed symbol is displayed on the
        button.

        @remarks This method always returns @false
    */
    bool GetAuthNeeded() const { return false; }


    /**
        Returns the default size for the buttons. It is advised to make all the
        dialog buttons of the same size and this function allows retrieving the
        (platform, and current font dependent) size which should be the best
        suited for this.

        The optional @a win argument is new since wxWidgets 3.1.3 and allows to
        get a per-monitor DPI specific size.
    */
    static wxSize GetDefaultSize(wxWindow* win = NULL);

    /**
        Returns the string label for the button. Either "Play" or "Pause"

        @see SetLabel()
    */
    wxString GetLabel() const;


    /**
        Sets the string label for the button.

        @param label
            The label to set.
    */
    void SetLabel(const wxString& label);


    /**
        Draw the button
    */
    void Draw(wxDC& dc);


    /**
        Events
    */
    void OnPaint(wxPaintEvent& evt);
    void EnterWindow(wxMouseEvent& evt);
    void LeaveWindow(wxMouseEvent& evt);
    void MouseLeftUp(wxMouseEvent& evt);

    /**
        Helpers
    */
    bool SendClickEvent();
};

#endif

