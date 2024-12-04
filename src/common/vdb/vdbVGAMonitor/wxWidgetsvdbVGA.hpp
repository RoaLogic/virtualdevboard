/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets virtual Devboard VGA Monitor C++ header file      //
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

#ifndef WX_WIDGETS_VDB_VGA_HPP
#define WX_WIDGETS_VDB_VGA_HPP

#include <wx/wxprec.h>
#include <wx/wx.h>

#include "gui_interface.hpp"
#include "vdbVGAMonitor.hpp"
#include <semaphore>

wxDECLARE_EVENT(wxEVT_VGA, wxCommandEvent);

namespace RoaLogic {
    using namespace observer;
    using namespace vdb;
namespace GUI {

    /**
     * @class cVdbVGAMonitor
     * @author Bjorn Schouteten
     * @brief VGA virtual development board component
     * 
     * @details
     * This class communicates with the verilator component, any
     * class that wants to listen to it should register itself through
     * the subject-observer pattern.
     * 
     * It receives the vgaDataReady event and copies the data into the
     * _copyArray. By doing this the verilated context can continue and the 
     * GUI thread can do the work of placing the data into a wxImage and show
     * it on the screen. 
     * 
     * @attention The notify function runs in the verilated context, where
     * the onVGAEvent runs in the GUI context.
     */
    class cWXvdbVGAMonitor : public cGuiVDBComponent, private wxFrame
    {
        private:
        static const size_t _cDefaultWidth = 640;   //!< Default image width to start with
        static const size_t _cDefaultHeight = 480;  //!< Default image height to start with
        wxEvtHandler* _evtHandler;                  //!< The event handler of this frame
        wxStaticBitmap* _myStaticBitmap;            //!< Pointer to the handler for the bitmap
        wxImage _myImage;                           //!< Temporary image, used to create a new bitmap       
        std::binary_semaphore _copySemaphore;       //!< Semaphore to protect the copy array
        bool close = false;
        //!< Temporary array with the maximum size possible, used to copy data between threads.
        uRGBValue _copyArray[cVdbVGAMonitor::cMaxVerticalLines * cVdbVGAMonitor::cMaxHorizontalLines];

        void notify(eEvent aEvent, void* data);
        void onClose();

        void onVGAEvent(wxCommandEvent& event);
        void closeEvt(wxCloseEvent& event);

        public:
            cWXvdbVGAMonitor(cVDBCommon* myVDBComponent, distancePoint position, wxEvtHandler* myEvtHandler);
            ~cWXvdbVGAMonitor();
    };

}}

#endif
