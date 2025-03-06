/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    WX widgets virtual Devboard IC C++ header file               //
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
 * @section vdbComponentIC Virtual development IC component
 *
 * The IC component is a virtual representation of a IC on the development board.
 * At this moment it does not control anything with the verilated context
 * 
 * Ini file properties:
 * | Property name | property type | Mandatory | Description | Default value |
 * |---------------|---------------|-------------|---------------|--------------|
 * | ICHeight | Dimension | no | The height of the IC | 0_mm|
 * | ICWidth | Dimension | no | The width of the IC | 0_mm  |
 * | ICLabel | string | no | The text on the component| "" |
 * 
 * 
 * Example ini file:
 * [FPGA]
 * type=IC
 * xoffset=39_mm
 * yoffset=26.5_mm
 * ICHeight=23_mm
 * ICWidth=23_mm
 * ICLabel=FPGA
 * 
 */

#ifndef WX_WIDGETS_IC_HPP
#define WX_WIDGETS_IC_HPP

#include "wxWidgetsVdbBase.hpp"

namespace RoaLogic {
    using namespace observer;
    using namespace vdb;
namespace GUI {

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

    /**
     * @class cWXVdbIC
     * @author Richard Herveille
     * @brief IC virtual development board component
     * 
     * @TODO: Adjust the file structure to have a seperate vdbIC file, this for a similar structure
     * 
     * @details
     * This class draws an abstract IC. If event handling is required, then that must be
     * handled in a derived class.
     */
    class cWXVdbIC : public cWXVdbBase
    {
        public:
        static constexpr std::string cICHeight = "ICHeight";
        static constexpr std::string cICWidth = "ICWidth";
        static constexpr std::string cICLabel = "ICLabel";

        private:
        sVdbICInformation* _myInformation;

        public:
	/**
	 * @brief Constructor
	 */
        cWXVdbIC(cVDBCommon* myVDBComponent, distancePoint position, wxWindow* windowParent, sVdbICInformation* information, double angle=0);

	/**
	 * @brief Destructor
	 */
        ~cWXVdbIC() {}

        /**
	 * @brief Paint the widget
	 * @details This function paints the widget. 
	 */
        void OnPaint(wxPaintEvent& event);
    };

}}

#endif
