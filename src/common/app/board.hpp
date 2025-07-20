/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Generic GUI board header file                                //
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
 * @section guiBoard GUI board layout
 * 
 * A board is defined as a set of components (peripherals) that are shown in the GUI and connect 
 * with the Verilated model. The board layout is defined through a *.ini file, which is parsed by the
 * application. The *.ini file defines the peripherals, their properties and their connections. All 
 * of this is done in the cGuiBoard class, which instantiates, controls and maintains the board and
 * its peripherals. After construction this class runs on the background and is not involved in any
 * of the data exchange between the GUI and the Verilated model.
 * 
 * Peripherals can be in two types, a verilator peripheral or a GUI peripheral. A verilator peripheral 
 * is connected to the Verilated model and is used to control the model, which can be directly linked with 
 * a GUI peripheral. A GUI peripheral is shown in the GUI and can have a connection with a standard 
 * verilator peripheral or just shown on the GUI.
 * 
 * For information about the peripherals, see the @ref vdbComponent_1 section. 
 * 
 * @section ini_file INI file
 * 
 * The ini file is a file which holds all the information about the GUI layout of the board
 * and its peripherals. The file is parsed by the application and the information is used to
 * create the GUI and connect the peripherals to the Verilated model. The file has multiple
 * sections, where each section defines a different part of the board. This can be split in
 * two types, a [board] section and a [periheral] section. The [board] section defines the board
 * layout and is mandatory, while the [peripheral] section defines the peripherals and is optional.
 * There can be as many [peripheral] sections as needed.abort
 * 
 * @attention The application does not check if the peripherals in the ini file overlap. This
 * is up to the user to verify and correct if needed.
 *  
 * All possible properties in a ini file, this counts for all sections. 
 * 
 * |Property type | Description | Example| 
 * |--------------|-------------|---------------|
 * | String | A plain string value, given as a string | type=LED |
 * | Integer | An integer value, given as a string | id=0 |
 * | Dimension | A dimension value is given in mm or inch, where the value is given as a string followed by a underscore and the unit (_mm or _inch) | width=12.5_mm |
 * | Color | A color value is given in R,G,B format | background=0,75,128 |
 * 
 * @subsection vdbBoardIniFile Board data in ini file
 *  
 * A INI file must have a [board] section, which defines the board. The following
 * properties can be added to the board section:
 * 
 * | Property name | property type | Mandatory | Description | Default value |
 * |---------------|---------------|-------------|---------------|--------------|
 * | name | String | yes | The name of the board | name=DE10lite |
 * | description | String | no | The description of the board | description=DE10lite virtual demo board |
 * | aboutTitle | String | no | The about title of the board | aboutTitle=About Virtual Demo Board |
 * | aboutText | String | no | The about information of the board | aboutText=This is a virtual demo board |
 * | width | Dimension | no | The width of the board in mm or inch | width=800_mm |
 * | height | Dimension | no | The height of the board in mm or inch | height=600_mm |
 * | background | Color | no | The background color of the board | background=0,0,0 |
 *  
 * @subsubsection example Example board section
 * [board]
 * name=DE10lite
 * description=DE10lite virtual demo board
 * aboutTitle=DE10lite about
 * aboutText=This is a virtual development board for the DE10lite
 * width=97.54_mm
 * height=80.01_mm 
 * background=0,75,128
 * 
 * @subsection vdbPeripheralIniFile Peripheral data in ini file
 * 
 * A INI file can have multiple [peripheral] sections, which define the peripherals. The following
 * properties are generally available for all peripherals. For a specific component, see the components
 * documentation.
 * 
 * | Property name | property type | Mandatory | Description | Default value |
 * |---------------|---------------|-------------|---------------|--------------|
 * | type | String | yes | The type of the component | No default value, see RoaLogic::GUI::eVdbComponentType for all possible types |
 * | scope | String | yes* | The verilated scope of the component, note that the scope must be packed with "" | No default value |
 * | id | Integer | yes* | The id of the component |id= 0 |
 * | xoffset | Dimension | yes | The x offset from the origin in mm or inch | xoffset=25_mm |
 * | yoffset | Dimension | yes | The y offset from the origin in mm or inch | yoffset=48_mm |
 * | angle | Dimension | no | The angle of the component in degrees | angle=0 |
 *
 * * The scope and ID is only mandatory for peripherals that communicate with the verilated design.
 * 
 * @subsubsection example Example peripheral section
 * [peripheral]
 * 
 * @subsection GUI_framework GUI framework
 * The board setup is designed in such way that it is possible to use different GUI frameworks. @todo: give more information about
 * the possible GUI frameworks and how to add a new one.
 * 
 * Supported GUI frameworks:
 * - wxWidgets
 * 
 */

//for std::unique_ptr
#include <memory>

#include "iniparser.hpp"
#include "subject.hpp"
#include "observer.hpp"

// Include all different components
#include "vdbVGAMonitor.hpp"
#include "vdbLED.hpp"
#include "vdb7SegmentDisplay.hpp"
#include "wxWidgetsVdbIC.hpp" //!@TODO: Adjust the file structure to have a seperate vdbIC file
#include "wxWidgetsVdbHeader.hpp" //!@TODO: Adjust the file structure to have a seperate vdbIC file

// Include all different GUI types
#include "wxWidgetsImplementation.hpp"

#ifndef BOARD_HPP
#define BOARD_HPP

namespace RoaLogic
{
    using namespace parser;
    using namespace observer;
namespace GUI
{

    /**
     * @brief 
     * 
     */
    class cGuiBoard
    {
        private:
        static constexpr std::string _cBoardString = "board";
        static constexpr std::string _cTypeString = "type";
        
        cIniparser _iniParser;
        std::thread _guiThread;
        cVirtualDemoBoard* _demoBoard = nullptr;

        int _argc;
        char** _argv;

        std::vector<cVdbLed*> _ledInstances;
        std::vector<cVdb7SegmentDisplay*> _7segInstances;
        std::vector<cVdbVGAMonitor*> _vgaInstances;

        distancePoint parseOffset(map<string,string>& values);
        sRGBColor parseColor(std::string valueName, map<string,string>& values);
        double parseAngle(map<string,string>& values);

        std::string parseScope(map<string,string>& values);
        int parseId(map<string,string>& values);

        bool setupBoard(int argc, char** argv, map<string,string> &values);

        void setupPeripheral(map<string,string>& values);

        cVdbLed* createLed(map<string,string>& values);
        cVdb7SegmentDisplay* create7Seg(map<string,string>& values);
        cVdbVGAMonitor* createVGA(map<string,string>& values);

        public:
        cGuiBoard(int argc, char** argv, std::string fileName);
        ~cGuiBoard();

        bool initialize();
        void registerObserver(cObserver* aObserver){if(_demoBoard) _demoBoard->registerObserver(aObserver);};
        void removeObserver(cObserver* aObserver){if(_demoBoard) _demoBoard->removeObserver(aObserver);};
    };
}
}

#endif