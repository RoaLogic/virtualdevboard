/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets main frame definition                             //
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

#ifndef WXWIDGETS_MAIN_FRAME_HPP
#define WXWIDGETS_MAIN_FRAME_HPP

#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/window.h>
#include "wxMediaButton.hpp"

#include "eventDefinition.hpp"
#include "subject.hpp"
#include "gui_interface.hpp"

using namespace RoaLogic::observer;
using namespace RoaLogic::GUI;
using namespace RoaLogic::vdb;
using namespace RoaLogic::dimensions;

wxDECLARE_EVENT(wxEVT_CHANGE_FRAME, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_ADD_VDB, wxCommandEvent);

struct sChangeFrameData : public wxClientData
{
    std::string applicationName;
    std::string aboutTitle;
    std::string aboutText;
    distanceSize minimalScreenSize;
    sRGBColor backgroundColor;
};

struct sAddVdbComponent : public wxClientData
{
    eVdbComponentType type;
    cVDBCommon* vdbComponent;
    distancePoint placement;
    void* componentDetails;
    double angle;
};

/**
 * @class cMainFrame
 * @author Bjorn Schouteten
 * @brief Main frame
 * @version 0.1
 * @date 19-okt-2024
 *
 * @details This class is the definition of the main frame for the virtual demo board
 * 
 * This class sets and controls the main frame. From the main frame events are handled
 * to any interested observer, however the subject itself is the virtual demo board.
 * This class shall only handle events coming through the wxWidgets event handling.
 * 
 */
class cMainFrame : public wxFrame
{
    private:
    static const int cLeftPanelWidth  = 120;
    static const int cLeftPanelOffset = 10;
    static const int cMinWidthSize    = 750;
    static const int cMinHeightSize   = 600;
    static const int cStartButtonID   = 100;
    static const int cResetButtonID   = 101;
    static const int cStopButtonID    = 102;
    
    std::string _myApplicationName = "Virtual development board";
    std::string _myAboutText = "Virtual development board";
    std::string _myAboutTitle = "Virtual development board";

    int _myBoardWidth = 0;
    int _myBoardHeight = 0;

    cSubject* _subject;

    wxSplitterWindow* _mySplitterWindow = new wxSplitterWindow{this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxEXPAND};
    wxPanel* _leftPanel = new wxPanel{_mySplitterWindow, wxID_ANY};
    wxPanel* _rightPanel = new wxPanel{_mySplitterWindow, wxID_ANY};

    wxMediaPlayPauseButton* _startButton;
    wxMediaStopButton* _stopButton;
    wxMediaResetButton* _resetButton;

    std::vector<cGuiVDBComponent*> vdbInstances;

    public:
    cMainFrame(cSubject* aSubject);
    ~cMainFrame();

    void onChangeFrame(wxCommandEvent& event);
    void onSize(wxSizeEvent& event);

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void onButtonStart(wxCommandEvent& event);
    void onButtonReset(wxCommandEvent& event);
    void onButtonStop(wxCommandEvent& event);

    void onAddVdb(wxCommandEvent& event);
};


#endif
