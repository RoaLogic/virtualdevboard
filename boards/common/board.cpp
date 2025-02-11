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

namespace RoaLogic
{
namespace GUI
{
    cGuiBoard::cGuiBoard(int argc, char** argv, std::string fileName) :
        _argc(argc),
        _argv(argv),
        _iniParser(fileName)
    {

    }

    cGuiBoard::~cGuiBoard()
    {
        // Close GUI
        if(threadGUI.joinable())
        {
            threadGUI.join();
        }
    }

    /**
     * @brief Initialize the GUI board
     * @details This function initializes the board on the GUI.
     * 
     * It first tries to open the *.ini file.
     */
    bool cGuiBoard::initialize()
    {
        //parse the INI file
        iniParser.open();
        iniParser.parse();
      
        iniData data = iniParser.data();

        if(data.find(_cBoardString) != data.end())
        {
            INFO << "Board section found\n";

            if(!setupBoard(argc, argv, data[_cBoardString]))
            {
                ERROR << "Error setting up board\n";
                return 0;
            }

            //setupPeripherals(data);
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
    bool setupBoard(int argc, char** argv, map<string,string> &values)
    {
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

        if(values.find("background") != values.end()) 
        {
            std::vector colours = split(values["background"], ',');

            if(colours.size() == 3)
            {
                backgroundColor.red   = std::stoi(colours.at(0));
                backgroundColor.green = std::stoi(colours.at(1));
                backgroundColor.blue  = std::stoi(colours.at(2));
            }
            else
            {
                WARNING << "Given background colours is incorrect! \n";
            }
        }

        INFO << "Creating board\n";
        //!@todo: Add option to use different GUI framework.
        demoBoard = new cVirtualDemoBoard();

        INFO << "Creating thread\n";
        // Create GUI and start it on different thread
        threadGUI = std::thread(&cVirtualDemoBoard::init, demoBoard, argc, argv, applicationName, aboutTitle, aboutText, minimalScreenSize, backgroundColor);
        this_thread::sleep_for(chrono::milliseconds(1000));// Give the GUI time to start, it has to be active before we can sent events to it
        return true;
    }

    bool setupPeripherals(iniData data)
    {
        for (auto const& peripheral : data)
        {
            if(peripheral.first.compare(0, 5, "board") != 0)
            {

            }
        }
        
    }

}}
