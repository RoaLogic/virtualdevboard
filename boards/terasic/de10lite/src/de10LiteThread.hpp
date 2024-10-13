/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    DE10-Lite Verilator Thread C++ header file                   //
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
#include <wx/msgqueue.h>

#include "de10lite.hpp"

#ifndef DE10LITE_THREAD_HPP
#define DE10LITE_THREAD_HPP

wxDECLARE_EVENT(wxEVT_DE10_Start, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_DE10_Stop, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_DE10_Pause, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_DE10_Resume, wxCommandEvent);

/**
 * @class cDE10LiteThread
 * @author Bjorn Schouteten
 * @brief Thread implementation for the de10Lite class
 * @version 0.1
 * @date 13-oct-2024
 *
 * @details This class is a threaded implemenation for the de10Lite wrapper class.
 * 
 * It inherits the cDE10Lite class, which is used in the command line implemenation.
 * All functions shall be inherited, it will only override the run() function. It is now
 * controlable from the GUI to start, stop or continue the verilated design.
 * 
 * @attention Make sure that the verilated instance is running in a seperate thread,
 *            class internally this is properly handled.
 */
class cDE10LiteThread : public wxThread, public cDE10Lite
{
    public:
    enum eDE10Message
    {
        Start,
        Stop,
        Pause,
        Resume,
        Reset
    };

    private:
    wxEvtHandler* _parent;
    wxMessageQueue<eDE10Message>& _eventQueue;
    int _logLevel = 2;

    enum eDE10State
    {
        running,
        paused,
        stopped,
    }_myState;

    public:
    cDE10LiteThread(wxEvtHandler* parent, wxMessageQueue<eDE10Message>& eventQueue);
    ~cDE10LiteThread();

    virtual void* Entry() override;
    virtual void OnExit() override;

	void processEventQueue();
    int run(size_t numClocks);
};

#endif // DE10LITE_THREAD_HPP