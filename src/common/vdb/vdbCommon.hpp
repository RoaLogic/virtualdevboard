/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Virtual Devboard common C++ header file                      //
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

//include Dpi headers, required to link verilator model to C++
#include "vdb__Dpi.h"

#include "log.hpp"
#include "testbench.hpp"
#include "subject.hpp"

#ifndef VDB_COMMON_HPP
#define VDB_COMMON_HPP

namespace RoaLogic
{
    using namespace observer;
namespace vdb
{
    /**
     * @class cVDBCommon
     * @author Bjorn Schouteten
     * @brief Common virtual development board component
     * @details
     * 
     * This is the common class for a virtual development board component. It 
     * holds the base for the system to interact with all the components in the
     * same matter.
     * 
     * It inherits the cSubject so that each component can notifyObservers through
     * the corresponding function. All events must be defined in the eventDefinition.hpp,
     * together with the structures of data that are sent over the events. For each
     * virtual development board component it doesn't matter who is listening, it
     * just sents the event. 
     * 
     * @todo: Add a way to receive events from the outside
     */
    class cVDBCommon : public cSubject
    {
        protected:
        struct sVdbMap
        {
            svScope scope;
            cVDBCommon* reference;
        };

        static void registerVdb(sVdbMap map)
        {
            _referencePointers.push_back(map);
        }

        static void unregisterVdb(sVdbMap map)
        {
            uint32_t iterator = 0;

            for(auto ref : _referencePointers)
            {
                if(ref.reference == map.reference)
                {
                    // Found the right observer, remove it and break out of the for loop
                    _referencePointers.erase(_referencePointers.begin() + iterator);
                    break;
                }

                iterator++;
            }
        }

        public:
        cVDBCommon(size_t id) : _myID(id){};

        static void processVerilatorEvent(svScope scope, uint32_t event)
        {
            bool found = false;

            for (const sVdbMap& ref : _referencePointers)
            {
                if(ref.scope == scope)
                {
                    ref.reference->verilatorCallback(event);
                    found = true;
                    break;
                }
            }

            if(!found)
            {
                WARNING << "VDB: Event on non registered module: " << svGetNameFromScope(scope) << " \n";
            }
        }

        virtual void verilatorCallback(uint32_t event) = 0;

        size_t getID(){return _myID;};

        private:
        static std::vector<sVdbMap> _referencePointers;
        size_t _myID;
    };

    inline std::vector<cVDBCommon::sVdbMap> cVDBCommon::_referencePointers;
}
}

#endif
