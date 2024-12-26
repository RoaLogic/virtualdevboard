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
 * Virtual development board components are components which have a specific
 * system functionality, for example a LED. Meaning that there is a output
 * of the system that connects with a LED, that LED is then shown on the GUI 
 * to the user. To set this up there are two different base designs, one basic 
 * module to connect with the verilated design and a module to show the state 
 * on the GUI. This is split for two reasons, one is that it's possible to have
 * a component which is not connected to the verilated design or not shown on 
 * the GUI. Second reason is that it's possible to change the UI framework 
 * without having to change each component, this also helps with the two 
 * different threads for the verilated design and the GUI.
 * 
 * All vdb components shall be placed in the vdb directory and have there own
 * submap named vdb<component name>. All design files shall be added
 * in this subdirectory, where the classes that connect with the verilated 
 * design are called vdb<component name>. Classes that show something on 
 * the GUI are named differently, <UI framework>vdb<component name>.
 * 
 * As example the LED, placed in subdirectory vdbLED. It has the system verilog
 * design in vdbLED.sv and the C++ sources that connect with the verilated design,
 * vdbLED.hpp and vdbLED.cpp. The LED implementation for wxWidgets is placed in the
 * wxWidgetsVdbLED.hpp and wxWidgetVdbLED.cpp.
 * 
 * @section vdbComponent_2 virtual development board component basic module
 * 
 * The virtual development board basic module is the module that directly 
 * connects with the verilated design. This can be done in two ways, first is
 * by a specific system verilog design component that uses DPI functions. 
 * Second method is by directly checking the design itself. Preferred is to
 * use the DPI methodology, as this simplifies and enhances design speed.
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
 * implementation. In this way the DPI functions are private and are not called
 * within the design.
 * 
 * @attention Every DPI function must call the processVerilatorEvent() function
 * so that the system can handle the event accordingly.
 * 
 * @section vdbComponent_3 virtual development board component UI common
 * 
 * Each UI component shall implement the RoaLogic::GUI::cGuiVDBComponent base 
 * class. In this class all the basics are implemented to directly communicate
 * with the vdb common component, which is passed during construction. There are
 * cases where the verilated component is not used, a nullptr can at that point 
 * be given and the class will handle it accordingly. 
 * 
 * The VDB common component does not care about the UI implementation and abstracts 
 * it away. This abstraction is created by the observer pattern and by using a
 * virtual function. Both serve a different use case, since the vdb component does 
 * not know anything about the UI implementation, but the UI implementation does 
 * know the vdb common component, it can call the virtual cppEvent() function. With 
 * this data from the UI can be passed into the verilated context, where each 
 * component can override the cppEvent() and implement it's implementation.
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
 * 
 * @section vdbComponent_4 virtual development board existing components
 * 
 * LED: see @ref vdbComponentLED
 * 7Segment display: see @ref vdbComponent7Seg
 * VGA: see @ref vdbComponentVGA
 * 
 * @section vdbComponent_5 virtual development board component creating a new one
 * 
 * @subsection Verilated component
 * A new VDB component which communicates with the verilated design is created by 
 * deriving from the cVDBCommon base class. The new class must then implement the 
 * void verilatorCallback(uint32_t event) function, which is called when an event 
 * from the verilated context occurs. In cases where it expects an event from the
 * GUI, it must also implement the void cppEvent(uint32_t event) function. Eventual
 * DPI functions are placed in the cpp file and shall call the verilatorCallback 
 * function, see example below. If there is a GUI component (which must register 
 * to the VDB component), we can notify it. If there is no component listening, 
 * the event will be ignored.
 * 
 * Example class definition:
 * @code {.c++}
 *
 * namespace RoaLogic
 * {
 * namespace vdb 
 * {
 *     class cMyNewVDBComponent : public cVDBCommon
 *     {
 *         private:
 *         // Define the verilator callback function
 *         void verilatorCallback(uint32_t event);
 *
 *         public:
 *         // The constructor must have the scopename
 *         cMyNewVDBComponent(std::string scopeName, uint8_t id); 
 *         ~cMyNewVDBComponent();
 *     };
 * }
 * }
 * @endcode
 * 
 * Example class implementation
 * @code {.c++}
 * 
 * // This function is a implementation of the DPI function declared in the system verilog code
 * void myNewVDBComponentDPIFunction
 * {
 *      // It calls the processVerilatorEvent function with the scope of the DPI function and
 *      // the value which is passed into the corresponding verilatorCallback function.
 *      cVDBCommon::processVerilatorEvent(svGetScope(), myValue);
 * }
 * 
 * namespace RoaLogic
 * {
 *   using namespace observer;
 * namespace vdb
 * {
 *      // Construct the new class, internally everything is done within the cVDBCommon constructor
 *      cMyNewVDBComponent::cMyNewVDBComponent(std::string scopeName, uint8_t id) :
 *          cVDBCommon(scopeName, id)
 *      {
 *          
 *      } 
 * 
 *      cMyNewVDBComponent::~cMyNewVDBComponent()
 *      {
 * 
 *      }
 * 
 *     void cMyNewVDBComponent::verilatorCallback(uint32_t event)
 *     {
 *          // Handle the verilated event, which is up to the class to handle
 * 
 *          // In case there is a GUI element, notify the observer and pass it the data
 *          notifyObserver(eEvent::myNewEvent, myEventDataPointer);
 *     }
 * 
 * }
 * }
 * @endcode
 * 
 * @subsection GUI component
 * 
 * In this part we only explain the specifics of adding a GUI component within the system, this 
 * excludes the drawing part of the component as this is specific for each GUI framework. Each 
 * GUI component shall be derived from the cGuiVDBComponent. This baseclass holds all information
 * to communicate with the VDB verilated component and to handle eventual calls from the GUI. The 
 * constructor takes a pointer to the VDB verilatored component, which can be given as nullptr in
 * case it is not used. The class will internally register to the VDB component and receive the
 * corresponding events in the notify() function. Since this function is specific to the component
 * it is virtual and shall be overwritten by the derived class. The constructor also takes a 
 * distancePoint, which translates to the position on the GUI screen. This position is relative
 * in dimension to the top left corner of the board screen. See @ref <add ref> for more information.
 * It is possible in some cases that the GUI has to clean up when the system is closed or restarted,
 * for this reason there is a virtual onClose() function. This function is called when the component
 * is closed to clean up, which is also depend on the implemented class.
 * 
 * Example class definition:
 * @code {.c++}
 *
 * namespace RoaLogic
 * {
 * namespace GUI 
 * {
 *     class cNewGuiVdbComponent : public cGuiVDBComponent, <possible GUI framework base classes>
 *     {
 *         private:
 *         // Function to receive events from the vdb component
 *         void notify(eEvent aEvent, void* data);
 * 
 *         public:
 *         // The constructor must have a pointer to the vdb component and a position on the screen
 *         cNewGuiVdbComponent(cVDBCommon* myVDBComponent, distancePoint position); 
 *         ~cNewGuiVdbComponent();
 * 
 *         // Possible function if needed
 *         void onClose();
 * 
 *         // Functions needed by the GUI framework to draw something on the screen
 *     };
 * }
 * }
 * @endcode
 * 
 *  * Example class implementation
 * @code {.c++}
 *  
 * namespace RoaLogic
 * {
 *   using namespace observer;
 *   using namespace dimensions;
 * namespace vdb
 * {
 *      // Construct the new GUI class, internally everything is done within the cGuiVDBComponent constructor
 *      cNewGuiVdbComponent::cNewGuiVdbComponent(cVDBCommon* myVDBComponent, distancePoint position) :
 *          cGuiVDBComponent(myVDBComponent, position)
 *      {
 *          
 *      } 
 * 
 *      cNewGuiVdbComponent::~cNewGuiVdbComponent()
 *      {
 * 
 *      }
 * 
 *     void cNewGuiVdbComponent::notify(eEvent aEvent, void* data)
 *     {
 *          // Handle the event, which is up to this class to handle
 *          // Do note that this function runs in the verilated thread, we need to pass
 *          // the event to the GUI thread before we can adjust the GUI
 *          postEventToGuiEventHandling(data);
 *     }
 * 
 * }
 * }
 * @endcode
 * 
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
