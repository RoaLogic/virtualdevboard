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

    /** @struct sRGBColor
     *  @brief RGB colour structure
     *  @details This structure can be used to
     * determine the colour according to the RGB value.
     */
    struct sRGBColor
    {
        uint8_t red;        //!< Red value of RGB
        uint8_t green;      //!< Green value of RGB
        uint8_t blue;       //!< Blue value of RGB
    };

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
        //!< Led component, uses the sVdbLedInformation structure to handle its layout
        vdbLed,
        //!< Connector component (now only DSUB), Uses width/height from information to handle its layout
        vdbConnector,
        //!< Header component, uses sVdbHeaderInformation structure to handle its layout
        vdbHeader,
        //!< VGA component, does not use any information
        vdbVGA,
        //!< 7 segment component, uses the sVdb7SegInformation structure to handle its layout
        vdb7SegmentDisplay,
	//!< IC component, uses the sVdbICInformation structure to handle its layout
	vdbIC
    };

    /** @enum eVdbLedType
     *  @brief Define the LED type
     */
    enum class eVdbLedType
    {
        round10mm,  //!< Round through hole 10 mm led
        round5mm,   //!< Round through hole 5 mm led
        round3mm,   //!< Round through hole 3mm led
        SMD1206,    //!< SMD led size 1206
        SMD0805,    //!< SMD led size 0805
        SMD0603,    //!< SMD led size 0603
        SMD0402,    //!< SMD led size 0402
        SMD3520     //!< SMD led size 3.5x2.0mm
    };

    /** @struct sVdbLedInformation
     *  @brief virtual development board led information
     *  @details This structure is used to design a
     * virtual development board led.
     * 
     * The type of led is defined through the eVdbLedType 
     * enumeration. Colour of the LED can be passed in as 
     * RGB color through the red, green and blue values.
     */
    struct sVdbLedInformation
    {
        eVdbLedType type;   //!< The type of the LED
        sRGBColor colour;   //!< Colour of the LED
    };

    /** @enum eVdb7SegType
     *  @brief Define the 7 segment types
     */
    enum class eVdb7SegType
    {
        commonAnode,   //!< 7 Segment display which uses a common anode
        commonCathode, //!< 7 Segment display which uses a common cathode
    };

    /** @struct sVdb7SegInformation
     *  @brief virtual development board 7 segment information
     *  @details This structure is used to design a
     * virtual development board 7 segment display.
     * 
     * The type of 7segment is defined through the eVdb7SegType 
     * enumeration. Color of the 7 segment can be passed in as 
     * RGB color through the red, green and blue values.
     */
    struct sVdb7SegInformation
    {
        eVdb7SegType type;
        sRGBColor colour;
    };

    /** @struct sVdbICInformation
     *  @brief virtual development board IC information
     *  @details This structure is used to design a virtual development board IC.
     *           The IC is drawn as a simple rectangle of distanceSize dimension,
     *           with text @label in the center
     */
    struct sVdbICInformation
    {
        cDistance width;
        cDistance height;
        std::string label;
    };

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
     * @enum eVdbHeaderType
     * @brief Defines the header types
     * bit0 = male/female
     * bit1 = angled/straight
     * bit2 = boxed/non-boxed
     */
    #define HDR_FEMALE 1
    #define HDR_ANGLED 2
    #define HDR_BOXED  4
    enum class eVdbHeaderType
    {
        MALE,                //!< Straight male header
        FEMALE,              //!< Straight female header
        MALE_ANGLED,         //!< Angled male header
        FEMALE_ANGLED,       //!< Angled female header
        MALE_BOXED,          //!< Straight male header with a box
        FEMALE_BOXED,        //!< Straight female header with a box
        MALE_ANGLED_BOXED,   //!< Angled male header with a box
        FEMALE_ANGLED_BOXED  //!< Angled female header with a box
    };


    /**
     * @struct sVdbHeaderInformation
     * @brief virtual development board header information
     * @details This structure is used to design a virtual development board header
     */
    struct sVdbHeaderInformation
    {
        eVdbHeaderType type;
        uint32_t       rows;
        uint32_t       columns;
        cDistance      pitch;
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
        virtual void setupGui(std::string applicationName, std::string aboutTitle, std::string aboutText, distanceSize minimalScreenSize, sRGBColor backgroundColor) = 0;
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
