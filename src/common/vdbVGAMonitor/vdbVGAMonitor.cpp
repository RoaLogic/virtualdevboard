/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Virtual Devboard VGA Monitor Verilator C++ wrapper           //
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

#include "vdbVGAMonitor.hpp"

#define DBG_VDB_VGA

/**
 * The following structure is build to compress the full VGA timing per VESA specs.
 * See the system verilog file for the values.
 */
struct sVGATiming
{
    uint16_t horizontalPixels;
    uint16_t verticalPixels;
    uint8_t frequencyHz;
    long double pixelClock;
    uint16_t totalHorizontal; // Active video + front porch + sync + back porch
    svBitVecVal  frontPorchHorizontal;
    svBitVecVal  syncHorizontal;
    svBitVecVal  backPorchHorizontal;
    uint16_t totalVertical; // Active video + front porch + sync + back porch
    svBitVecVal  frontPorchVertical;
    svBitVecVal  syncVertical;
    svBitVecVal  backPorchVertical;
};

static const sVGATiming cVGATiming[] =
{
    { 640, 480, 60, 25.175_MHz,  800, 16,  96,  48, 524, 11, 2, 31},
    { 640, 480, 72, 31.500_MHz,  832, 24,  40, 128, 520,  9, 3, 28},
    { 640, 480, 75, 31.500_MHz,  800, 16,  96,  48, 524, 11, 2, 32},
    { 640, 480, 85, 36.000_MHz,  832, 32,  48, 112, 509,  1, 3, 25},
    { 800, 600, 56, 38.100_MHz, 1088, 32, 128, 128, 619,  1, 4, 14},
    { 800, 600, 60, 40.000_MHz, 1056, 40, 128,  88, 628,  1, 4, 23},
    { 800, 600, 72, 50.000_MHz,  932, 56, 120,  64, 666, 37, 6, 23},
    { 800, 600, 75, 49.500_MHz, 1056, 16,  80, 160, 624,  1, 2, 21},
    { 800, 600, 85, 56.250_MHz, 1048, 32,  64, 152, 632,  1, 3, 27},
    {1024, 768, 60, 65.000_MHz, 1344, 24, 136, 160, 806,  3, 6, 29},
    {1024, 768, 70, 75.000_MHz, 1328, 24, 136, 144, 804,  1, 6, 29},
    {1024, 768, 75, 78.750_MHz, 1312, 16,  96, 176, 800,  1, 3, 28},
    {1024, 768, 85, 94.500_MHz, 1376, 48,  96, 208, 808,  1, 3, 36}
};

static const size_t cVGATimingSize = sizeof(cVGATiming) / sizeof(cVGATiming[0]);


// // Define the wxEVT_LED, which is special within this class
// wxDEFINE_EVENT(wxEVT_VGA, wxCommandEvent);

// wxEvtHandler* cVdbVGA::_parent; // The parent handler for the VGA events
// Internal reference for mapping ID's to cVdbVGA instances
std::vector<cVdbVGA::sVdbVGAMap> cVdbVGA::_referencePointers; 

/**
 * @brief VGA Monitor HSYNC DPI-C callback
 * @details This function gets called when the VGA Monitor detects an HSYNC
 * 
 */
void vdbVGAMonitorHSYNC(int id)
{
    cVdbVGA::processVGAEvent(svGetScope(), eVgaEvent::hsync);
}

/**
 * @brief VGA Monitor VSYNC DPI-C callback
 * @details This function gets called when the VGA Monitor detects a VSYNC
 * 
 */
void vdbVGAMonitorVSYNC(int id)
{
    cVdbVGA::processVGAEvent(svGetScope(), eVgaEvent::vsync);
}

/**
 * @brief Register a new virtual VGA class 
 * @details This function registers a new virtual VGA class with the static modules.
 * 
 * It stores the reference information into the vector and stores the global parent variabele
 * 
 * @param[in] reference       The reference information to store
 */
void cVdbVGA::registerVirtualVGA(sVdbVGAMap reference)
{
    _referencePointers.push_back(reference);
}

/**
 * @brief Process a VGA event from the verilated class
 * @details This function processes a VGA event from the verilated context.
 * 
 * It checks in the reference pointers if it finds the corresponding ID. When
 * found it will call the corresponding function of the event.
 * 
 * @param[in] vgaID       The VGA ID which had the event
 * @param[in] event       The VGA event which occured
 */
void cVdbVGA::processVGAEvent(svScope scope, eVgaEvent event)
{
    bool found = false;

    for (const sVdbVGAMap& ref : _referencePointers)
    {
        if(ref.scope == scope)
        {
            if(event == eVgaEvent::vsync)
            {
                ref.reference->handleVsync();
            }
            else if(event == eVgaEvent::hsync)
            {
                ref.reference->handleHsync();
            }
            found = true;
            break;
        }
    }

    if(!found)
    {
        WARNING << "VGA: Event on non found module: " << svGetNameFromScope(scope) << " \n";
    }
}

/**
 * @brief Send a LED event in the verilator thread
 * @details Send a event from any LED
 * 
 * Within this function we create a event which is sent to the GUI
 * thread. It holds the LED ID and the state of the LED. Those are
 * packed in a sEventData structure and then posted to the event
 * handling system
 * 
 * @attention This function runs in the verilator thread context
 * 
 * @param[in] vgaID     The ID of the VGA which had the event
 * @param[in] state     The new state of the LED
 */
// void cVdbVGA::SendVGAEvent(int vgaID, bool vsync)
// {
//     wxCommandEvent testEvent{wxEVT_VGA};
//     sEventData* const eventData{ new sEventData};

//     if(_parent)
//     {
//         eventData->id = vgaID;
//         eventData->vsync = vsync;

//         #ifdef DBG_VDB_VGA
//         INFO << "Received VGA " << eventData->id << " vsync: " << eventData->vsync << " Thread: " <<  wxThread::GetCurrentId() << "\n";
//         #endif

//         testEvent.SetClientObject(eventData);
//         wxPostEvent(_parent, testEvent);
//     }
// }

// /**
//  * @brief Handle a led event in the GUI thread
//  * @details 
//  * Within this function a LED event from the system is handled. 
//  * 
//  * @attention This function runs in the GUI thread context
//  * 
//  * @param[in] event     The event which happend
//  */
// void cVdbVGA::OnVGAEvent(wxCommandEvent& event)
// {
//     sEventData* eventData = reinterpret_cast<sEventData*>(event.GetClientObject());

//     for (const sVirtualVGAMap& ref : _referencePointers)
//     {
//         if(ref.ID == eventData->id)
//         {
//             if(eventData->vsync)
//             {
//                 ref.ledReference->handleVsync();
//             }
//             else
//             {
//                 ref.ledReference->handleHsync();
//             }

//             #ifdef DBG_VDB_VGA
//             INFO << "Processed VGA " << eventData->id << " vsync: " << eventData->vsync << " Thread: " << wxThread::GetCurrentId() <<"\n";
//             #endif
//         }
//     }
// }

// cVdbVGA::cVdbVGA(std::string scopeName, wxEvtHandler* aParent, int id) :
//     wxFrame(nullptr, wxID_ANY, "Virtual VGA monitor")
// {
//     registerVirtualVGA(sVirtualVGAMap{id, this},  aParent);
//     aParent->Bind(wxEVT_VGA, cVdbVGA::OnVGAEvent);

//     const svScope scope = svGetScopeFromName(scopeName.c_str());
//     svSetScope(scope);

//     newImage = wxImage(640,640, false);

//     for (size_t i = 0; i < 640; i++)
//     {
//         for (size_t y = 0; y < 640; y++)
//         {
//             newImage.SetRGB(i, y, 10, 200, 0);
//         }
//     }

//     wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
//     _myStaticBitmap = new wxStaticBitmap(this, wxID_ANY, wxBitmap(newImage), wxDefaultPosition, wxSize(640,640));
    
//     topSizer->Add(_myStaticBitmap, 0, wxEXPAND);

//     SetSizerAndFit(topSizer);
//     Show(true);
// }

cVdbVGA::cVdbVGA(std::string scopeName, cTimeInterface* timeInterface, cClock* pixelClock) :
    _timeInterface(timeInterface),
    _pixelClock(pixelClock)
{
    assert(pixelClock != nullptr);

    _myScope = svGetScopeFromName(scopeName.c_str());
    svSetScope(_myScope);

    registerVirtualVGA(sVdbVGAMap{_myScope, this});
}

cVdbVGA::~cVdbVGA()
{

}

void cVdbVGA::show(bool show)
{
    //Show(show);
}

void cVdbVGA::handleHsync()
{
    _numHsync++;
}

void cVdbVGA::handleVsync()
{
    if(_timeInterface)
    {
        simtime_t currentVSyncTime = _timeInterface->getTime();
        _timeBetweenVsync = currentVSyncTime - _previousVSyncTime;
        _previousVSyncTime = currentVSyncTime;

        #ifdef DBG_VDB_VGA
        INFO << "VGA: Frequency " << _timeBetweenVsync.Hz() << "Hz \n";
        INFO << "VGA: Num hsync in vsync:"<< _numHsync << "\n";
        #endif

        for (size_t i = 0; i < cVGATimingSize; i++)
        {
            if(cVGATiming[i].frequencyHz == std::round(_timeBetweenVsync.Hz()) &&
               cVGATiming[i].totalVertical == _numHsync)
            {
                if(_currentSetting != i)
                {
                    #ifdef DBG_VDB_VGA
                    INFO << "VGA: Found resolution: "<< cVGATiming[i].horizontalPixels << "*"<< cVGATiming[i].verticalPixels << "\n";
                    #endif
                    _currentSetting = i;

                    svSetScope(_myScope);     
                    _pixelClock->setLowPeriod(cVGATiming[i].pixelClock/2.0);
                    _pixelClock->setHighPeriod(cVGATiming[i].pixelClock/2.0);

                    _pixelClock->enable();

                    vdbVGAMonitorSetHorizontalTiming(&cVGATiming[i].frontPorchHorizontal, &cVGATiming[i].syncHorizontal, &cVGATiming[i].backPorchHorizontal);
                    vdbVGAMonitorSetVerticalTiming(&cVGATiming[i].frontPorchVertical, &cVGATiming[i].syncVertical, &cVGATiming[i].backPorchVertical);
                }
                else
                {
                    // Read the data from VGA monitor
                }

                break;
            }
            else
            {
                _currentSetting = 0xFF;
                _pixelClock->disable();
            }
        }

        _numHsync = 0;
    }
}

