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
#include "distance.hpp"

namespace RoaLogic {
    using namespace observer;
    using namespace vdb;
    using namespace dimensions;
namespace GUI {

    /** @enum eVdbComponentType
     *  @brief Enum which defines all the different 
     * virtual development board components.
     * 
     * @details This enumeration defines all possible
     * virtual development board components which can be
     * created in the system.
     * 
     * When a new component is added, this enum shall be 
     * extended
     */
    enum class eVdbComponentType
    {
        vdbNone,                //!< No component
        vdbLed,                 //!< LED component, uses the sVdbLedInformation structure to handle its layout
        vdbConnector,           //!< Connector component (now only VGA), does not yet use any information
        vdbHeader,              //!< Header component, uses the sVdbHeaderInformation structure to handle its layout
        vdbVGA,                 //!< VGA component, does not use any information
        vdb7SegmentDisplay,     //!< 7 segment component, uses the sVdb7SegInformation structure to handle its layout
	    vdbIC,                  //!< IC component, uses the sVdbICInformation structure to handle its layout
    };

    struct sVdbComponentTypeLookUp
    {
        eVdbComponentType type;
        std::string typeName;
    };

    static const sVdbComponentTypeLookUp cVdbComponentTypeLookUp[] = 
    {
        {eVdbComponentType::vdbNone,            "None"},
        {eVdbComponentType::vdbLed,             "LED"},
        {eVdbComponentType::vdbConnector,       "Connector"},
        {eVdbComponentType::vdbHeader,          "Header"},
        {eVdbComponentType::vdbVGA,             "VGA"},
        {eVdbComponentType::vdb7SegmentDisplay, "7SEG"},
        {eVdbComponentType::vdbIC,              "IC"},
    };
    static const size_t cVdbComponentTypeLookUpSize = sizeof(cVdbComponentTypeLookUp) / sizeof(cVdbComponentTypeLookUp[0]);

    /** @enum eVdbConnectorType
     *  @brief Defines the connector types
     */
    enum class eVdbConnectorType
    {
        DSUB   //!< D-SUB style
    };

    /** @struct sVdbConnectorInformation
     *  @brief virtual development board connector information
     *  @details This structure is used to design a virtual development board connector
     *
     * The type of connector is defined through the eVdbConnectorType enumeration
     * Size is defined through the size variable
     */
    struct sVdbConnectorInformation
    {
        eVdbConnectorType type;
        cDistance         width;
        cDistance         height;
        std::string       label;
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
        virtual void addVdbComponent(eVdbComponentType type, cVDBCommon* vdbComponent, distancePoint point, void* information, double angle=0) = 0;
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
        cVDBCommon*   _myVDBComponent;
        distancePoint _myScreenPosition;

        public:
        cGuiVDBComponent(cVDBCommon* myVDBComponent, distancePoint myPosition) :
            _myVDBComponent(myVDBComponent),
            _myScreenPosition(myPosition)
        {
            if (myVDBComponent) myVDBComponent->registerObserver(this);
        }

        ~cGuiVDBComponent()
        {
            removeObserver();
        }

        void removeObserver()
        {
            if (_myVDBComponent) _myVDBComponent->removeObserver(this);
        }

        size_t getID() const
        {
            return _myVDBComponent ? _myVDBComponent->getID() : 0;
        }

        int getIntID() const
        {
            return static_cast<int>(getID());
        }

        virtual void onClose(){};
        virtual void notify(eEvent aEvent, void* data) = 0;
    };

}}

#endif
