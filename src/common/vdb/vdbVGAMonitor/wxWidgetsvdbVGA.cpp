/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets virtual Devboard VGA Monitor C++ source file      //
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

#include "wxWidgetsvdbVGA.hpp"

// Define the wxEVT_VGA, which is special within this class
wxDEFINE_EVENT(wxEVT_VGA, wxCommandEvent);

namespace RoaLogic {
    using namespace observer;
namespace GUI {

    /**
     * @brief Construct a new wx widgets VGA monitor frame
     * @details 
     * In this function we create the class and create all 
     * the components, set it and show the frame. Also the
     * event is bind so that we can handle any incoming event 
     * from the verilated context.
     * 
     * Also the default image is filled with a predefined message,
     * which is passed to the monitor and shown.
     */
    cWXvdbVGAMonitor::cWXvdbVGAMonitor(cVDBCommon* myVDBComponent, wxEvtHandler* myEvtHandler) :
        wxFrame(NULL, wxID_ANY, wxT("VGA monitor")),
        cGuiVDBComponent(myVDBComponent),
        _evtHandler(myEvtHandler),
        _myImage(640, 480, false),
        _copySemaphore(1)
    {
        for (size_t i = 0; i < 640; i++)
        {
            for (size_t y = 0; y < 480; y++)
            {
                if(i <= 91)
                {
                    _myImage.SetRGB(i, y, 113, 112, 115);
                }
                else if(i > 91 && i <= 182)
                {
                    _myImage.SetRGB(i, y, 224, 221, 112);
                }
                else if(i > 182 && i <= 273)
                {
                    _myImage.SetRGB(i, y, 118, 228, 232);
                }
                else if(i > 273 && i <= 364)
                {
                    _myImage.SetRGB(i, y, 55, 250, 97);
                }
                else if(i > 364 && i <= 455)
                {
                    _myImage.SetRGB(i, y, 204, 71, 237);
                }
                else if(i > 455 && i <= 546)
                {
                    _myImage.SetRGB(i, y, 204, 14, 52);
                }
                else
                {
                    _myImage.SetRGB(i, y, 49, 14, 204);
                }
            }
        }
        _myStaticBitmap = new wxStaticBitmap(this, wxID_ANY, _myImage, wxDefaultPosition, wxDefaultSize);

        wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
        topSizer->Add(_myStaticBitmap, 1, wxEXPAND);
        SetSizerAndFit(topSizer);

        Show(true);

        // Use a specific Bind due to bug inside of wxWidgets, see 
        // https://stackoverflow.com/questions/38833116/conversion-in-derived-class-inaccessible-if-base-class-is-protected
        myEvtHandler->Bind(wxEVT_VGA, std::bind(&cWXvdbVGAMonitor::onVGAEvent, this, std::placeholders::_1));
        //myEvtHandler->Bind(wxEVT_VGA, &cWXvdbVGAMonitor::onVGAEvent, this, wxID_ANY);
    }

    /**
     * @brief destructor
     */
    cWXvdbVGAMonitor::~cWXvdbVGAMonitor()
    {
        delete _myStaticBitmap;
    }

    /**
     * @brief notify function from the vdb component
     * @details This function receives events from the component
     * it is registered to. This shall be the vdbVGAMonitor and the 
     * received events are vgaData and vgaDataReady. 
     * 
     * When the horizontalLines and verticalLines are 0 the image size
     * is still unknown
     * 
     * First check that the image is still the same as the data received,
     * if this is not the case, resize the image. Following this add the
     * data into the image. 
     * 
     * When the data is complete, sent the wxVGA_EVT.
     * 
     * @note this function runs in the verilated context.
     * 
     * @todo: Check that the received data fits in the frame and 
     *        that no invalid data is received.
     */
    void cWXvdbVGAMonitor::notify(eEvent aEvent, void* data)
    {
        sVgaData* eventData = reinterpret_cast<sVgaData*>(data);

        if(aEvent == eEvent::vgaData || aEvent == eEvent::vgaDataReady)
        {
            if(eventData->horizontalLines != 0 && eventData->verticalLines != 0)
            {
                // Check if image is the same as current image, else resize the image
                if( (_myImage.GetWidth()  != eventData->horizontalLines) || 
                    (_myImage.GetHeight() != eventData->verticalLines))
                {
                    _myImage.Resize(wxSize(eventData->horizontalLines, eventData->verticalLines), wxDefaultPosition);
                }

                _copySemaphore.acquire();
                for (size_t y = 0; y < (_myImage.GetWidth() * _myImage.GetHeight()); y++)
                {
                    _copyArray[y] = eventData->dataArray[y];
                }
                _copySemaphore.release();

                if(aEvent == eEvent::vgaDataReady)
                {
                    wxCommandEvent vgaEvent{wxEVT_VGA};
                    wxPostEvent(_evtHandler, vgaEvent);
                }
            }
        }
    }

    /**
     * @brief Handle the VGA event
     * @details This function handles the wxEVT_VGA event
     * 
     * The event is sent when we need to update the image,
     * data is already set, so only set the new bitmap image.
     * 
     * @note This function runs in the GUI thread
     * 
     */
    void cWXvdbVGAMonitor::onVGAEvent(wxCommandEvent& event)
    {
        size_t imageWidth = _myImage.GetWidth();
        size_t currentOffset = 0;

        _copySemaphore.acquire();
        for (size_t y = 0; y < _myImage.GetHeight(); y++)
        {
            for (size_t x = 0; x < _myImage.GetWidth(); x++)
            {
                _myImage.SetRGB(x, y, _copyArray[currentOffset].red, _copyArray[currentOffset].green, _copyArray[currentOffset].blue);
                currentOffset++;
            }
        }
        _copySemaphore.release();

        _myStaticBitmap->SetBitmap(_myImage);
    }

}}