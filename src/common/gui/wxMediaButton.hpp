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


/**
 * @brief wxMediaButton class
 * @details Base class for media buttons
 */
class wxMediaButton : public wxWindow
{
    protected:
    double scale = 0.8;


    public:
    /**
     * @brief Default ctor
     */
    wxMediaButton();


    /**
     * @brief Constructor, create and show the button
     * @details wxMediaButton constructor with same parameter as wxButton for compatibility reasons
     */
    wxMediaButton(wxWindow* parent, wxWindowID id,
                  const wxString& label = wxEmptyString,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxButtonNameStr);

    /**
     * @brief Destructor
     */
    ~wxMediaButton() {}


    /**
        Returns @true if an authentication needed symbol is displayed on the
        button.

        @remarks This method always returns @false
    */
    bool GetAuthNeeded() const { return false; }


    /**
     * @brief Default button size
     * @details Returns the default size for the buttons. It is advised to make all the
                dialog buttons of the same size and this function allows retrieving the
                (platform, and current font dependent) size which should be the best
                suited for this.

        The optional @a win argument is new since wxWidgets 3.1.3 and allows to
        get a per-monitor DPI specific size.
    */
    static wxSize GetDefaultSize(wxWindow* win = NULL) { return wxSize(50,50); }


    /**
     * @brief Draw the button
     */
    virtual void Draw(wxDC& dc);


    /**
        Events with pre- and post-handlers
    */
    virtual void OnPaintPreHandle(wxPaintEvent &evt) {}
    void OnPaint(wxPaintEvent& evt);
    virtual void OnPaintPostHandle(wxPaintEvent &evt) {}

    virtual void EnterWindowPreHandle(wxMouseEvent& evt) {}
    void EnterWindow(wxMouseEvent& evt);
    virtual void EnterWindowPostHandle(wxMouseEvent& evt) {}

    virtual void LeaveWindowPreHandle(wxMouseEvent& evt) {}
    void LeaveWindow(wxMouseEvent& evt);
    virtual void LeaveWindowPostHandle(wxMouseEvent& evt) {}

    virtual void MouseLeftUpPreHandle(wxMouseEvent& evt) {}
    void MouseLeftUp(wxMouseEvent& evt);
    virtual void MouseLeftUpPostHandle(wxMouseEvent& evt) {}

    /**
        Helpers
    */
    bool SendClickEvent();
};




/**
 * @brief wxMediaPlayPauseButton class
 * @details Draws and handles the PlayPause media button
 */
class wxMediaPlayPauseButton : public wxMediaButton
{
    private:
    /**
       Button phase, play=false, pause=true
    */
    bool phase;
    static const bool PAUSE = true;
    static const bool PLAY  = false;


    public:
    /**
      * @brief Default ctor
      */
    wxMediaPlayPauseButton();


    /**
     * @brief Constructor, creating and showing the button
     * @details Construct and draw the wxPlayPauseButton
                Same parameters as wxButton for compatibility reasons
     */
    wxMediaPlayPauseButton(wxWindow* parent, wxWindowID id,
                           const wxString& label = wxEmptyString,
                           const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxDefaultSize,
                           long style = 0,
                           const wxValidator& validator = wxDefaultValidator,
                           const wxString& name = wxButtonNameStr);


    /**
     * @brief Destructor
     */
    ~wxMediaPlayPauseButton() {}


    /**
     * @brief Returns the string label for the button. Either "Play" or "Pause"
     * @see SetLabel()
     */
    wxString GetLabel() const;


    /**
     * @brief Sets the string label for the button.
     * @param label The label to set.
     */
    void SetLabel(const wxString& label);


    /**
     * @brief Draw the button
     */
    void Draw(wxDC& dc);

    /**
     * @brief MouseLeftUp pre-handler
     */
    void MouseLeftUpPreHandle(wxMouseEvent &evt);
};




/**
 * @brief wxMediaStopButton class
 * @details Draws and handles the Stop media button
 */
class wxMediaStopButton : public wxMediaButton
{
    public:
    /**
      * @brief Default ctor
      */
    wxMediaStopButton();


    /**
     * @brief Constructor, creating and showing the button
     * @details Construct and draw the wxPlayPauseButton
                Same parameters as wxButton for compatibility reasons
     */
    wxMediaStopButton(wxWindow* parent, wxWindowID id,
                      const wxString& label = wxEmptyString,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxButtonNameStr);


    /**
     * @brief Destructor
     */
    ~wxMediaStopButton() {}


    /**
     * @brief Returns the string label for the button. Always returns "Stop"
     * @see SetLabel()
     */
    wxString GetLabel() const { return "Stop"; }


    /**
     * @brief Draw the button
     */
    void Draw(wxDC& dc);
};




/**
 * @brief wxMediaPowerButton class
 * @details Draws and handles the Power media button
 */
class wxMediaPowerButton : public wxMediaButton
{
    public:
    /**
      * @brief Default ctor
      */
    wxMediaPowerButton();


    /**
     * @brief Constructor, creating and showing the button
     * @details Construct and draw the wxPlayPauseButton
                Same parameters as wxButton for compatibility reasons
     */
    wxMediaPowerButton(wxWindow* parent, wxWindowID id,
                       const wxString& label = wxEmptyString,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = 0,
                       const wxValidator& validator = wxDefaultValidator,
                       const wxString& name = wxButtonNameStr);


    /**
     * @brief Destructor
     */
    ~wxMediaPowerButton() {}


    /**
     * @brief Returns the string label for the button. Always returns "Power"
     * @see SetLabel()
     */
    wxString GetLabel() const { return "Power"; }


    /**
     * @brief Draw the button
     */
    void Draw(wxDC& dc);
};

#endif

