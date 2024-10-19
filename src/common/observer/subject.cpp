/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Subject implementation source file                           //
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

#include "subject.hpp"
#include <cstring>

namespace RoaLogic {
namespace observer {

    /**
     * @brief Construct a new Subject object
     * 
     */
    cSubject::cSubject()
    {

    }

    /**
     * @brief Destroy the Subject object
     */
    cSubject::~cSubject()
    {

    }

    /**
     * @brief register an observer
     * 
     * @details Register an observer for this subject
     *          Loops to the array and checks if any free spot is available
     * 
     * @param aObserver     The reference to the observer
     * 
     * @return false             To many observers registered or observer is already registered
     * @return true              Succesfully registered
     */
    bool cSubject::registerObserver(cObserver* aObserver)
    {
        bool result = false;
        bool found = false;

        if(aObserver)
        {
            if(_observers.size() < _cMaxObservers)
            {
                for(auto observer : _observers)
                {
                    if(aObserver == observer)
                    {
                        found = true;
                    }
                }

                if (!found)
                {
                    _observers.push_back(aObserver);
                    result = true;
                }
            }            
        }

        return result;
    }

    /**
     * @brief Remove observer from array
     * 
     * @details Remove the given observer from the list
     *          Function goes through every registered element on the list 
     *          when it finds the corresponding observer the entry is set to zero
     * 
     * @param[in] aObserver     The observer to remove
     * 
     * @return false        The given observer is not registered
     * @return true         Observer removed succesfully
     */
    bool cSubject::removeObserver(cObserver* aObserver)
    {
        bool result = false;
        uint32_t iterator = 0;

        if(aObserver)
        {
            for(auto observer : _observers)
            {
                if(aObserver == observer)
                {
                    // Found the right observer, remove it and break out of the for loop
                    _observers.erase(_observers.begin() + iterator);
                    result = true;
                    break;
                }

                iterator++;
            }
        }  
        
        return result;
    }

    /**
     * @brief update function
     * 
     * @details update function for the event type
     *          Called when an event occurs, travesers through the list and calls every registerd update function
     * 
     * @param[in] aEvent The event what has occured
     * @param[in] data   A pointer to the data belonging to the event, when no data this shall be a nullptr
     */
    void cSubject::notifyObserver(eEvent aEvent, void* data)
    {
        for(auto observer : _observers)
        {
            observer->notify(aEvent, data);
        }
    }

}}