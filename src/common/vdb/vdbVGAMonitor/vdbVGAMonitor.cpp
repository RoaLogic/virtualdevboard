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

using namespace RoaLogic::vdb;
#define DBG_MEASURE_VDB_VGA

#ifdef DBG_MEASURE_VDB_VGA
#include <chrono>
using namespace std::chrono;
steady_clock::time_point _previousVsync;
uint32_t hsyncCount = 0;
uint64_t hsyncTimeUs = 0;
#endif

/**
 * The following structure is build to compress the full VGA timing per VESA specs.
 * See the system verilog file for the values.
 */
struct sVGATiming
{
    svBitVecVal  horizontalPixels;
    svBitVecVal  verticalPixels;
    uint8_t frequencyHz;
    long double pixelClock;
    svBitVecVal  totalHorizontal; // Active video + front porch + sync + back porch
    svBitVecVal  frontPorchHorizontal;
    svBitVecVal  syncHorizontal;
    svBitVecVal  backPorchHorizontal;
    svBitVecVal  totalVertical; // Active video + front porch + sync + back porch
    svBitVecVal  frontPorchVertical;
    svBitVecVal  syncVertical;
    svBitVecVal  backPorchVertical;
};

/**
 * @details This is a lookup table for the VGA timings. It is tailored to be used
 * within this class, for a general overview see the vdbVGAMonitor.sv
 */
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
/**
 * @details The number of elements in the VGA timing lookup table
 */
static const size_t cVGATimingSize = sizeof(cVGATiming) / sizeof(cVGATiming[0]);

//!< Static internal references for mapping scopes to cVdbVGA instances
std::vector<cVdbVGAMonitor::sVdbVGAMap> cVdbVGAMonitor::_referencePointers; 

/**
 * @brief VGA Monitor VSYNC DPI-C callback
 * @details This function gets called when the VGA Monitor detects a VSYNC
 * 
 * @note Static function used by each VGA object
 * 
 * @param[in] ID    The ID of the VGA, currently unused
 */
void vdbVGAMonitorVSYNC(int id)
{
    #ifdef DBG_MEASURE_VDB_VGA
    auto start =steady_clock::now();
    #endif
    // Get the scope of the current call and pass this into the processing function
    cVdbVGAMonitor::processVGAEvent(svGetScope(), cVdbVGAMonitor::eVgaEvent::vsync);

    #ifdef DBG_MEASURE_VDB_VGA
    auto stop = steady_clock::now();
    INFO << "VSync duration: " << duration_cast<microseconds>(stop - start).count() << " us \n";
    INFO << "VSync timing: " << duration_cast<milliseconds>(stop - _previousVsync).count() << " ms \n";
    _previousVsync = steady_clock::now();
    #endif
}

namespace RoaLogic
{
    using namespace observer;
namespace vdb
{
    /**
     * @brief Register a new virtual VGA class 
     * @details This function registers a new virtual VGA class with the static modules.
     * 
     * It stores the reference information into the vector and stores the global parent variabele
     * 
     * @note Static function used by each VGA object
     * 
     * @param[in] reference       The reference information to store
     */
    void cVdbVGAMonitor::registerVirtualVGA(sVdbVGAMap reference)
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
     * @note Static function used by each VGA object
     * 
     * @param[in] scope       The scope of the VGA event
     * @param[in] event       The VGA event which occured
     */
    void cVdbVGAMonitor::processVGAEvent(svScope scope, eVgaEvent event)
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
     * @brief Construct a new cVdbVGAMonitor class
     * @details This function is the constructor of the cVdbVGAMonitor class
     * 
     * It needs the scopeName to map verilated context to the C++ context. The time interface
     * is used to get the simulation time, which is needed for calculating the VGA frequency. Where
     * the pixelClock must be generated within this class to trigger the module internal clocking.
     * 
     * @param[in] scopeName     The scope of this class
     * @param[in] timeInterface Pointer to the timing interface
     * @param[in] pixelClock    Pointer to the pixelClock for generating the VGA pixel clock
     */
    cVdbVGAMonitor::cVdbVGAMonitor(std::string scopeName, cTimeInterface* timeInterface, cClock* pixelClock,
                VlUnpacked<unsigned int,cMaxVerticalLines*cMaxHorizontalLines>& framebuffer) :
        _timeInterface(timeInterface),
        _pixelClock(pixelClock),
        _myFramebuffer(framebuffer)
    {
        // Make sure that the passed parameters are not nullpointers
        assert(timeInterface != nullptr);   
        assert(pixelClock != nullptr);

        // Get the scope according to the given name
        _myScope = svGetScopeFromName(scopeName.c_str());
        svSetScope(_myScope);

        // Make sure that the scope is not a nullpointer
        assert(_myScope != nullptr);

        // Store this class instance in the gloval class instances
        registerVirtualVGA(sVdbVGAMap{_myScope, this});

        _myEventData.horizontalLines = 0;
        _myEventData.verticalLines = 0;

        #ifdef DBG_MEASURE_VDB_VGA
        _previousVsync = std::chrono::steady_clock::now();
        #endif
    }

    /**
     * @details Destruct the virtual VGA module
     */
    cVdbVGAMonitor::~cVdbVGAMonitor()
    {

    }

    /**
     * @brief Handle a VGA Vsync signal  
     * @details This function handles the VSYNC signal from the VGA module.
     * 
     * In this function we calculate the time between each VSYNC, this with the number of HSYNC's
     * within this timeframe can be used to see what the current VGA setting is. Those calculated
     * values are used to compare with the lookup VGA timing table. When no element is found the
     * pixel clock is disabled and the current setting is set to 0xff, the no found value.
     * 
     * The first time we find the right setting, we set the _currentSetting to the corresponding
     * lookup table offset. With this we use _currentSetting for lookup table indexing. In case
     * the right setting is found the scope is set and the corresponding settings are sent to the 
     * verilated context. Also the event data information is set correctly, so that the user knows
     * that the image is about to change. At last the pixel clock is calculated and started, 
     * so that we get the right data from the device.
     * 
     * When the setting is already found we get the last line of data and sent the vgaDataReady 
     * event, so that the user knows that it's the last line of data. It is up to the user on how
     * to handle the data of this event. 
     * 
     * @note The passed data is a pointer that is continously updated, make sure that the
     * data abstraction is thread safe.
     */
    void cVdbVGAMonitor::handleVsync()
    {
        simtime_t currentVSyncTime = _timeInterface->getTime();
        simtime_t timeBetweenVsync = currentVSyncTime - _previousVSyncTime;
        _previousVSyncTime = currentVSyncTime;

        #ifdef DBG_VDB_VGA
        INFO << "VGA: Frequency " << timeBetweenVsync.Hz() << "Hz \n";
        INFO << "VGA: Num hsync in vsync:"<< vdbVGAMonitorGetLineCnt() << "\n";
        #endif

        // Loop through the lookup table 
        for (size_t i = 0; i < cVGATimingSize; i++)
        {
            // Check if we have found the right element in the table
            if(cVGATiming[i].frequencyHz == std::round(timeBetweenVsync.Hz()) &&
                cVGATiming[i].totalVertical == vdbVGAMonitorGetLineCnt())
            {
                // _currentSetting is used to see if we already found an element
                if(_currentSetting != i)
                {
                    // New resolution found
                    #ifdef DBG_VDB_VGA
                    INFO << "VGA: Found resolution: "<< cVGATiming[i].horizontalPixels << "*"<< cVGATiming[i].verticalPixels << "\n";
                    #endif
                    _currentSetting = i;

                    // Set the scope and sent the values through the DPI functions
                    svSetScope(_myScope);

                    //Program VGAMonitor model
		            vdbVGAMonitorSetHorizontalTiming(&cVGATiming[i].horizontalPixels,
                                                     &cVGATiming[i].frontPorchHorizontal,
                                                     &cVGATiming[i].syncHorizontal,
                                                     &cVGATiming[i].backPorchHorizontal);
                    vdbVGAMonitorSetVerticalTiming(&cVGATiming[i].verticalPixels,
                                                   &cVGATiming[i].frontPorchVertical,
                                                   &cVGATiming[i].syncVertical,
                                                   &cVGATiming[i].backPorchVertical);

                    // Set the pixel clock timing
                    long double pixelClock = cVGATiming[i].totalHorizontal * cVGATiming[i].totalVertical * timeBetweenVsync.Hz();
                    _pixelClock->setLowPeriod ( (1.0/pixelClock)/2.0 );
                    _pixelClock->setHighPeriod( (1.0/pixelClock)/2.0 );

                    // Enable pixel clock
                    _pixelClock->enable();

                    // Set the horizontal and vertical pixels
                    _myEventData.horizontalLines = cVGATiming[_currentSetting].horizontalPixels;
                    _myEventData.verticalLines = cVGATiming[_currentSetting].verticalPixels;
                }

                // Set the data in the event, number of vertical and horizontal pixels is already set
                _myEventData.dataArray = reinterpret_cast<uRGBValue*>(_myFramebuffer.data());
                notifyObserver(eEvent::vgaDataReady, &_myEventData);

                break;
            }
            else
            {
                _currentSetting = 0xFF;
                _pixelClock->disable();
            }
        }
    }

}}
