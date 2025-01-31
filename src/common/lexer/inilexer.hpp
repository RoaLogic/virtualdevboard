/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Lexer for INI files                                          //
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


#ifndef ROA_INILEXER
#define ROA_INILEXER

//include base class
#include "lexer.hpp"
#include <string>

namespace RoaLogic
{
namespace lexer
{
    /**
     * @brief Enumeration for the different types of lines in an INI file
     * 
     */
    enum class EIniLineType
    {
        unknown,        //!< Unknown line element
        section,        //!< Found a section
        subsection,     //!< Found a subsection
        keyvalue,       //!< Found a key value pair
        comment,        //!< Found a comment
        empty,          //!< Empty line
        endOfFile       //!< End of file detected
    };

    class CIniLine
    {
        friend class cIniLexer;
        private:
        EIniLineType _type = EIniLineType::unknown;
        std::string _section;
        std::string _subsection;
        std::string _key;
        std::string _value;

        public:
        CIniLine(){}

        EIniLineType getType(void) { return _type; }
        std::string getSection(void) { return _section; }
        std::string getSubsection(void) { return _subsection; }
        std::string getKey(void) { return _key; }
        std::string getValue(void) { return _value; }
    };

    /**
     * @class ini lexer
     * @author Bjorn Schouteten
     * @brief Lexer for ini files
     * 
     */
    class cIniLexer : public filelexer<std::string>
    {
        public:
            /**
             * @brief Constructor
             */
            cIniLexer(std::istream* istream) : filelexer(istream) {}

            /**
             * @brief Destructor
             */
            ~cIniLexer() {}

            CIniLine getLine(void)
            {
                CIniLine iniLine;
                bool endOfLine = false;
                std::string line;

                if(istream->peek() == EOF)
                {
                    iniLine._type = EIniLineType::endOfFile;
                    return iniLine;
                }

                std::getline(*istream, line);

                if(line.length() > 0)
                {
                    switch (line.at(0))
                    {
                    case '#':
                        [[fallthrough]];
                    case '!':
                        [[fallthrough]];
                    case ';':
                        iniLine._type = EIniLineType::comment;
                        #ifdef DEBUG_INI_LEXER
                        INFO << "Ini lexer: Comment: " << line << "\n";
                        #endif
                        break;
                    case '[':
                        iniLine._type = EIniLineType::section;

                        iniLine._section = line.substr(1, line.find(']')-1);

                        if(iniLine._section.find('.') != std::string::npos)
                        {
                            iniLine._type = EIniLineType::subsection;
                            iniLine._subsection = iniLine._section.substr(0, iniLine._section.find('.') + 1);
                            #ifdef DEBUG_INI_LEXER
                            INFO << "Ini lexer: Subsection: " << iniLine._subsection << "\n";
                            #endif
                        }
                        else
                        {
                            #ifdef DEBUG_INI_LEXER
                            INFO << "Ini lexer: Found section: " << iniLine._section << "\n";
                            #endif
                        }
                        break;
                    case ' ':
                        [[fallthrough]];
                    case '\n':
                        iniLine._type = EIniLineType::empty;
                        #ifdef DEBUG_INI_LEXER
                        INFO << "Ini lexer: Empty line: " << line << "\n";
                        #endif
                        break;
                    default:
                        iniLine._type = EIniLineType::keyvalue;
                        iniLine._key = line.substr(0, line.find('='));
                        iniLine._value = line.substr(line.find('=') + 1);
                        #ifdef DEBUG_INI_LEXER
                        INFO << "Ini lexer: Key: " << iniLine._key << " value: " << iniLine._value << "\n";
                        #endif
                        break;
                    }
                }
                else
                {
                    iniLine._type = EIniLineType::empty;
                    #ifdef DEBUG_INI_LEXER
                    INFO << "Empty line \n";
                    #endif
                }

                return iniLine;
            }
    };

}
}
#endif
