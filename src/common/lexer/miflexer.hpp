/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Lexer for Altera MIF files                                   //
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


#ifndef ROA_MIFLEXER
#define ROA_MIFLEXER

//include base class
#include "lexer.hpp"

namespace RoaLogic
{
namespace lexer
{
    /**
     * Lexer tokens
     */
    enum token_t {
        tokAddressRadix,
        tokBegin,
        tokBin,
        tokBracketLeft,
        tokBracketRight,
        tokColon,
        tokContent,
        tokDataRadix,
        tokDec,
        tokDepth,
        tokDot,
        tokDotDot,
        tokEnd,
        tokEquals,
        tokHex,
        tokMinus,
        tokNumber,
        tokOct,
        tokPercent,
        tokSemicolon,
        tokWidth,
        tokEOF,
        tokUnknown
    };

    /**
     * @class miflexer
     * @author Richard Herveille
     * @brief Lexer for MIF files
     * version 1.0.0
     *
     * Based on:
     * craftinginterpreters.com
     * https://github.com/agrif/mif/blob/master/mif/__init__.py
     * https://github.com/sierrafoxtrot/srecord/blob/master/srecord/input/file/mif.cc
     */
    class miflexer : public filelexer<token_t>
    {
        protected:
            /**
             * tokValue holds the extracted value if token == tokNumber
             */
            long tokValue;

            /**
             * lexRadix, sets the radix for alphanumeric values
             */
            int lexRadix;

        public:
            /**
             * @brief Constructor
             */
            miflexer(std::istream* istream) : filelexer(istream), lexRadix(10) {}

            /**
             * @brief Destructor
             */
            ~miflexer() {}

            /**
             * @brief Get token value
             * @details Return numerical value associated with tokNumber
             * @return Numerical value lexed from input stream
             */
            long getNumber() const { return tokValue; }

            /**
             * @brief Set lexer radix
             * @details Sets the radix the lexer uses when lexing alphanumeric values
             */
            void setRadix (int radix) { lexRadix = radix; }

            /**
             * @brief Get lexer radix
             * @details Returns the radix the lexer uses
             * @return Lexer radix
             */
            int getRadix() const { return lexRadix; }

            /**
             * @brief Get next token
             * @details Gets next token from input stream
             * @returns Next token
             */
            token_t getToken()
            {
                for (;;)
                {
                    int c = istream->get();

                    if (c == EOF)
                        return tokEOF;

                    switch (c)
                    {
                        //Skip white spaces
                        case '\n': lineno++;
                        case ' ':
                        case '\t':
                        case '\r':
                            continue;


                        //Handle multi-line comments
                        case '%':
                            do
                            {
                                c = istream->get();
                            }
                            while (c != EOF && c != '%');
                            continue;


                        case '-':
                            if (istream->peek() == '-')
                            {
                                //comment
                                do
                                {
                                    c = istream->get();
                                }
                                while (c != EOF && c != '\n');
                                continue;
                            }
                            else
                                return tokMinus;


                        case '=':
                            return tokEquals;


                        case ':':
                            return tokColon;


                        case ';':
                            return tokSemicolon;


                        case '[':
                            return tokBracketLeft;


                        case ']':
                            return tokBracketRight;


                        case '.':
                            if (istream->peek() == '.')
                            {
                                istream->get();
                                return tokDotDot;
                            }
                            else
                                return tokDot;

                
                        //Alphanumeric tokens
                        //case 0...9 would be so nice here
                        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                        case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
                        case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
                        case 's': case 't': case 'u': case 'v': case 'w': case 'x':
                        case 'y': case 'z':
                        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                        case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
                        case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
                        case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
                        case 'Y': case 'Z':
                        case '0': case '1': case '2': case '3': case '4': case '5':
                        case '6': case '7': case '8': case '9':
                        case '_':
                        {
                            std::string sId;

                            for (;;)
                            {
                                //append character (as char) to sId
                                sId += (char)c;

                                //get next character
                                c = istream->get();

                                if (c == EOF)
                                    break;


                                switch (c)
                                {
                                    //keep adding alphanum characters
                                    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                                    case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
                                    case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
                                    case 's': case 't': case 'u': case 'v': case 'w': case 'x':
                                    case 'y': case 'z':
                                    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                                    case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
                                    case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
                                    case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
                                    case 'Y': case 'Z':
                                    case '0': case '1': case '2': case '3': case '4': case '5':
                                    case '6': case '7': case '8': case '9':
                                    case '_':
                                        continue;


                                    //Not an alphanum, so not part of the string
                                    //Put it back into the stream
                                    default:
                                        istream->unget();
                                        break;
                                }

                                //exit append-loop
                                break;

                            } //end append-for-loop


                            //We got sId, now what is it?

                            //is it a number?
                            const char* c_str = sId.c_str();
                            char* endptr;
                            tokValue = strtol(c_str, &endptr, lexRadix);
                            if (endptr != c_str && *endptr == '\0')
                                return tokNumber;

                            //no, not a number
                            //is it a token?
                            if (sId == "ADDRESS_RADIX")
                                return tokAddressRadix;
                            if (sId == "BEGIN")
                                return tokBegin;
                            if (sId == "BIN")
                                return tokBin;
                            if (sId == "CONTENT")
                                return tokContent;
                            if (sId == "DATA_RADIX")
                                return tokDataRadix;
                            if (sId == "DEC")
                               return tokDec;
                            if (sId == "DEPTH")
                                return tokDepth;
                            if (sId == "END")
                                return tokEnd;
                            if (sId == "HEX")
                                return tokHex;
                            if (sId == "OCT")
                                return tokOct;
                            if (sId == "UNS")
                                return tokDec;
                            if (sId == "WIDTH")
                                return tokWidth;

                            return tokUnknown;
                        } //end alphanum


                        default:
                            return tokUnknown;

                    } //end switch
                } //end for
            }
    };

}
}
#endif
