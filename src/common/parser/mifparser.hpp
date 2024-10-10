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


#ifndef ROA_MIFPARSER
#define ROA_MIFPARSE

//include parser
#include "parser.hpp"

//include MIF lexer
#include "miflexer.hpp"

//include std::vector
#include <vector>

//#include <filesystem>

namespace RoaLogic
{
namespace parser
{
    /**
     * Struct to hold <address,data> pair
     */
    struct parseData_t {
        long address;
        long data;
    };

    typedef std::vector<parseData_t> parseDataArray_t;


    /**
     * @class mifparser
     * @author Richard Herveille
     * @brief Parser for MIF files
     * version 1.0.0
     *
     * Based on:
     * craftinginterpreters.com
     * https://github.com/agrif/mif/blob/master/mif/__init__.py
     * https://github.com/sierrafoxtrot/srecord/blob/master/srecord/input/file/mif.cc
     */
    class mifparser : public parserBase<miflexer,parseDataArray_t>
    {
       private:
            /**
             * input filestream
             */
            std::ifstream* _ifs;
            std::string    _filename;


            /**
             * Parse variables
             */
            long depth;
            long width;
            long addressRadix;
            long dataRadix;
            long addressStart;
            long addressRange;

            /**
             * Vector holding the <address,data> pair
             */
            parseData_t parseData;

            /**
             * States for parser statemachine
             */
            typedef enum {
                stateHeader,
                stateAddress,
                stateData,
                stateEof
            } state_t;

            state_t state;


            /**
             * @brief Open file
             */
            void open(std::string fileName)
            {
                _ifs = new std::ifstream;

                _ifs->exceptions(std::ifstream::failbit | std::ifstream::badbit);

                try
                {
                    _ifs->open(fileName, std::fstream::in);
                }
                catch(const std::ifstream::failure& e)
                {
                    throw ParserException(std::string("Failed to open %s", e.what()));
                }
            }


            /**
             * get* routines
             */
            void getColon()
            {
                if (lexer->getToken() != tokColon)
                    throw ParserException("colon ':' expected");
            }

            void getEquals()
            {
                if (lexer->getToken() != tokEquals)
                    throw ParserException("equal sign '=' expected");
            }

            long getNumber()
            {
                if (lexer->getToken() != tokNumber)
                    throw ParserException("number expected");

                return lexer->getNumber();
            }

            void getSemicolon()
            {
                if (lexer->getToken() != tokSemicolon)
                    throw ParserException("semicolon ';' expected");
            }

            void getBracketRight()
            {
                if (lexer->getToken() != tokBracketRight)
                    throw ParserException("Right bracket ']' expected");
            }

            int getRadix()
            {
                switch (lexer->getToken())
                {
                    case tokBin:
                        return 2;

                    case tokDec:
                        return 10;

                    case tokHex:
                        return 16;

                    case tokOct:
                        return 8;

                    default:
                        throw ParserException("unsupported radix");
                }
            }


            /**
             * @brief Parser
             * @details Parses .mif file and returns array of memory
             */
            bool parse()
            {
                for (;;)
                {
                    switch (state)
                    {
                        /**
                         * Header contains DEPTH, WIDTH
                         * ADDRESS_RADIX, DATA_RADIX
                         * CONTENT (with BEGIN)
                         */
                        case (stateHeader):
                            switch (lexer->getToken())
                            {
                                case tokDepth:
                                    getEquals();
                                    depth = getNumber();
                                    getSemicolon();
                                    break;

                                case tokWidth:
                                    getEquals();
                                    width = getNumber();
                                    getSemicolon();
                                    break;

                                case tokAddressRadix:
                                    getEquals();
                                    addressRadix = getRadix();
                                    getSemicolon();
                                    break;

                                case tokDataRadix:
                                    getEquals();
                                    dataRadix = getRadix();
                                    getSemicolon();
                                    break;

                                case tokContent:
                                    if (lexer->getToken() != tokBegin)
                                        throw ParserException("BEGIN expected");
                                    state = stateAddress;
                                    break;

                                default:
                                  throw ParserException("Unknown entry in MIF header section");
                            }
                            break;


                        /**
                         * Address format:
                         * A (single number)
                         * [A0..A1]
                         */
                        case (stateAddress):
                            lexer->setRadix(addressRadix);
                            switch (lexer->getToken())
                            {
                                case tokNumber:
                                    addressStart = lexer->getNumber();
                                    addressRange = 0;
                                    getColon();
                                    state = stateData;
                                    break;

                                case tokBracketLeft:
                                    {
                                        if (lexer->getToken() != tokNumber)
                                            throw ParserException("Start of address range expected");

                                        long addressLo = lexer->getNumber();

                                        if (lexer->getToken() != tokDotDot)
                                            throw ParserException("Address range expected '...'");

                                        if (lexer->getToken() != tokNumber)
                                           throw ParserException("End of address range expected");

                                        long addressHi = lexer->getNumber();
                                        if (addressHi < addressLo)
                                            throw ParserException("Address high-range less than low-range");

                                        addressStart = addressLo;
                                        addressRange = addressHi - addressLo +1;
                                        getBracketRight();
                                        getSemicolon();
                                        state = stateData;
                                    }
                                    break;

                                case tokEnd:
                                    state = stateEof;
                                    break;

                                default:
                                    throw ParserException("Address expected");
                            }
                            break;


                        case (stateData):
                            lexer->setRadix(dataRadix);
                            switch (lexer->getToken())
                            {
                                case tokNumber:
                                    //create new <address,data> pair
                                    parseData.address = addressStart;
                                    parseData.data    = lexer->getNumber(); 
                                    dataBuffer.push_back(parseData);
                                    addressStart++;
                                    addressRange--;
                                    break;

                                case tokSemicolon:
                                    state = stateAddress;
                                    break;

                                default:
                                    throw ParserException("Data expected");
                            }
                            break;


                        case (stateEof):
                          return true;
                    }
                }
            }


        public:
           /**
            * @brief Construct a new .mif parser object
            * @details This function constructs the object
            */
            mifparser (std::string& fileName) : _filename(fileName)
            {
                addressRadix = 10;
                dataRadix = 10;
                state = stateHeader;
                open(_filename);
                lexer = new miflexer(_ifs);
                parse();
            }


            /**
             * @brief Destructor
             */
            ~mifparser()
            {
                if(_ifs->is_open())
                {
                    _ifs->close();
                }
            }
    };
}
}
#endif
