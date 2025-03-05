/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Virtual Devboard LED Verilator C++ header file               //
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

/**
 * @section vdbComponentLED Virtual development LED component
 *
 * The LED component is a virtual representation of a LED on the development board.
 * It shall be controlled by the verilated model and can be used to show the state of a signal.
 * 
 * The following led types are supported:
 * - Round 10mm through hole led (Round10)
 * - Round 5mm through hole led (Round5)
 * - Round 3mm through hole led (Round3)
 * - SMD 1206 led (SMD1206)
 * - SMD 0805 led (SMD0805)
 * - SMD 0603 led (SMD0603)
 * - SMD 0402 led (SMD0402)
 * - SMD 3520 led (SMD3520)
 * 
 * 
 * Ini file properties:
 * | Property name | property type | Mandatory | Description | Default value |
 * |---------------|---------------|-------------|---------------|--------------|
 * | ledType | String | no | The type of LED | ledType=SMD3520 |
 * | ledColour | Color | no | The colour of the LED | ledColour=255,0,0  |
 * 
 * 
 * Example ini file:
 * [LED1]
 * type=LED
 * scope="TOP.de10lite_verilator_wrapper.gen_vdbLED[0].LED_inst"
 * id=1
 * xoffset=48mm
 * yoffset=66mm
 * ledType=SMD3520
 * ledColour=255,0,0 
 * 
 */

#ifndef VDB_LED_HPP
#define VDB_LED_HPP

#include "vdbCommon.hpp"

namespace RoaLogic
{
namespace vdb
{
    /** @enum eVdbLedType
     *  @brief Define the LED type
     */
    enum class eVdbLedType
    {
        unknownLedType, //!< Unknown LED type
        round10mm,      //!< Round through hole 10 mm led
        round5mm,       //!< Round through hole 5 mm led
        round3mm,       //!< Round through hole 3mm led
        SMD1206,        //!< SMD led size 1206
        SMD0805,        //!< SMD led size 0805
        SMD0603,        //!< SMD led size 0603
        SMD0402,        //!< SMD led size 0402
        SMD3520,        //!< SMD led size 3.5x2.0mm
    };

    /**
     * @brief Structure for LED type lookup
     * 
     */
    struct sVdbLedTypeLookup
    {
        eVdbLedType type;
        std::string typeName;
    };

    /**
     * @brief Lookup table for LED types
     */
    static const sVdbLedTypeLookup cVdbLedTypeLookup[] = 
    {
        {eVdbLedType::round10mm,     "Round10"},
        {eVdbLedType::round5mm,      "Round5"},
        {eVdbLedType::round3mm,      "Round3"},
        {eVdbLedType::SMD1206,       "SMD1206"},
        {eVdbLedType::SMD0805,       "SMD0805"},
        {eVdbLedType::SMD0603,       "SMD0603"},
        {eVdbLedType::SMD0402,       "SMD0402"},
        {eVdbLedType::SMD3520,       "SMD3520"},
    };
    static const size_t cVdbLedTypeLookupLookUpSize = sizeof(cVdbLedTypeLookup) / sizeof(cVdbLedTypeLookup[0]);

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

    /**
     * @class cVdbLed
     * @author Bjorn Schouteten
     * @brief Virtual GUI LED controlled by verilog instance
     * @version 0.1
     * @date 13-oct-2024
     * 
     * @details This class controls a verilated LED instance.
     * 
     * It takes the verilator event through the verilator callback and notifies
     * anyone listening to this led. This class fully runs in the verilated context.
     * The base is the cVDBCommon class which does all the low level handling and 
     * setting up the callback mechanism for any DPI functions. The user shall call
     * the cVDBCommon::processVerilatorEvent with the scope and the eventual event.
     * 
     */
    class cVdbLed : public cVDBCommon
    {
        public:
        static constexpr std::string _cLedType = "ledType";
        static constexpr std::string _cLedColourName = "ledColour";

        enum class eVdbLedEvent
        {
            ledOn,
            ledOff
        };

        private:
        void verilatorCallback(uint32_t event);

        public:
        cVdbLed(std::string scopeName, uint8_t id);
        ~cVdbLed();

        static eVdbLedType getLedType(std::string type)
        {
            for(size_t i = 0; i < cVdbLedTypeLookupLookUpSize; i++)
            {
                if(cVdbLedTypeLookup[i].typeName == type)
                {
                    return cVdbLedTypeLookup[i].type;
                }
            }

            return eVdbLedType::SMD3520;
        }
    };
}
}

#endif
