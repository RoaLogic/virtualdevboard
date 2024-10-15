/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    DE10-Lite Verilator GUI main C++ file                        //
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

#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/splitter.h>

#include "de10LiteThread.hpp"

#include "vdbLED.hpp"

class cVirtualDemoBoard : public wxApp
{
    private:
    cDE10LiteThread* de10Thread;

    public:
    ~cVirtualDemoBoard();

    bool OnInit() override;
};

wxIMPLEMENT_APP(cVirtualDemoBoard);

class cMainFrame : public wxFrame
{
    private:
    static const int cLeftPanelOffset = 10;
    static const int cMinWidthSize    = 750;
    static const int cMinHeightSize   = 600;
    static const int cStartButtonID   = 100;
    static const int cStopButtonID    = 101;
    static const int cPauseButtonID   = 102;
    static const int cResumeButtonID  = 103;

    static const int cNumLeds = 8;
    
    wxSplitterWindow* _mySplitterWindow = new wxSplitterWindow{this, wxID_ANY};
    wxPanel* _leftPanel = new wxPanel{_mySplitterWindow, wxID_ANY};
    wxPanel* _rightPanel = new wxPanel{_mySplitterWindow, wxID_ANY};

    wxButton* _startButton;
    wxButton* _stopButton;
    wxButton* _pauseButton;
    wxButton* _resumeButton;

    //Led* testLed;
    std::vector<cVirtualLed*> ledInstances;

    wxDECLARE_EVENT_TABLE();

    public:
    wxMessageQueue<cDE10LiteThread::eDE10Message> _de10ThreadMessageQueue;

    cMainFrame();

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void onButtonStart(wxCommandEvent& event);
    void onButtonStop(wxCommandEvent& event);
    void onButtonPause(wxCommandEvent& event);
    void onButtonResume(wxCommandEvent& event);
};

wxBEGIN_EVENT_TABLE(cMainFrame, wxFrame)
    EVT_MENU(wxID_ABOUT, cMainFrame::OnAbout)
    EVT_MENU(wxID_EXIT, cMainFrame::OnExit)
    EVT_BUTTON(cStartButtonID, cMainFrame::onButtonStart)
    EVT_BUTTON(cStopButtonID, cMainFrame::onButtonStop)
    EVT_BUTTON(cPauseButtonID, cMainFrame::onButtonPause)
    EVT_BUTTON(cResumeButtonID, cMainFrame::onButtonResume)

wxEND_EVENT_TABLE()

bool cVirtualDemoBoard::OnInit()
{
    cMainFrame* frame = new cMainFrame();
    frame->Show(true);

    // Create the new thread and couple it with the main frame of the application
    de10Thread = new cDE10LiteThread(frame, frame->_de10ThreadMessageQueue);

    if(de10Thread->Create() != wxTHREAD_NO_ERROR)
    {
        wxMessageBox(wxT("Couldn't create the DE10 board"));
        return false;
    }

    if(de10Thread->Run() != wxTHREAD_NO_ERROR)
    {
        wxMessageBox(wxT("Couldn't run the DE10 board"));
        return false;
    }

    return true;
}

cVirtualDemoBoard::~cVirtualDemoBoard()
{
    de10Thread->Delete();
    de10Thread->Wait();
    delete de10Thread;
}

cMainFrame::cMainFrame() :
    wxFrame(nullptr, wxID_ANY, "Virtual DE10 demo board")
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
    _stopButton   = new wxButton(_leftPanel, cStopButtonID , wxT("Stop"));
    _pauseButton  = new wxButton(_leftPanel, cPauseButtonID, wxT("Pause"));
    _resumeButton = new wxButton(_leftPanel, cResumeButtonID, wxT("Resume"));

    leftPanelSizer->Add(_startButton,  0, wxLEFT, cLeftPanelOffset);
    leftPanelSizer->Add(_stopButton,   0, wxLEFT, cLeftPanelOffset);
    leftPanelSizer->Add(_pauseButton,  0, wxLEFT, cLeftPanelOffset);
    leftPanelSizer->Add(_resumeButton, 0, wxLEFT, cLeftPanelOffset);

    _leftPanel->SetSizer(leftPanelSizer);


    /*********************************
     * Right panel
     ********************************/

    wxBoxSizer* rightPanelSizer = new wxBoxSizer(wxHORIZONTAL);

    for (size_t i = 0; i < cNumLeds; i++)
    {
        cVirtualLed* newLed = new cVirtualLed(_mySplitterWindow, i, _rightPanel, wxPoint( (50*i) , 0), 50 , 'g');
        ledInstances.push_back(newLed);
        rightPanelSizer->Add(newLed, 0, wxLEFT, 100);
    }

    /*********************************
     * Main screen setup
     ********************************/

    _mySplitterWindow->SplitVertically(_leftPanel, _rightPanel, 120);
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(_mySplitterWindow, 1, wxEXPAND);
    SetSizerAndFit(topSizer);
    SetMinSize(wxSize(cMinWidthSize, cMinHeightSize));
}

void cMainFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void cMainFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("DE10 lite virtual demo board", "DE10 lite", wxOK | wxICON_INFORMATION);
}

void cMainFrame::onButtonStart(wxCommandEvent& event)
{
    cDE10LiteThread::eDE10Message msg = cDE10LiteThread::eDE10Message::Start;
    _de10ThreadMessageQueue.Post(msg);
}

void cMainFrame::onButtonStop(wxCommandEvent& event)
{
    cDE10LiteThread::eDE10Message msg = cDE10LiteThread::eDE10Message::Stop;
    _de10ThreadMessageQueue.Post(msg);
}

void cMainFrame::onButtonPause(wxCommandEvent& event)
{
    cDE10LiteThread::eDE10Message msg = cDE10LiteThread::eDE10Message::Pause;
    _de10ThreadMessageQueue.Post(msg);
}

void cMainFrame::onButtonResume(wxCommandEvent& event)
{
    cDE10LiteThread::eDE10Message msg = cDE10LiteThread::eDE10Message::Resume;
    _de10ThreadMessageQueue.Post(msg);
}
