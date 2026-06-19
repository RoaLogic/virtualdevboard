/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Generic GUI board source file                                //
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

#include "board.hpp"
#include "stringHelper.hpp"

namespace RoaLogic
{
    using namespace common;
namespace GUI
{
    /**
     * @brief Construct a new cGuiBoard object
     * 
     * @param[in] argc          Program parameter count, where specific GUI parameters can be given
     * @param[in] argv          Program parameters, where specific GUI parameters can be given
     * @param[in] fileName      Name of the *.ini file to use
     */
    cGuiBoard::cGuiBoard(cTestBenchVirtualDevBoard* testbench, int argc, char** argv, std::string fileName) :
        _testbench(testbench),
        _argc(argc),
        _argv(argv),
        _iniParser(fileName)
    {

    }

    /**
     * @brief Destroy the Gui Board object
     * @details This destructor will close the GUI.
     * It first waits until the GUI thread is finished 
     * and then closes the GUI.
     * 
     */
    cGuiBoard::~cGuiBoard()
    {
        // Close GUI
        if(_guiThread.joinable()) _guiThread.join();

        for(auto& led : _ledInstances) delete led;
        for (auto&  seg7 : _7segInstances) delete seg7;
        for (auto&  vga : _vgaInstances) delete vga;
    }

    /**
     * @brief Initialize the GUI board
     * @details This function initializes the board on the GUI.
     * 
     * It first opens and parses the *.ini file. Following this it
     * get's the data and starts by setting up the board. When 
     * a board section is found, it will try to setup the board. 
     * 
     * The last step is then to add all peripherals to the board,
     * which is done by traversing the map and setting up the
     * peripherals.
     * 
     * @return true     Board setup succesfull
     * @return false    Board setup failed
     */
    bool cGuiBoard::initialize()
    {
        //Open and parse the INI file
        _iniParser.open();
        _iniParser.parse();
      
        iniData data = _iniParser.data();

        // Check if there is a board section, if this is not the case
        // we can't continue, so return 0
        if(data.find(_cBoardString) != data.end())
        {
            INFO << "Board section found\n";

            // Setup the board, if this fails return 0
            if(!setupBoard(_argc, _argv, data[_cBoardString]))
            {
                ERROR << "Error setting up board\n";
                return 0;
            }

            // Traverse all peripherals and try to set those up
            // This shall not fail, since we then just don't show the periperhal
            for (auto& [key, value] : data)
            {
                if(key.compare(0, _cBoardString.size(), _cBoardString) != 0)
                {
                    INFO << "Found periperhal with name: " << key << "\n";

                    setupPeripheral(value);
                }
            }
        }
        else
        {
            ERROR << "No board section found\n";
            return 0;
        }

        return 1;
    }

    /**
     * @brief Create the virtual development board GUI layout
     * @details This function creates the virtual development board GUI layout.
     * 
     * It traverses the given map and searches the corresponding elements for the GUI.
     * All options that are found are used to setup the screen, all others will use the default values,
     * check the documentation for the default values.
     * 
     * @param[in] values  The map with the values for the board
     * @return true   Board setup succesfull
     * @return false  Board setup failed
     */
    bool cGuiBoard::setupBoard(int argc, char** argv, map<string,string> &values)
    {
        static constexpr std::string _cBackGroundColorName = "background";
        std::string applicationName = "Virtual Demo Board";
        std::string aboutTitle = "About Virtual Demo Board";
        std::string aboutText = "This is a virtual demo board";
        distanceSize minimalScreenSize = {800_mm, 600_mm};
        sRGBColor backgroundColor = {0, 0, 0};

        INFO << "Setup board \n";

        if(values.find("name") != values.end())
        {
            applicationName = values["name"] ;
            INFO << "Found application name: " << applicationName << "\n";
        } 
        else return false;

        if(values.find("aboutTitle") != values.end())
        {
            aboutTitle = values["aboutTitle"];
            INFO << "Found about title: " << aboutTitle << "\n";
        } 
        if(values.find("aboutText") != values.end())
        { 
            aboutText = values["aboutText"];
            INFO << "Found about text: " << aboutText << "\n";
        } 

        if(values.find("width") != values.end() && values.find("height") != values.end())
        {
            INFO << "Found width: " << values["width"] << "\n";
            INFO << "Found heigth: " << values["height"] << "\n";
            minimalScreenSize.width = convertStringToDistance(split(values["width"], '_'));
            minimalScreenSize.height = convertStringToDistance(split(values["height"], '_'));

            if(minimalScreenSize.width == 0 || minimalScreenSize.height == 0)
            {
                ERROR << "Given board size is incorrect! \n";
                return false;
            }
        }

        backgroundColor = parseColor(_cBackGroundColorName, values);

        INFO << "Creating board\n";
        //!@todo: Add option to use different GUI framework.
        _demoBoard = new cVirtualDemoBoard();

        INFO << "Creating thread\n";
        // Create GUI and start it on different thread
        _guiThread = std::thread(&cVirtualDemoBoard::init, _demoBoard, _argc, _argv, applicationName, aboutTitle, aboutText, minimalScreenSize, backgroundColor);
        this_thread::sleep_for(chrono::milliseconds(1000));// Give the GUI time to start, it has to be active before we can sent events to it
        return true;
    }

    /**
     * @brief Setup a periperhal
     * @details This function creates a periperhal on the board.
     * It uses the map from the INI file to setup the periperhal according to the given values.
     * 
     * There are two mainly different types of periperhals:
     * - Peripheral that communicates with the verilated design
     * - Peripheral that is only for the GUI
     * 
     * Both have different setups, and with that different values. This can be checked 
     * through the scope and id field.
     * 
     * @param[in] values    The values according to the periperhal
     */
    void cGuiBoard::setupPeripheral(map<string,string>& values)
    {
        eVdbComponentType componentType = eVdbComponentType::vdbNone;

        for (auto& [key, perValue] : values)
        {
            perValue.pop_back();
            INFO << "Peripheral Key: " << key << " Data: " << perValue << "\n";
        }

        if(values.find(_cTypeString) != values.end())
        {
            for (size_t i = 0; i < cVdbComponentTypeLookUpSize; i++)
            {
                //if(values[_cTypeString].compare(0, cVdbComponentTypeLookUp[i].typeName.size(), cVdbComponentTypeLookUp[i].typeName) == 0)
                if((values[_cTypeString].compare(cVdbComponentTypeLookUp[i].typeName)) == 0)
                {
                    componentType = cVdbComponentTypeLookUp[i].type;
                    INFO << "Found peripheral type: " << values[_cTypeString] << "\n";
                    break;
                }
            }

            switch (componentType)
            {
                case eVdbComponentType::vdbLed:
                {
                    cVdbLed* led = createLed(values);
                    if(led != nullptr)
                    {
                        _ledInstances.push_back(led);

                        eVdbLedType ledType = cVdbLed::getLedType(values[cVdbLed::_cLedType]);
                        sRGBColor ledColour = parseColor(cVdbLed::_cLedColourName, values);

                        _demoBoard->addVdbComponent(componentType, led, parseOffset(values), new sVdbLedInformation(ledType, ledColour));
                    }
                    break;
                }
                case eVdbComponentType::vdb7SegmentDisplay:
                {
                    cVdb7SegmentDisplay* seg7 = create7Seg(values);
                    if(seg7 != nullptr)
                    {
                        _7segInstances.push_back(seg7);

                        eVdb7SegType ledType = cVdb7SegmentDisplay::get7SegType(values[cVdb7SegmentDisplay::_c7SegType]);
                        sRGBColor ledColour = parseColor(cVdb7SegmentDisplay::_c7SegColourName, values);

                        _demoBoard->addVdbComponent(componentType, seg7, parseOffset(values), new sVdb7SegInformation(ledType, ledColour));
                    }
                    break;
                }
                case eVdbComponentType::vdbVGA:
                {
                    cVdbVGAMonitor* vga = createVGA(values);
                    if(vga != nullptr)
                    {
                        _vgaInstances.push_back(vga);

                        _demoBoard->addVdbComponent(eVdbComponentType::vdbConnector, nullptr, parseOffset(values), 
                                                    new sVdbConnectorInformation(eVdbConnectorType::DSUB, 30.8_mm, 16.2_mm,"VGA"), parseAngle(values));

                        _demoBoard->addVdbComponent(eVdbComponentType::vdbVGA, vga, distancePoint(50.0_mm, 100.0_mm), nullptr);
                    }

                    break;
                }
                case eVdbComponentType::vdbIC:
                {
                    // At this moment the IC's don't support any communication with the verilated model, so there is no need to
                    // create this. Same counts for the pointer, there is no need to store this.
                    _demoBoard->addVdbComponent(componentType, 
                                                nullptr, 
                                                parseOffset(values), 
                                                new sVdbICInformation(
                                                    convertStringToDistance(split(values[cWXVdbIC::cICWidth], '_')), 
                                                    convertStringToDistance(split(values[cWXVdbIC::cICHeight], '_')),
                                                    values[cWXVdbIC::cICLabel]));
                    break;
                }
                case eVdbComponentType::vdbHeader:
                {
                    _demoBoard->addVdbComponent(componentType,
                                                nullptr,
                                                parseOffset(values),
                                                new sVdbHeaderInformation(
                                                    cWXVdbHeader::getHeaderType(values[cWXVdbHeader::cHeaderTypeText]),
                                                    stoi(values[cWXVdbHeader::cHeaderRowsText]),
                                                    stoi(values[cWXVdbHeader::cHeaderColumsText]),
                                                    convertStringToDistance(split(values[cWXVdbHeader::cHeaderPitchText], '_'))));

                    break;
                }
                case eVdbComponentType::vdbNone:
                    [[fallthrough]];
                default:
                    WARNING << "Peripheral type not recognized\n";
                    return;
                    break;
            }
        }
        else 
        {
            ERROR << "No type found for peripheral\n";
            return;
        }
    }

    cVdbLed* cGuiBoard::createLed(map<string,string>& values)
    {
        std::string scope = parseScope(values);
        int id = parseId(values);

        if(scope.size() != 0)
        {
            return new cVdbLed(scope, id);
        }

        WARNING << "No scope or id found for LED\n";
        return nullptr;
    }

    cVdb7SegmentDisplay* cGuiBoard::create7Seg(map<string,string>& values)
    {
        std::string scope = parseScope(values);
        int id = parseId(values);

        if(scope.size() != 0)
        {
            return new cVdb7SegmentDisplay(scope, id);
        }

        WARNING << "No scope or id found for 7 segment\n";
        return nullptr;
    }

    cVdbVGAMonitor* cGuiBoard::createVGA(map<string,string>& values)
    {
        std::string scope = parseScope(values);

        if(scope.size() != 0)
        {
            // time interface, coming from the testbench
            // Clock instance, coming from the testbench
            // Framebuffer, belonging to the VGA instance
            return new cVdbVGAMonitor(scope, _testbench, _testbench->getVGAClock(), _testbench->getVGAFrameBuffer());
        }

        WARNING << "No scope or id found for VGA\n";
        return nullptr;
    }

    /**
     * @brief Parse the scope field
     * @details This function parses the scope field from the given values.
     * If it is not set in the fields it will return an empty string.
     * 
     * @param[in] values    The values to parse
     * @return std::string  The given scope of the peripheral, or empty when failed
     */
    std::string cGuiBoard::parseScope(map<string,string>& values)
    {
        static constexpr std::string _cScopeString = "scope";

        if(values.find(_cScopeString) != values.end())
        {
            std::string scopeString = values[_cScopeString];
            //scopeString.pop_back();
            return scopeString;
        }

        return "";
    }

    /**
     * @brief Parse the ID field
     * @details This function parses the ID field from the given values.
     * If it is not set in the fields it will return 0.
     * 
     * @param[in] values    The values to parse
     * @return int          The given ID of the peripheral 
     */
    int cGuiBoard::parseId(map<string,string>& values)
    {
        static constexpr std::string _cIDString = "id";

        if(values.find(_cIDString) != values.end())
        {
            return stoi(values[_cIDString]);
        }

        return 0;
    }

    /**
     * @brief Parse a given color field
     * @details This function parses a given color field from the given values.
     * If it is not set in the fields it will return a black color.
     * 
     * @param[in] valueName Name of the color field
     * @param[in] values    The values to parse
     * @return sRGBColor    The given color of the peripheral
     */
    sRGBColor cGuiBoard::parseColor(std::string valueName, map<string,string>& values)
    {
        sRGBColor color = {0, 0, 0};

        if(values.find(valueName) != values.end()) 
        {
            std::vector colours = split(values[valueName], ',');

            if(colours.size() == 3)
            {
                color.red   = std::stoi(colours.at(0));
                color.green = std::stoi(colours.at(1));
                color.blue  = std::stoi(colours.at(2));
            }
            else
            {
                WARNING << "Given colours is incorrect! \n";
            }
        }

        return color;
    }

    /**
     * @brief Parse a given offset field
     * @details This function parses a given offset field from the given values.
     * If it is not set in the fields it will return a 0 offset.
     * 
     * @param[in] values        The values to parse
     * @return distancePoint    The given offset of the peripheral
     */
    distancePoint cGuiBoard::parseOffset(map<string,string>& values)
    {
        static constexpr std::string _cXOffsetName = "xoffset";
        static constexpr std::string _cYOffsetName = "yoffset";

        distancePoint offset = {0_mm, 0_mm};

        if(values.find(_cXOffsetName) != values.end() && values.find(_cYOffsetName) != values.end())
        {
            offset.x = convertStringToDistance(split(values[_cXOffsetName], '_'));
            offset.y = convertStringToDistance(split(values[_cYOffsetName], '_'));
        }

        return offset;
    }

    double cGuiBoard::parseAngle(map<string,string>& values)
    {
        static constexpr std::string _cAngleName = "angle";

        if(values.find(_cAngleName) != values.end())
        {
            return std::stod(values[_cAngleName]);
        }

        return 0;
    }

}}
