/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    altsyncram Verilator C++ wrapper                             //
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


#include "altsyncram.hpp"
#include "mifparser.hpp"


using namespace RoaLogic;
using namespace parser;

//create altsyncram map
//std::map<std::string,cAltsyncram> mapAltsyncram;


//TODO: move into 'common' library file
/**
 * @brief Split string based on delimiter
 * @details Split string str based on delimiter delim
 *
 * @return split string as a std::vector<std:string> 
 */
std::vector<std::string> split(const std::string& str, const char delim)
{
    std::stringstream input_stream { str };
    std::vector<std::string> string_list;
    std::string current_string;

    while( std::getline( input_stream, current_string, delim ) )
            string_list.emplace_back( std::move( current_string ) );

    return string_list;
}


/**
 * @brief altsyncram DPI-C callback
 * @details This function can be called from inside Verilog and acts as a callback back into C++
 * 
 */
void altsyncram_registerInstance()
{
    //get instance scope
    svScope scope = svGetScope();

    //get hierarchical name
    const char* scopeName = svGetNameFromScope(scope);

    //report progress
    INFO << "Registered altsyncram " << scopeName << "\n";
}


/**
 * @brief Initialize altsyncram
 * @details Initialize altsyncram instance <instance> with contents from <fileName>
 * 
 */
int altsyncram_initialize(std::string instance, std::string fileName)
{
    INFO << "Initializing " << instance << " from " << fileName << "\n";

    //first get verilator-scope from instance name
    svScope scope = svGetScopeFromName(instance.c_str());

    //does scope exists?
    if (scope == nullptr)
    {
        WARNING << "Instance " << instance << " not found\n";
        return -1;
    }


    //Which file-type is this?
    //Look at extension to select
    std::filesystem::path filepath = fileName;
    std::string extension = filepath.extension();

    if (extension.compare(".hex") == 0 || extension.compare(".ihex") ==0)
        return altsyncram_initializeFromHex(scope, fileName);

    if (extension.compare(".mif") == 0)
        return altsyncram_initializeFromMif(scope, fileName);

    if (extension.compare(".v") == 0 || extension.compare(".ver") == 0)
        return altsyncram_initializeFromVerilog(scope, fileName);


    //unknown file type
    WARNING << "Unknown file extension for " << fileName << "\n";
    return -1;
}


/**
 * @brief Initialize altsyncram from hex file
 * @details Initialize altsyncram instance <instance> with contents from intel-hex file <fileName>
 * 
 */
int altsyncram_initializeFromHex(svScope scope, std::string fileName)
{
    WARNING << "initialize from hex not implemented yet\n";
    return -1;
}


/**
 * @brief Initialize altsyncram from mif file
 * @details Initialize altsyncram instance <instance> with contents from altera-mif file <fileName>
 * 
 */
int altsyncram_initializeFromMif(svScope scope, std::string fileName)
{
 //   cParseMif * parser;
    mifparser* parser;

    try
    {
        parser = new mifparser(fileName);
    }
    catch (const ParserException& e)
    {
      ERROR << "Parse error: " << e.what() << "\n";
      return -1;
    }

    //Set scope
    svSetScope(scope);

    //Iterate over the memory array
    svBitVecVal svData;
    for (parseData_t tuple : parser->data())
    {
      svData = tuple.data;
      altsyncram_setMemory(tuple.address, &svData);
    }

std::cout << "returning to main\n";
    return 0;
}


/**
 * @brief Initialize altsyncram from verilog file
 * @details Initialize altsyncram instance <instance> with contents from verilog file <fileName>
 * 
 */
int altsyncram_initializeFromVerilog(svScope scope, std::string fileName)
{
    WARNING << "initialize from verilog not implemented yet\n";
    return -1;
}

