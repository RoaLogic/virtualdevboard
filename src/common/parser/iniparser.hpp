/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Parser for Altera MIF file                                   //
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

#ifndef ROA_INIPARSER
#define ROA_INIPARSER

//include parser
#include "parser.hpp"

#include "inilexer.hpp"

#include <map>
#include <string>

#define DEBUG_INI_PARSER

namespace RoaLogic
{
namespace parser
{

    using iniData = std::map<std::string, std::map<std::string, std::string>>;

    /**
     * @class CIniParser
     * @author Bjorn Schouteten
     * @brief This class parses *.ini files
     * @details Within this class a *.ini file is parsed and stored in a map.
     * 
     * It traverses the file line for line and stores the data into a map.
     * 
     * Following values are specific in a *.ini file.
     * - [<section name>] is used for a section with a specific name
     * - [.<subsection name>] is used for a subsection with a specific name
     *   [<section name>.<subsection name>] can also be used for a subsection
     * - key=value , is used for a key value pair
     * - # or ; is used for a comment
     * 
     * This parses does not support duplicate section names
     * Every key value pair must be on a single line
     * 
     */
    class cIniparser : public parserBase<cIniLexer, iniData>
    {
        private:
        std::string  _fileName;
        std::ifstream _file;
        std::string  _lastSection;

        public:
        cIniparser(){};
        cIniparser(std::string fileName) :
            _fileName(fileName)
        {

        }

        ~cIniparser()
        {

        }

        void open(std::string fileName)
        {
            _fileName = fileName;
            open();
        }

        void open()
        {
            if(_fileName.size() == 0)
            {
                throw ParserException("No filename given");
            }
            else if (_file.is_open())
            {
                throw ParserException("File: " + _fileName + " already open");
            }
            
            try
            {
                _file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                _file.open(_fileName);
            }
            catch(const std::ifstream::failure& e)
            {
                std::string error="Failed to open file: ";
                error.append(_fileName);
                throw ParserException(error);
            }
        }

        bool parse()
        {
            #ifdef DEBUG_INI_PARSER
            INFO << "Parsing INI file: " << _fileName << "\n";
            #endif
            bool endOfFile = false;

            lexer = new cIniLexer(&_file);

            while(!endOfFile)
            {
                CIniLine line = lexer->getLine();

                switch (line.getType())
                {
                    case EIniLineType::section:
                        if (line.getSection().length() > 0)
                        {
                            dataBuffer.insert(std::make_pair(line.getSection(), std::map<std::string, std::string>()));
                            _lastSection = line.getSection();
                        }
                        break;

                    case EIniLineType::subsection:
                        //parseKey();
                        break;

                    case EIniLineType::keyvalue:
                        if (line.getKey().length() > 0)
                        {
                            dataBuffer[_lastSection].insert(std::make_pair(line.getKey(), line.getValue()));
                        }
                        break;

                    case EIniLineType::comment:
                        break;

                    case EIniLineType::endOfFile:
                        endOfFile = true;
                        break;
                    case EIniLineType::unknown:
                    default:
                        endOfFile = true;
                        throw ParserException("Unknown entry in INI file");
                }
            }

            delete lexer;
            return true;
        }
    };
}}

#endif