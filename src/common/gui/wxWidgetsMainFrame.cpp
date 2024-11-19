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
#include "wxWidgetsvdbVGA.hpp"
#include "wxWidgetsVdbLED.hpp"
#include "wxWidgetsVdb7SegmentDisplay.hpp"


wxDEFINE_EVENT(wxEVT_STATUS, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_ADD_VDB, wxCommandEvent);

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
    
    _startButton  = new wxMediaPlayPauseButton(_leftPanel, cStartButtonID, wxT("Play"));
    _startButton->Bind(wxEVT_BUTTON, &cMainFrame::onButtonStart, this);
    _stopButton  = new wxMediaStopButton(_leftPanel, cStopButtonID, wxT("Stop"));
    _stopButton->Bind(wxEVT_BUTTON, &cMainFrame::onButtonStop, this);
    _resetButton   = new wxMediaResetButton(_leftPanel, cResetButtonID , wxT("Reset"));
    _resetButton->Bind(wxEVT_BUTTON, &cMainFrame::onButtonReset, this);

    leftPanelSizer->Add(_startButton, 0, wxALIGN_CENTER, cLeftPanelOffset);
    leftPanelSizer->Add(_resetButton, 0, wxALIGN_CENTER, cLeftPanelOffset);
    leftPanelSizer->Add(_stopButton,  0, wxALIGN_CENTER, cLeftPanelOffset);
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

    Bind(wxEVT_ADD_VDB, &cMainFrame::onAddVdb, this, wxID_ANY);
}

cMainFrame::~cMainFrame()
{
    for(cGuiVDBComponent* vdb : vdbInstances)
    {
        // Remove observer at this point, so we don't receive any events anymore
        // The components are deleted in a different constructor
        vdb->removeObserver();
        vdb->onClose();
    }
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
}

void cMainFrame::onButtonReset(wxCommandEvent& event)
{
    _subject->notifyObserver(eEvent::reset);
}

void cMainFrame::onButtonStop(wxCommandEvent& event)
{
    _startButton->SetLabel("Start");

    for(cGuiVDBComponent* vdb : vdbInstances)
    {
        // Remove observer at this point, so we don't receive any events anymore
        // The components are deleted in a different constructor
        vdb->removeObserver();
        vdb->onClose();
    }

    _rightPanel->DestroyChildren();

    vdbInstances.erase(vdbInstances.begin(), vdbInstances.end());

    _subject->notifyObserver(eEvent::stop);
}

void cMainFrame::onAddVdb(wxCommandEvent& event)
{
    static uint8_t offset = 0;
    sAddVdbComponent* eventData = reinterpret_cast<sAddVdbComponent*>(event.GetClientObject());

    if(eventData)
    {
        switch (eventData->type)
        {
            case eVdbComponentType::vdbLed :
            {
                cWXVdbLed* newLED = new cWXVdbLed(eventData->vdbComponent, offset, this, _rightPanel, 
                                               wxPoint( (50*offset) , 0), 50 , eventData->information);
                vdbInstances.push_back(newLED);

                offset++;
                break;
            }
            case eVdbComponentType::vdb7SegmentDisplay :
            {
                cWXVdb7SegmentDisplay* new7SegmentDisplay = new cWXVdb7SegmentDisplay(eventData->vdbComponent, offset, this, _rightPanel,
                                                                      wxPoint( (300*offset) , 100), 50 , eventData->information);
                vdbInstances.push_back(new7SegmentDisplay);

                offset++;
                break;
            }
            case eVdbComponentType::vdbVGA :
            {
                cWXvdbVGAMonitor* newVGA = new cWXvdbVGAMonitor(eventData->vdbComponent, 0, this);
                vdbInstances.push_back(newVGA);
                break;
            }

        default:
            ERROR << "Unknown vdb component type registered \n";
            break;
        }
    }
    else
    {
        ERROR << "Failed to read event data when adding vdb component \n";
    }    
}
