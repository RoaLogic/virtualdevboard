/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Base Class for Lexers                                        //
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


#ifndef ROA_LEXER
#define ROA_LEXER
namespace RoaLogic
{
namespace lexer
{
    /**
     * @class lexer
     * @author Richard Herveille
     * @brief Template class for a lexer
     * version 1.0.0
     *
     * Based on:
     * craftinginterpreters.com
     * https://github.com/agrif/mif/blob/master/mif/__init__.py
     * https://github.com/sierrafoxtrot/srecord/blob/master/srecord/input/file/mif.cc
     */
    template <typename T> class lexerBase
    {
        protected:
            /**
             * input stream
             */
            std::istream* istream;

        public:
            /**
             * @brief Constructor
             */
            lexerBase(std::istream* istream) : istream(istream) {}

            /**
             * @brief Destructor
             */
            ~lexerBase() {}

            /**
             * @brief Get next token
             * @details Gets next token from input stream
             * @returns Next token
             */
            virtual T getToken() { return (T)0; }
    };


    /**
     * @class filelexer
     * @author Richard Herveille
     * @brief Template class for a lexer that uses a file as input stream
     * version 1.0.0
     *
     * Based on:
     * craftinginterpreters.com
     * https://github.com/agrif/mif/blob/master/mif/__init__.py
     * https://github.com/sierrafoxtrot/srecord/blob/master/srecord/input/file/mif.cc
     */
    template <typename T> class filelexer : public lexerBase<T>
    {
        protected:
            /**
             * line number
             */
            long lineno;

        public:
            /**
             * @brief Constructor
             */
            filelexer(std::istream* istream) : lexerBase<T>(istream), lineno(0) {}

            /**
             * @brief Destructor
             */
            ~filelexer() {}

            /**
             * @brief Get next token
             * @details Gets next token from input stream
             * @returns Next token
             */
            long getLineno() const { return lineno; }
    };
}
}
#endif
