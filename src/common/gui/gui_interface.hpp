/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    GUI interface class                                          //
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

#ifndef GUI_INTERFACE_HPP
#define GUI_INTERFACE_HPP

#include "subject.hpp"
#include "vdbCommon.hpp"

namespace RoaLogic {
    using namespace observer;
    using namespace vdb;
namespace GUI {

    enum class eVdbComponentType
    {
        vdbLed,
        vdbVGA,
        vdb7SegmentDisplay
    };

    /**
     * @class cGuiInterface
     * @author Bjorn Schouteten
     * @brief GUI interface
     * @version 0.1
     * @date 19-okt-2024
     *
     * @details This class is a interface which shall be derived
     * by the GUI implementation.
     * 
     * When an event happens in the GUI implementation it can be 
     * processed by any observer listening to this interface.
     * 
     * At the moment that the system wants to show something to the
     * user it will call the corresponding interface function. The
     * implementation of the interface will then convert the event
     * to the GUI event handling system.
     * 
     * @attention Be aware that it's likely that the interface and
     * verilator are running in different threads. When calling any
     * of these function or sending a notification, the corresponding
     * function is still in the callers context. 
     * 
     */
    class cGuiInterface : public cSubject
    {
        public:        
        virtual void addVirtualLED(cVDBCommon* vdbComponent, char color) = 0;
        virtual void addVirtualVGA(cVDBCommon* vdbComponent) = 0;
        virtual void addVirtual7SegmentDisplay(cVDBCommon* vdbComponent) = 0;
    };

    /**
     * @class cGuiVDBComponent
     * @author Bjorn Schouteten
     * @brief GUI virtual development board component
     * @version 0.1
     * @date 03-nov-2024
     * @details
     * This class is a base class for any GUI element which implements
     * a verilated vdb component. It makes sure that all events from the
     * verilated vdb component are passed through the notify function.
     * 
     * @todo: Add a method to sent data from the GUI to the verilated design
     */
    class cGuiVDBComponent : public cObserver
    {
        private:
        cVDBCommon* _myVDBComponent;

        protected:
        uint32_t _myID;

        public:
        cGuiVDBComponent(cVDBCommon* myVDBComponent, uint32_t id) :
            _myVDBComponent(myVDBComponent),
            _myID(id)
        {
            myVDBComponent->registerObserver(this);
        }

        ~cGuiVDBComponent()
        {
            _myVDBComponent->removeObserver(this);
        }

        void removeObserver()
        {
            _myVDBComponent->removeObserver(this);
        }

        virtual void onClose(){};
        virtual void notify(eEvent aEvent, void* data) = 0;
    };

}}

#endif
