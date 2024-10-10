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

#include "de10lite.hpp"

#include <noValueOption.hpp>
#include <valueOption.hpp>


//Setup namespaces
using namespace RoaLogic;
using namespace common;
using namespace testbench;
using namespace tasks;


//Create program options variable
cProgramOptions programOptions;

//Specify Program options
cNoValueOption            optHelp      ("h", "help",     "Show this help and exit", false);
cNoValueOption            optTrace     ("t", "trace",    "Enable waveform dump", false);
cValueOption<std::string> optWaveFile  ("",  "wave",     "Waveform file");
cValueOption<std::string> optLog       ("l", "log",      "Set the path for the log file");
cValueOption<uint8_t>     optLogLvl    ("",  "level",    "Log level; start loggin from 0=Debug, 1=Log, 2=Info, 3=Warning, 4=Error, 5=Fatal");
cValueOption<std::string> optInitFile  ("",  "initfile", "Initialisation file for the on-chip RAM");

//type definitions for program options and logger (see bottom of the file)
int setupProgramOptions(int argc, char** argv);
void setupLogger(void);


//Main routine
int main(int argc, char** argv)
{
  bool enableTrace = false;

  //first setup the program options
  if (setupProgramOptions(argc,argv))
  {
    return 0;
  }

  //next setup the logger
  setupLogger();

  //Trace enabled?
  enableTrace = optTrace.isSet();

  //Setup testbench
  std::unique_ptr<VerilatedContext> contextp(new VerilatedContext);

  //parse Verilator options
  contextp->commandArgs(argc,argv);

  //create testbench
  cDE10Lite* de10lite = new cDE10Lite(contextp.get(), enableTrace);

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

  //Initialize RAMs
  if (optInitFile.isSet())
  {
    //split string at ':' to get <instance> : <initfile>
    std::vector<string> split_string = split(optInitFile.value(), ':');

    //initialise altsyncram instance
    altsyncram_initialize(split_string[0], split_string[1]);
  }
std::cout << "back in main\n";

  //Run the testbench
  de10lite->run();

  //close testbench
  delete de10lite;

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
