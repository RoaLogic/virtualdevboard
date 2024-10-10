/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Base Class for Parsers                                       //
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


#ifndef ROA_PARSER
#define ROA_PARSER

//include lexer
#include "lexer.hpp"

namespace RoaLogic
{
namespace parser
{
    using namespace lexer;

    /**
     * @brief Parser Exceptions
     * @details Allows throwing custom exceptions
     */
    class ParserException : public std::exception {
        private:
            std::string _msg;

        public:
            ParserException(std::string msg) noexcept : _msg(msg) {}

            virtual const char* what() const noexcept {
                return _msg.c_str();
            }
    };


    /**
     * @class parserBase
     * @author Richard Herveille
     * @brief Template class for a parser
     * version 1.0.0
     *
     * Based on:
     * craftinginterpreters.com
     * https://github.com/agrif/mif/blob/master/mif/__init__.py
     * https://github.com/sierrafoxtrot/srecord/blob/master/srecord/input/file/mif.cc
     */
    template <typename lexer_t, typename data_t> class parserBase
    {
        protected:
            /**
             * lexer
             */
            lexer_t* lexer;

            /**
             * data buffer
             */
            data_t dataBuffer;

        public:
            /**
             * Constructor
             */
            parserBase() {}

            /**
             * Destructor
             */
            ~parserBase() {}

            /**
             * @brief Method returning parsed data
             * @returns Parsed data
             */
            virtual data_t& data() { return dataBuffer; }
    };
}
}
#endif

