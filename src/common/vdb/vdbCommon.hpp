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

/**
 * @section vdbComponent_1 Virtual development board components
 * 
 * Virtual development board components are specific components which
 * on one side connect with the verilated design and on the other side
 * show there state to the user. It connects with the verilated
 * code and interpret the signals of the design back into logic. This
 * is not limited to C++ code, it's possible that it has a minor 
 * System Verilog design component. Using System Verilog enhances 
 * performance but also aids the C++ component development and makes
 * it more simple and more readable. Each component shall have a specific
 * implementation for showing information to the user, this is seperated
 * so that different UI frameworks can be used. But with this it's also
 * possible to implement something completly different for showing the 
 * information to the user. The second reason to have seperate implementation
 * for the verilated design and the UI design is the use of different 
 * threads. 
 * 
 * One example for this are the LED's, they have a system verilog component,
 * which notifies when the corresponding pin of the design is toggled. It 
 * simplifies the C++ code that it only waits for an event and doesn't need
 * to poll the IO line every clock tick. On the UI we could draw the LED and
 * let it glow when the LED is active and dim when the LED is off.
 * 
 * @section vdbComponent_2 virtual development board verilated common design
 * 
 * Since the base of each virtual development board component is the same, a
 * generic base class is designed, see @ref RoaLogic::vdb::cVDBCommon. This is a 
 * header only class implementation which holds everything needed to communicate 
 * with a verilated component. To link a vdb component with a verilated design 
 * component, the verilated scope is used. This scope must be given during 
 * construction of the class, see @ref <add ref> for more information about 
 * the scope of a verilated design.
 * 
 * When a verilated component has an event, like the toggling of an IO pin, a 
 * specific DPI function is called. However this DPI function only knows the 
 * scope of the design and not the corresponding class scope. Because of this
 * the vdb common component holds a few static functions to handle the mapping
 * of the scope and the pointer of the vdb component. All vdb components and 
 * their scope are stored statically, which is automaticcaly done during class 
 * creation and destruction. 
 * 
 * DPI functions must call the processVerilatorEvent() with the scope of the 
 * event and possible event data. It traverses all the components to find 
 * the related scope, when found it calls the virtual verilatorCallback()
 * function. In this function it has changed the context and we are now 
 * inside the class of the vdb component with the related event data. Since 
 * the verilatorCallback() function is pure virtual the derived class
 * determines the implementation of the vdb component.
 * 
 * @note DPI functions shall be placed within the *.cpp file of the corresponding
 * implementation. This so that we cannot accidently call those functions.
 * 
 * @attention Every DPI function must call the processVerilatorEvent() function
 * so that the system can handle the event accordingly.
 * 
 * @section vdbComponent_3 virtual development board UI common
 * 
 * Each virtual development board component is developed in such way that it 
 * is abstracted from the UI framework. This means that each vdb component shall
 * have a verilated design component and a specific UI implementation for the
 * corresponding framework. Since the vdb common component does not know or care
 * anything about the UI implementation it abstracts this fully away. 
 * 
 * The abstraction is created by the observer pattern and by using a virtual function.
 * Both serve a different use case, since the vdb component does not know anything
 * about the UI implementation, but the UI implementation does know the vdb common 
 * component, it can call the virtual cppEvent() function. With this data from the 
 * UI can be passed into the verilated context, where each component can override 
 * the cppEvent() and implement it's implementation.
 * 
 * From the verilated context to the UI is slightly more complicated, especially since
 * each UI framework uses a different event mechanism. Because of this every vdb 
 * component shall implement a CSubject, so that any UI component can register to it.
 * Every vdb component can now notify the UI component, which at that point can
 * notify the UI framework with it's own event handling. 
 * 
 * @attention Due that UI components shall be updated within there own thread,
 * the observer of the vdb component must handle the context switch from the
 * verilator thread to the UI thread.
 */

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
        private:
        /**
         * @brief Structure to hold the scope and
         * pointer for a vdb component 
         */
        struct sVdbMap
        {
            svScope scope;          //!< Scope of the component
            cVDBCommon* reference;  //!< Pointer to the component
        };
        static std::vector<sVdbMap> _referencePointers; //!< Static vector for all maps

        protected:
        /**
         * @brief register a vdb component
         * @details This function registers a map of the
         * scope and vdb common component together into 
         * a static vector.
         * 
         * @param[in] map   The new vdb map to register
         */
        static void registerVdb(sVdbMap map)
        {
            _referencePointers.push_back(map);
        }

        /**
         * @brief unregister a vdb component
         * @details This function unregisters a map of the
         * scope and vdb common component. First we traverse
         * the list and see if we can find the reference,
         * when found this element is removed from the list.
         * 
         * @param[in] map   The vdb map to unregister
         */
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
        /**
         * @brief Process a verilator event
         * @details This function shall be called from a verilated DPI
         * function. It takes the scope of the event and the value of the
         * event (which is specific to each component). 
         * 
         * Traverse the full list of registered components and see if we
         * can find the scope. When the scope is found the virtual 
         * verilatorCallback function is called with the corresponding
         * event code.
         * 
         * @param[in] scope     The verilated scope of the event
         * @param[in] event     The event which happend (specific to the component)
         */        
        static void processVerilatorEvent(svScope scope, uint32_t event)
        {
            bool found = false;

            // Loop all registered vdb components
            for (const sVdbMap& ref : _referencePointers)
            {
                // Check if we found the component
                if(ref.scope == scope)
                {
                    // Call the callback with the event value
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

        protected:
        size_t  _myID;           //!< The ID of the vdb component
        svScope _myScope;        //!< The scope of the verilated context

        public:
        /**
         * @brief Construct a new cVDBCommon object
         * @details This constructer creates a new cVDBCommon object
         *
         * First the scope is set according to the given name, where it is
         * also checked that the scope exists. When this is ok, it registers
         * this class for any verilator actions.
         *
         * @param[in] scopeName     Scope of this vdb component
         * @param[in] id            Optional ID of the component
         */
        cVDBCommon(std::string scopeName, size_t id) : 
            _myID(id)
        {
            // Get the scope according to the given name
            _myScope = svGetScopeFromName(scopeName.c_str());
            svSetScope(_myScope);

            // Make sure that the scope is not a nullpointer
            assert(_myScope != nullptr);

            // Store this class instance in the global class instances
            registerVdb(sVdbMap{_myScope, this});
        };

        /**
         * @brief destruct the cVDBCommon object
         *
         * Unregister this class so that it's not called anymore
         */
        virtual ~cVDBCommon()
        {
            unregisterVdb(sVdbMap{_myScope, this});
        }

        /**
         * @brief Get the ID of the component
         * 
         * @return The ID number of the component 
         */
        size_t getID(){return _myID;};

        /**
         * @brief Verilator function callback
         * @details This function is called when an
         * event occurs for this vdb component. In this function
         * we are in the class and have all knowledge needed.
         * 
         * The event data is specific per component, where it
         * is just passed as uint32_t.
         * 
         * @param[in] event The event data for the handler
         */
        virtual void verilatorCallback(uint32_t event) = 0;

        /**
         * @brief Callback function from the C++ side
         * @details This function is called when an event
         * occurs on the C++ side of the vdb component.
         * 
         * Derived classes can override this function as 
         * needed and implement their own methodology.
         * 
         * @todo Do we need a context switch or could it
         * be changed from a different thread, it is already
         * async, so why not?
         * 
         * @param[in] event     The event data 
         */
        virtual void cppEvent(uint32_t event){};
    };

    inline std::vector<cVDBCommon::sVdbMap> cVDBCommon::_referencePointers;
}
}

#endif
