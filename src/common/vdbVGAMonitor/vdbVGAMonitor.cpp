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
    float pixelClockMHz;
    uint16_t totalHorizontal; // Active video + front porch + sync + back porch
    uint16_t totalVertical; // Active video + front porch + sync + back porch
};

static const sVGATiming cVGATiming[] =
{
    { 640, 480, 60, 25.175,  800, 524},
    { 640, 480, 72, 31.500,  832, 520},
    { 640, 480, 75, 31.500,  800, 524},
    { 640, 480, 85, 36.000,  832, 509},
    { 800, 600, 56, 38.100, 1088, 619},
    { 800, 600, 60, 40.000, 1056, 628},
    { 800, 600, 72, 50.000,  932, 666},
    { 800, 600, 75, 49.500, 1056, 624},
    { 800, 600, 85, 56.250, 1048, 632},
    {1024, 768, 60, 65.000, 1344, 806},
    {1024, 768, 70, 75.000, 1328, 804},
    {1024, 768, 75, 78.750, 1312, 800},
    {1024, 768, 85, 94.500, 1376, 808}
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
    cVdbVGA::processVGAEvent(id, eVgaEvent::hsync);
}

/**
 * @brief VGA Monitor VSYNC DPI-C callback
 * @details This function gets called when the VGA Monitor detects a VSYNC
 * 
 */
void vdbVGAMonitorVSYNC(int id)
{
    cVdbVGA::processVGAEvent(id, eVgaEvent::vsync);
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
void cVdbVGA::processVGAEvent(int vgaID, eVgaEvent event)
{
    bool found = false;

    for (const sVdbVGAMap& ref : _referencePointers)
    {
        if(ref.ID == vgaID)
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
        WARNING << "VGA: Event on non found module: " << vgaID << " \n";
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

cVdbVGA::cVdbVGA(std::string scopeName, int id, cTimeInterface* timeInterface) :
    _timeInterface(timeInterface)
{
    registerVirtualVGA(sVdbVGAMap{id, this});

    const svScope scope = svGetScopeFromName(scopeName.c_str());
    svSetScope(scope);
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
    bool found = false;

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
                #ifdef DBG_VDB_VGA
                INFO << "VGA: Found resolution: "<< cVGATiming[i].horizontalPixels << "*"<< cVGATiming[i].verticalPixels << "\n";
                #endif
                found = true;
                break;
            }
        }

        if(found)
        {
            svBitVecVal value = 10;
            vdbVGAMonitorSetHorizontalTiming(&value, &value, &value);
        }

        _numHsync = 0;
    }
}

