/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets GUI implementation                                //
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

#include "wxWidgetsImplementation.hpp"
#include "wx/display.h"

DECLARE_APP(cVirtualDemoBoard)
IMPLEMENT_APP_NO_MAIN(cVirtualDemoBoard)

cVirtualDemoBoard::cVirtualDemoBoard()
{
    wxApp::SetInstance(this);
}

cVirtualDemoBoard::~cVirtualDemoBoard()
{
    notifyObserver(eEvent::close);
}

bool cVirtualDemoBoard::OnInit()
{
    // for ( unsigned int i = 0; i < wxDisplay::GetCount(); ++i )
    // {
    //     const wxDisplay display(i);
    //     INFO << "Display " << i << " PPI: " << display.GetPPI().x << "x" << display.GetPPI().y << ", scale factor: " << display.GetScaleFactor() << "\n";
    // }

    _mainFrame = new cMainFrame(this);
    _mainFrame->Show(true);

    return true;
}

void cVirtualDemoBoard::init(int argc, char** argv)
{
    wxEntry(argc, argv);
}

void cVirtualDemoBoard::setupGui(std::string applicationName, std::string aboutTitle, std::string aboutText, distanceSize minimalScreenSize, sRGBColor backgroundColor)
{
    wxCommandEvent changeFrameEvent{wxEVT_CHANGE_FRAME};
    sChangeFrameData* const changeFrameData{ new sChangeFrameData};

    changeFrameData->applicationName = applicationName;
    changeFrameData->aboutTitle = aboutTitle;
    changeFrameData->aboutText = aboutText;
    changeFrameData->minimalScreenSize = minimalScreenSize;
    changeFrameData->backgroundColor = backgroundColor;

    changeFrameEvent.SetClientObject(changeFrameData);
    wxPostEvent(_mainFrame, changeFrameEvent);
}

void cVirtualDemoBoard::addVdbComponent(eVdbComponentType type, cVDBCommon* vdbComponent, distancePoint point, void* information, double angle)
{
    wxCommandEvent statusEvent{wxEVT_ADD_VDB};
    sAddVdbComponent* const eventData{ new sAddVdbComponent};

    eventData->type = type;
    eventData->vdbComponent = vdbComponent;
    eventData->placement = point;
    eventData->componentDetails = information;
    eventData->angle = angle;

    statusEvent.SetClientObject(eventData);
    wxPostEvent(_mainFrame, statusEvent);
}
