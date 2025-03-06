/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Virtual Devboard 7-Segment Display Verilator C++ header file //
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
 * @section vdbComponent7Seg Virtual development 7 segment component
 *
 * The 7 Segment component is a virtual representation of a 7 segment display on the development board.
 * It shall be controlled by the verilated model and can be used to give an indication to the user.
 * 
 * The following 7 segment types are supported:
 * - commonAnode (anode)
 * - commonCathode (cathode)
 * 
 * 
 * Ini file properties:
 * | Property name | property type | Mandatory | Description | Default value |
 * |---------------|---------------|-------------|---------------|--------------|
 * | 7segType | String | no | The type of 7segment | 7segType=anode |
 * | 7segColour | Color | no | The colour of the 7segment | ledColour=255,0,0  |
 * 
 * 
 * Example ini file:
 * [7SEG]
 * type=7SEG
 * scope=TOP.de10lite_verilator_wrapper.gen_vdb7SegmentDisplay[0].hex_inst
 * id=1
 * xoffset=1_mm
 * yoffset=79_mm
 * 7segColour=255,0,0
 * 7segType=commonAnode
 * 
 */

#ifndef VDB_7SEGMENT_HPP
#define VDB_7SEGMENT_HPP

#include "vdbCommon.hpp"

namespace RoaLogic
{
namespace vdb
{
    
    /** @enum eVdb7SegType
     *  @brief Define the 7 segment types
     */
    enum class eVdb7SegType
    {
        commonAnode,   //!< 7 Segment display which uses a common anode
        commonCathode, //!< 7 Segment display which uses a common cathode
    };

    /**
     * @brief Structure for LED type lookup
     * 
     */
    struct sVdb7SegTypeLookup
    {
        eVdb7SegType type;
        std::string typeName;
    };

    /**
     * @brief Lookup table for LED types
     */
    static const sVdb7SegTypeLookup cVdb7SegTypeLookup[] = 
    {
        {eVdb7SegType::commonAnode,   "anode"},
        {eVdb7SegType::commonCathode, "cathode"},
    };
    static const size_t cVdb7SegTypeLookupLookUpSize = sizeof(cVdb7SegTypeLookup) / sizeof(cVdb7SegTypeLookup[0]);



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

    /**
     * @class cVdb7SegmentDisplay
     * @author Bjorn Schouteten
     * @brief Virtual GUI 7-Segment Display controlled by verilog instance
     * @version 0.1
     * @date 16-nov-2024
     *
     * @details This class controls a verilated 7-Segment Display instance.
     *
     * It takes the verilator event through the verilator callback and notifies
     * anyone listening to this led. This class fully runs in the verilated context.
     * The base is the cVDBCommon class which does all the low level handling and 
     * setting up the callback mechanism for any DPI functions. The user shall call
     * the cVDBCommon::processVerilatorEvent with the scope and the eventual event.
     */
    class cVdb7SegmentDisplay : public cVDBCommon
    {
        private:
        void verilatorCallback(uint32_t event);

        public:
        static constexpr std::string _c7SegType = "7segType";
        static constexpr std::string _c7SegColourName = "7segColour";
        
        cVdb7SegmentDisplay(std::string scopeName, uint8_t id);
        ~cVdb7SegmentDisplay();

        static eVdb7SegType get7SegType(std::string type)
        {
            for(size_t i = 0; i < cVdb7SegTypeLookupLookUpSize; i++)
            {
                if(cVdb7SegTypeLookup[i].typeName == type)
                {
                    return cVdb7SegTypeLookup[i].type;
                }
            }

            return eVdb7SegType::commonAnode;
        }
    };
}
}

#endif
