/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets GUI definition                                    //
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

#ifndef WXWIDGETS_GUI_HPP
#define WXWIDGETS_GUI_HPP

#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/splitter.h>

#include "gui_interface.hpp"
#include "wxWidgetsMainFrame.hpp"

using namespace RoaLogic::observer;
using namespace RoaLogic::GUI;

/**
 * @class cVirtualDemoBoard
 * @author Bjorn Schouteten
 * @brief WX widgets virtual demo board implementation
 * @version 0.1
 * @date 19-okt-2024
 *
 * @details This class is a GUI implementation for the virtual demo board
 * 
 * It is specificly coupled with wxWidgets and shall only use wxWidget elements.
 * 
 * to communicate with the verilated design it inherits from the cGuiInterface, 
 * since this interface is a subject we can notify events. 
 * 
 * @attention The virtual functions coming from the cGuiInterface are all run
 * within the verilator context, so we must make sure that we don't change any 
 * GUI elements.
 * 
 */
class cVirtualDemoBoard : public wxApp, public cGuiInterface
{
    private:
    bool OnInit() override;
    cMainFrame* _mainFrame;

    public:
    cVirtualDemoBoard();
    ~cVirtualDemoBoard();
    void init(int argc, char** argv);

    void addVirtualLED(size_t numLeds);
    //void setCurrentStatus(eSystemState state);
};



#endif