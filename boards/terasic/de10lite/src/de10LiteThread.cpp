/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    DE10-Lite Verilator Thread C++ source file                   //
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

#include "de10LiteThread.hpp"
#include <log.hpp>

using namespace RoaLogic;
using namespace common;

cDE10LiteThread::cDE10LiteThread(wxEvtHandler* parent, wxMessageQueue<eDE10Message>& eventQueue) :
    wxThread(wxTHREAD_JOINABLE),
    cDE10Lite(std::unique_ptr<VerilatedContext>(new VerilatedContext).get(), false),
    _parent(parent),
    _eventQueue(eventQueue)
{
    
}

cDE10LiteThread::~cDE10LiteThread()
{
    Exit();
}

/**
 * @brief This is the thread entry function
 * 
 * @return void* 
 */
void* cDE10LiteThread::Entry()
{
    sCoRoutineHandler reset = cDE10Lite::Reset();
    bool started = false;

    cLog::getInstance()->init(_logLevel, "");
    INFO << "Started log with level: " << _logLevel << "\n";

    // Thread keeps running untill Delete() is called
    // When Delete is called(), the OnExit function is called
    while (!TestDestroy())
    {  
        processEventQueue();

        if(_myState == eDE10State::running)
        {
            run(1000);
            wxLogStatus("Verilator started");
        }
        else
        {
            wxLogStatus("Verilator stopped");
        }
    }

    //close log
    cLog::getInstance()->close();

    return 0;
}

int cDE10LiteThread::run(size_t numClocks)
{
    size_t tickCount = 0;

    while (tickCount++ < numClocks)
    {
        tick();
    }

    if(finished())
    {
        _myState = eDE10State::stopped;
        INFO << "Simulation ended\n";
    }

    return 0;
}

void cDE10LiteThread::OnExit()
{
    finish();
}

void cDE10LiteThread::processEventQueue()
{
    eDE10Message message;

    // Check to see if we have an event
    if (_eventQueue.ReceiveTimeout(0, message) == wxMSGQUEUE_NO_ERROR )
    {
        switch (message)
        {
        case eDE10Message::Resume:
            [[fallthrough]]
        case eDE10Message::Start:
            _myState = eDE10State::running;
            break;

        case eDE10Message::Stop:
            _myState = eDE10State::stopped;
            break;

        case eDE10Message::Pause:
            _myState = eDE10State::paused;
            break;

        case eDE10Message::Reset:
            
            break;

        default:
            INFO << "Unknown event message received \n";
            break;
        }
    }
}
