/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets virtual Devboard Header C++ header file           //
//                                                                 //
/////////////////////////////////////////////////////////////////////
//                                                                 //
//    Copyright (C) 2025 Roa Logic BV - www.roalogic.com           //
//    Copyright (C) 2025 richard.herveille@roalogic.com            //
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
 * @section vdbComponentHeader Virtual development Header component
 *
 * The Header component is a virtual representation of a Header on the development board.
 * At this moment it does not control anything in the verilated design
 * 
 * The following header types are supported (the text following the type is the name used in the ini file):
 * - Male (male)
 * - Female (female)
 * - Male angled (male_angled)
 * - Female angled (female_angled)
 * - Male boxed (male_boxed)
 * - Female boxed (female_boxed)
 * - Male angled and boxed (male_angled_boxed)
 * - Female angled and boxed (female_angled_boxed)
 * 
 * 
 * Ini file properties:
 * | Property name | property type | Mandatory | Description | Default value |
 * |---------------|---------------|-------------|---------------|--------------|
 * | headerType | String | no | The type of header | headerType=male |
 * | headerRows | int | no | The number of rows for the header | headerRows=1  |
 * | headerColums | int | no | The number of colums for the header | headerColums=2 |
 * | headerPitch | dimension | no | The pitch of the header | headerPitch=2.54_mm | 
 * 
 * 
 * Example ini file:
 * [header1]
 * type=Header
 * xoffset=20_mm
 * yoffset=0_mm
 * headerType=MALE_BOXED
 * headerRows=2
 * headerColums=20
 * headerPitch=2.54_mm
 * 
 */

#ifndef WX_WIDGETS_VDB_HEADER_HPP
#define WX_WIDGETS_VDB_HEADER_HPP

#include "wxWidgetsVdbBase.hpp"

namespace RoaLogic {
    using namespace observer;
    using namespace vdb;
namespace GUI {

    /**
     * @enum eVdbHeaderType
     * @brief Defines the header types
     * bit0 = male/female
     * bit1 = angled/straight
     * bit2 = boxed/non-boxed
     */
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
     * @brief Structure for LED type lookup
     * 
     */
    struct sVdbHeaderTypeLookup
    {
        eVdbHeaderType type;
        std::string typeName;
    };

    /**
     * @brief Lookup table for LED types
     */
    static const sVdbHeaderTypeLookup cVdbHeaderTypeLookup[] = 
    {
        {eVdbHeaderType::MALE,                  "male"},
        {eVdbHeaderType::FEMALE,                "female"},
        {eVdbHeaderType::MALE_ANGLED,           "male_angled"},
        {eVdbHeaderType::FEMALE_ANGLED,         "female_angled"},
        {eVdbHeaderType::MALE_BOXED,            "male_boxed"},
        {eVdbHeaderType::FEMALE_BOXED,          "female_boxed"},
        {eVdbHeaderType::MALE_ANGLED_BOXED,     "male_angled_boxed"},
        {eVdbHeaderType::FEMALE_ANGLED_BOXED,   "female_angled_boxed"},
    };
    static const size_t cVdbHeaderTypeLookupLookUpSize = sizeof(cVdbHeaderTypeLookup) / sizeof(cVdbHeaderTypeLookup[0]);


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
     * @class cWXVdbHeader
     * @author Richard Herveille
     * @brief virtual development board header type component
     * 
     * @details
     * This class draws an abstract header. If event handling is required, then that must be
     * handled in a derived class.
     */
    class cWXVdbHeader : public cWXVdbBase
    {
        public:
        static constexpr std::string cHeaderTypeText = "headerType";
        static constexpr std::string cHeaderRowsText = "headerRows";
        static constexpr std::string cHeaderColumsText = "headerColums";
        static constexpr std::string cHeaderPitchText = "headerPitch";

        private:
        static const int cFemaleBitValue = 0x01;
        static const int cAngledBitValue = 0x02;
        static const int cBoxedBitValue  = 0x04;
        static inline wxColour colGold = wxColour(255,215,0);

        public:
        /**
         * @brief Constructor
         */
        cWXVdbHeader(cVDBCommon* myVDBComponent, distancePoint position, wxWindow* windowParent, sVdbHeaderInformation* information, double angle=0);

        /**
         * @brief Destructor
         */
        ~cWXVdbHeader() {}

        /**
         * @brief Header size
         * @details Returns the size of the header
         */
        virtual distanceSize GetDeviceSize() const;
        virtual distanceSize GetDeviceSize(sVdbHeaderInformation* information) const;

        /**
         * @brief Paint the widget
         * @details This function paints the widget. 
         */
        void OnPaint(wxPaintEvent& event);

        static eVdbHeaderType getHeaderType(std::string type)
        {
            for(size_t i = 0; i < cVdbHeaderTypeLookupLookUpSize; i++)
            {
                if(cVdbHeaderTypeLookup[i].typeName == type)
                {
                    return cVdbHeaderTypeLookup[i].type;
                }
            }

            return eVdbHeaderType::MALE;
        }
    };
}}

#endif
