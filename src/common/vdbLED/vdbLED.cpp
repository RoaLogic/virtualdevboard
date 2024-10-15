/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Virtual Devboard LED Verilator C++ wrapper                   //
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

#include "vdbLED.hpp"

#include <wx/graphics.h>

//#define DBG_VDB_LED

// Define the wxEVT_LED, which is special within this class
wxDEFINE_EVENT(wxEVT_LED, wxCommandEvent);

wxEvtHandler* cVirtualLed::_parent; // The parent handler for the LED events
// Internal reference for mapping ID's to cVirtualLed instances
std::vector<cVirtualLed::sVirtualLedMap> cVirtualLed::_referencePointers; 

/**
 * @brief LED-On DPI-C callback
 * @details This function gets called when the LED 
 * state in the design changes state to on.
 * 
 * It will pass it's event to the handleLedEvent so that it 
 * can be processed correctly within the virtual led class.
 * 
 * @attention This function runs in the verilator thread context
 */
void vdbLedOn(int id)
{
    //get instance scope
    //svScope scope = svGetScope();

    //get hierarchical name
    //const char* scopeName = svGetNameFromScope(scope);

    cVirtualLed::SendLedEvent(id, true);
}

/**
 * @brief LED-Off DPI-C callback
 * @details This function gets called when the LED 
 * state in the design changes state to off.
 * 
 * It will pass it's event to the handleLedEvent so that it 
 * can be processed correctly within the virtual led class.
 * 
 * @attention This function runs in the verilator thread context
 */
void vdbLedOff(int id)
{
    //get instance scope
    //svScope scope = svGetScope();

    //get hierarchical name
    //const char* scopeName = svGetNameFromScope(scope);

    cVirtualLed::SendLedEvent(id, false);
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
 * @param[in] ledID     The ID of the LED which had the event
 * @param[in] state     The new state of the LED
 */
void cVirtualLed::SendLedEvent(int ledID, bool state)
{
    wxCommandEvent testEvent{wxEVT_LED};
    sEventData* const eventData{ new sEventData};

    if(_parent)
    {
        eventData->id = ledID;
        eventData->on = state;

        #ifdef DBG_VDB_LED
        INFO << "Received led " << eventData->id << " state: " << eventData->on << " Thread: " <<  wxThread::GetCurrentId() << "\n";
        #endif

        testEvent.SetClientObject(eventData);
        wxPostEvent(_parent, testEvent);
    }
}

/**
 * @brief Handle a led event in the GUI thread
 * @details 
 * Within this function a LED event from the system is handled. 
 * 
 * @attention This function runs in the GUI thread context
 * 
 * @param[in] event     The event which happend
 */
void cVirtualLed::OnLedEvent(wxCommandEvent& event)
{
    sEventData* eventData = reinterpret_cast<sEventData*>(event.GetClientObject());

    for (const sVirtualLedMap& ref : _referencePointers)
    {
        if(ref.ID == eventData->id)
        {
            ref.ledReference->SetStatus(eventData->on);

            #ifdef DBG_VDB_LED
            INFO << "Processed led " << eventData->id << " state: " << eventData->on << " Thread: " << wxThread::GetCurrentId() <<"\n";
            #endif
        }
    }
}

/**
 * @brief Register a new virtual LED class 
 * @details This function registers a new virtual LED class with the static modules.
 * 
 * It stores the mapping information into the vector and stores the global parent variabele
 * 
 * @param[in] map       The mapping information to store
 * @param[in] aParent   A pointer to the GUI parent
 */
void cVirtualLed::registerVirtualLed(sVirtualLedMap map, wxEvtHandler* aParent)
{
    _referencePointers.push_back(map);
    _parent = aParent;
}

/**
 * @brief Constructor of a new virtual LED
 * 
 * @param[in] aParent       Pointer to the parent event handler
 * @param[in] id            ID of the LED
 * @param[in] windowParent  Pointer to the parent window
 * @param[in] Position      Relative position on the parent window
 * @param[in] Size          The size of the LED
 * @param[in] color         The led color
 */
cVirtualLed::cVirtualLed(wxEvtHandler* aParent, int id, wxWindow* windowParent, wxPoint Position, int Size, char color) :
    wxWindow(windowParent, wxID_ANY, Position, wxSize(Size,Size), wxTRANSPARENT_WINDOW, 'R'),
    _windowParent(windowParent),
    _color(color)
{
    registerVirtualLed(sVirtualLedMap{id, this},  aParent);
    aParent->Bind(wxEVT_LED, cVirtualLed::OnLedEvent);
    Connect(wxEVT_PAINT, wxPaintEventHandler(cVirtualLed::OnPaint));
}

/**
 * @brief 
 * 
 * @param event 
 */
void cVirtualLed::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

    D1 = GetClientSize().GetX();

    // Generar paradas de la gradiente
    wxGraphicsGradientStops stops;

    if(FlagStatus)
    {
        switch(_color)
        {
            case 'r':
            case 'R':   stops.Add(wxColor(255,255,255),0.0f);
                        stops.Add(wxColor(255,0,0),1.0f);
                        break;

            case 'g':
            case 'G':   stops.Add(wxColor(255,255,255),0.0f);
                        stops.Add(wxColor(0,255,0),1.0f);
                        break;

            case 'b':
            case 'B':   stops.Add(wxColor(255,255,255),0.0f);
                        stops.Add(wxColor(0,0,255),1.0f);
                        break;

            case 'y':
            case 'Y':   stops.Add(wxColor(255,255,255),0.0f);
                        stops.Add(wxColor(255,255,0),1.0f);
                        break;

            default :   stops.Add(wxColor(255,255,255),0.0f);
                        stops.Add(wxColor(255,0,0),1.0f);
                        break;
        }
    }
    else
    {
        switch(_color)
        {
            case 'r':
            case 'R':   stops.Add(wxColor(128,0,0),0.0f);
                        stops.Add(wxColor(0,0,0),1.0f);
                        break;

            case 'g':
            case 'G':   stops.Add(wxColor(0,128,0),0.0f);
                        stops.Add(wxColor(0,0,0),1.0f);
                        break;

            case 'b':
            case 'B':   stops.Add(wxColor(0,0,128),0.0f);
                        stops.Add(wxColor(0,0,0),1.0f);
                        break;

            case 'y':
            case 'Y':   stops.Add(wxColor(128,128,0),0.0f);
                        stops.Add(wxColor(0,0,0),1.0f);
                        break;

            default :   stops.Add(wxColor(128,0,0),0.0f);
                        stops.Add(wxColor(0,0,0),1.0f);
                        break;
        }
    }

    // Dibujar led
    gc->SetPen(wxPen(wxColor(0,0,0), 1, wxTRANSPARENT));
    gc->SetBrush(gc->CreateLinearGradientBrush(0,0,D1,D1,wxColor(35,35,35),wxColor(180,180,180)));
    gc->DrawEllipse(0,0,D1,D1);

    D2 = (2*D1)/3;
    x1 = D1/6;
    y1 = x1;

    gc->SetBrush(gc->CreateRadialGradientBrush(D1/2,D1/2,D1/2,D1/2,D2/2,stops));
    gc->DrawEllipse(x1,y1,D2,D2);

    delete gc;
}

void cVirtualLed::SetStatus(bool status)
{
    FlagStatus = status;    
    Refresh();
}

void cVirtualLed::SetColor(char color)
{
    _color = color;
    Refresh();
}
