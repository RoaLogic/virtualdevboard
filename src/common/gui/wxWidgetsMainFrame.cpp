/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets main frame implementation                         //
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

#include "wxWidgetsMainFrame.hpp"

wxDEFINE_EVENT(wxEVT_STATUS, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_ADD_LED, wxCommandEvent);

cMainFrame::cMainFrame(cSubject* aSubject) :
    wxFrame(nullptr, wxID_ANY, "Virtual DE10 demo board"),
    _subject(aSubject)
{
    /**********************************
     * Setup the top menu and the status bar
     *********************************/
    wxMenu *menuFile = new wxMenu;

    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&help");
    Bind(wxEVT_MENU, &cMainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &cMainFrame::OnAbout, this, wxID_ABOUT);

    SetMenuBar(menuBar);

    CreateStatusBar();

    /*********************************
     * Setup left panel of the system
     ********************************/

    wxBoxSizer* leftPanelSizer = new wxBoxSizer(wxVERTICAL);
    leftPanelSizer->Add(new wxStaticText(_leftPanel, wxID_STATIC, wxT("Configuration"), 
                                         wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT),
                                         0, wxLEFT, cLeftPanelOffset);
    
    _startButton  = new wxButton(_leftPanel, cStartButtonID, wxT("Start"));
    _startButton->Bind(wxEVT_BUTTON, &cMainFrame::onButtonStart, this);
    // _stopButton   = new wxButton(_leftPanel, wxID_ANY , wxT("Stop"));
    // _pauseButton  = new wxButton(_leftPanel, wxID_ANY, wxT("Pause"));
    // //_resumeButton = new wxButton(_leftPanel, cResumeButtonID, wxT("Resume"));

    leftPanelSizer->Add(_startButton,  0, wxLEFT, cLeftPanelOffset);
    // leftPanelSizer->Add(_stopButton,   0, wxLEFT, cLeftPanelOffset);
    // leftPanelSizer->Add(_pauseButton,  0, wxLEFT, cLeftPanelOffset);
    // //leftPanelSizer->Add(_resumeButton, 0, wxLEFT, cLeftPanelOffset);

    // // Bind the functions for starting and pausing verilator to the verilator thread
    // // Both the Start and resume button have the same functionality
    // function<void (wxCommandEvent&)> startHandler(bind(&cDE10LiteThread::OnStart, de10Thread, std::placeholders::_1));
    // _startButton->Bind(wxEVT_BUTTON, startHandler, wxID_ANY, wxID_ANY, de10Thread);

    // // function<void (wxCommandEvent&)> resumeHandler(bind(&cDE10LiteThread::OnStart, &(*de10Thread), std::placeholders::_1));
    // // _resumeButton->Bind(wxEVT_BUTTON, resumeHandler);

    // function<void (wxCommandEvent&)> pauseHandler(bind(&cDE10LiteThread::OnPauze, de10Thread, std::placeholders::_1));
    // _pauseButton->Bind(wxEVT_BUTTON, pauseHandler, wxID_ANY, wxID_ANY, de10Thread);

    _leftPanel->SetSizer(leftPanelSizer);


    /*********************************
     * Right panel
     ********************************/

    /*********************************
     * Main screen setup
     ********************************/
    _mySplitterWindow->SplitVertically(_leftPanel, _rightPanel, 120);
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(_mySplitterWindow, 1, wxEXPAND);
    SetSizerAndFit(topSizer);
    SetMinSize(wxSize(cMinWidthSize, cMinHeightSize));

    //Bind(wxEVT_STATUS, &cMainFrame::onStatusChange, this, wxID_ANY);
    Bind(wxEVT_ADD_LED, &cMainFrame::onAddLed, this, wxID_ANY);
}

void cMainFrame::OnExit(wxCommandEvent& event)
{
    _subject->notifyObserver(eEvent::close);
    Close(true);
}

void cMainFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("DE10 lite virtual demo board", "DE10 lite", wxOK | wxICON_INFORMATION);
}

void cMainFrame::onButtonStart(wxCommandEvent& event)
{
    _subject->notifyObserver(eEvent::stateChange);

    if(_startButton->GetLabel() == wxT("Start"))
    {
        _startButton->SetLabel("Pause");
    }
    else if(_startButton->GetLabel() == wxT("Pause"))
    {
        _startButton->SetLabel("Resume");
    }
    else if(_startButton->GetLabel() == wxT("Resume"))
    {
        _startButton->SetLabel("Pause");
    }
}

void cMainFrame::onAddLed(wxCommandEvent& event)
{
    sAddLedEvent* eventData = reinterpret_cast<sAddLedEvent*>(event.GetClientObject());

    if(eventData)
    {
        wxBoxSizer* rightPanelSizer = new wxBoxSizer(wxHORIZONTAL);

        for (size_t i = 0; i < eventData->numLeds; i++)
        {
            cVirtualLed* newLed = new cVirtualLed(_mySplitterWindow, i, _rightPanel, wxPoint( (50*i) , 0), 50 , 'g');
            ledInstances.push_back(newLed);
            rightPanelSizer->Add(newLed, 0, wxLEFT, 100);
            _mySplitterWindow->SplitVertically(_leftPanel, _rightPanel, 120);
        }

        Refresh();
    }
}

// void cMainFrame::onStatusChange(wxCommandEvent& event)
// {
//     sSystemStateEvent* eventData = reinterpret_cast<sSystemStateEvent*>(event.GetClientObject());
    
//     if(eventData->state == eSystemState::running)
//     {
//         SetStatusText("Started verilator");
//         _startButton->SetLabel("Pause");
//     }
// }