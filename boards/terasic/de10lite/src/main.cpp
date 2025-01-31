/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    DE10-Lite Verilator main C++ file                            //
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
 * @mainpage Virtual development board
 * 
 * This is the software documentation for the virtual development board. The virtual development board 
 * is a framework for creating virtual development boards. A virtual development board is an 
 * FPGA/ASIC development board that is entirely emulated on a PC. The FPGA/ASIC logic is converted 
 * into cycle accurate C++ models using Verilator. The peripherals (other components) on the board 
 * are emulated using graphical components (e.g. LEDs, 7-segment Display, VGA), 
 * abstracted (e.g. SDRAM memories), or by accessing the host PCs hardware and OS resources directly 
 * (e.g. UART, ethernet, ...).
 * 
 * @section overview Overview
 * 
 * The virtual development board consists out of a few components, the system verilog design, the core
 * of the virtual development board and virtual development board components. To connect a system verilog
 * design with the virtual development board components a wrapper file is needed. Depending on the used
 * components in the system verilog design, the corresponding system verilog components must be created and 
 * linked in the wrapper file. The virtual development board components are the graphical representation of
 * the peripherals on the board. These components are shown in the GUI and can be interacted with. 
 * 
 * During the verilating step, the system verilog design is converted into a C++ model. This model is then 
 * created and connected with the virtual development board components. Which is all done in the core of the
 * virtual development board. The core is the main class that connects the system verilog design with the 
 * C++ testbench core, which runs the simulation. In the case that the UI is enabled, there also is a GUI
 * core, which is the main class that connects the virtual development board components with the GUI. Within 
 * the core the virtual development board components are created and connected with the C++ model. Depending
 * on the component type, it either uses the system verilog component part of connects directly with the 
 * C++ model through the testbench core.
 * 
 * For more information about the virtual development board components, see @ref vdbComponent_1.
 * For more information about how the setup of the system verilog design is and how to change the 
 * design, see @todo: Add link and documentation to the system verilog design.
 * 
 * @image html overview.png
 * 
 * 
 * See below for a more detailed description of the core.
 * 
 * @section boardLayout Board layout
 * 
 * A board is defined as a set of components (peripherals) that are shown in the GUI and connect 
 * with the Verilated model. The board layout is defined through a *.ini file, which is parsed by the
 * application. The *.ini file defines the components, their properties, and their connections.
 * 
 * All dimensions must be given in mm or inch, the application will convert this accordingly. Each value
 * shall be given as a string, where after the value a underscore is used followed by the unit. For 
 * example: 12.5_mm or 6.7_inch.
 * 
 * Must haves in the INI file:
 * The INI file must have a [board] section, which defines the board. 
 * Following properties must be added
 *  - name:   The name of the board
 * 
 * Following properties can be added
 *  - aboutTitle:      The about title of the board, default "About Virtual Demo Board"
 *  - aoutText:        The about information of the board, default "This is a virtual demo board"
 *  - width:           The width of the board in mm or inch, default 800, note that if width is given, height must also be given
 *  - height:          The height of the board in mm or inch, default 600, note that if height is given, width must also be given
 *  - background:      The background color of the board, default white
 * 
 * @section newBoard Creating a new board
 * 
 * Every board must have a system verilog wrapper file, which is processed through Verilator. Next to
 * the wrapper file also a *.ini must be created. 
 */

#include "de10lite.hpp"

#include <noValueOption.hpp>
#include <valueOption.hpp>

#include "wxWidgetsImplementation.hpp"

#include "iniparser.hpp"

//Setup namespaces
using namespace RoaLogic;
using namespace common;
using namespace testbench;
using namespace tasks;
using namespace parser;
using namespace dimensions;

//Create program options variable
cProgramOptions programOptions;

//Specify Program options
cNoValueOption            optHelp      ("h", "help",     "Show this help and exit", false);
cNoValueOption            optTrace     ("t", "trace",    "Enable waveform dump", false);
cValueOption<std::string> optWaveFile  ("",  "wave",     "Waveform file");
cValueOption<std::string> optLog       ("l", "log",      "Set the path for the log file");
cValueOption<uint8_t>     optLogLvl    ("",  "level",    "Log level; start loggin from 0=Debug, 1=Log, 2=Info, 3=Warning, 4=Error, 5=Fatal");
cValueOption<std::string> optUIFile    ("u", "uilayout", "UI layout *.ini file");
cValueOption<std::string> optInitFile  ("",  "initfile", "Initialisation file for the on-chip RAM");
cValueOption<uint32_t>    optNoGui     ("",  "nogui",    "Start system without a GUI, possible to add in simulation time in milliseconds");

//type definitions for program options and logger (see bottom of the file)
int setupProgramOptions(int argc, char** argv);
void setupLogger(void);
void setupMemories(void);
bool setupBoard(int argc, char** argv, map<string,string> &values);

cVirtualDemoBoard* demoBoard = nullptr;
std::thread threadGUI;
cIniparser iniParser;

//Main routine
int main(int argc, char** argv)
{
  bool enableTrace = false;
  bool rerun = false;

  //first setup the program options
  if (setupProgramOptions(argc,argv)) { return 1; }

  setupLogger(); //next setup the logger  
  enableTrace = optTrace.isSet(); //Trace enabled?

  std::unique_ptr<VerilatedContext> contextp(new VerilatedContext); //Setup testbench
  contextp->commandArgs(argc, argv); //parse eventual Verilator options

  // See if we run with a GUI or not
  if(!optNoGui.isSet() )
  {
    // Process the option file
    if(optUIFile.isSet())
    {
      //parse the INI file
      iniParser.open(optUIFile.value());
      iniParser.parse();
      
      iniData data = iniParser.data();

      if(data.find("board") != data.end())
      {
          INFO << "Board section found\n";

          if(!setupBoard(argc, argv, data["board"]))
          {
              ERROR << "Error setting up board\n";
              return 1;
          }
      }
      else
      {
        ERROR << "No board section found\n";
        return 1;
      }      
    }
    else
    {
      ERROR << "Gui selected but no GUI file given!\n";
      return 1;
    }
  }  

  do
  {
    rerun = false;
    //create testbench
    cDE10Lite* de10lite = new cDE10Lite(contextp.get(), enableTrace, demoBoard);

    setupMemories();

    //Open waveform dump file if enabled
    if (enableTrace)
    {
      if (optWaveFile.isSet())
      {
        de10lite->opentrace(optWaveFile.value());
      }
      else
      {
        de10lite->opentrace("waves.vcd");
      }
    }

    //Run the testbench
    if(optNoGui.isSet())
    {
      de10lite->run(optNoGui.value());
    }
    else
    {
      if(de10lite->run() == eRunState::restart)
      {
        rerun = true;
      }
    }

    //close testbench
    delete de10lite;
  } while (rerun);
  
  // Close GUI
  if(threadGUI.joinable())
  {
    threadGUI.join();
  }

  //close log
  cLog::getInstance()->close();

  //exit program
  return 0;
}

/**
 * @brief Function to setup the program options for this main file
 * @details this function sets the program options, parses the given
 * parameters and then checks some initial program options
 * 
 * @param argc 
 * @param argv 
 * @return 0  continue execution
 * @return >0 Stop program 
 */
int setupProgramOptions(int argc, char** argv)
{
    programOptions.add(&optHelp);
    programOptions.add(&optTrace);
    programOptions.add(&optWaveFile);
    programOptions.add(&optLog);
    programOptions.add(&optLogLvl);
    programOptions.add(&optInitFile);
    programOptions.add(&optNoGui);
    programOptions.add(&optUIFile);

    programOptions.parse(argc, argv);

    if(optHelp.isSet())
    {
        programOptions.printKnownOptions();
        return 1;
    }

    return 0;
}

/**
 * @brief Function to setup the logger
 * @details This function sets the logger up.
 * It checks if the level option is set and gets the value,
 * if it's not set it will default to INFO.
 * 
 * When the optLog is set, the file path will be selected. 
 * In other cases it will use the terminal for output.
 * 
 * @attention This function must have the optLog and optLogLvl 
 * in the system.
 */
void setupLogger(void)
{
    int logLvl = 0;

    if (optLogLvl.isSet()) 
    {
        logLvl = optLogLvl.value();
    }
    else
    {
        logLvl = 2;
    }    

    if(optLog.isSet())
    {
        cLog::getInstance()->init(logLvl, optLog.value());
    }
    else
    {
        cLog::getInstance()->init(logLvl, "");
    }

    INFO << "Started log with level: " << logLvl << "\n";
}

/**
 * @brief Fucntion to setup the memories of the system
 * @details This function will setup the memories of the system
 * It checks the given command line options and initializes the 
 * memories accordingly.
 * 
 * Currently initializes:
 * - altsyncram
 * 
 */
void setupMemories(void)
{
    //Initialize RAMs
    if (optInitFile.isSet())
    {
        if(optInitFile.value().find(':') != optInitFile.value().npos)
        {
            //split string at ':' to get <instance> : <initfile>
            std::vector<string> init_string = split(optInitFile.value(), ':');

            //initialise altsyncram instance
            altsyncram_initialize(init_string[0], init_string[1]);
        }
        else
        {
            WARNING << "Wrong init file passed, missing delimiter\n";
        }
    }
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
    distanceSize minimalScreenSize = {800, 600};
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

    //if(values.find("background") != values.end()) backgroundColor = sRGBColor(values["background"]->second);

    INFO << "Creating board\n";
    //!@todo: Add option to use different GUI framework.
    demoBoard = new cVirtualDemoBoard();

    INFO << "Creating thread\n";
    // Create GUI and start it on different thread
    threadGUI = std::thread(&cVirtualDemoBoard::init, demoBoard, argc, argv, applicationName, aboutTitle, aboutText, minimalScreenSize, backgroundColor);
    this_thread::sleep_for(chrono::milliseconds(1000));// Give the GUI time to start, it has to be active before we can sent events to it
    return true;
}