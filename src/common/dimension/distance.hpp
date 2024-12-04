/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Dimension-Distance class                                     //
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

#ifndef DIMENSIONS_DISTANCE_HPP
#define DIMENSIONS_DISTANCE_HPP

namespace RoaLogic {
namespace Dimensions {

    class cDistance
    {
        private:
        long double _distance;                        //distance in meter

        public:
        /**
         * Definitions
         */
        constexpr static double mmPerInch   = 25.4;   //definition of Inch
        constexpr static double milsPerInch = 1000;   //definition of mil
        constexpr static double inchPerFoot = 12;     //definition of foot
        constexpr static double feetPerYard = 3;      //definition of yard
        constexpr static double feetPerMile = 2580;   //definition of mile

        /**
         * @brief Constructor
         */
        cDistance() : _distance(0.0){};

        /**
         * @brief Constructor
         * @param val Initial value
         */
        cDistance(long double val) : _distance(val){}

        /**
         * @brief Desctructor
         */
        ~cDistance(){};

        /**
         * Output formats
         */

        long double mils() const { return inch() * milsPerInch;  }
        long double inch() const { return mm()   / mmPerInch;    }
        long double feet() const { return inch() / inchPerFoot;  }
        long double ft()   const { return feet();                }
        long double yard() const { return feet() / feetPerYard;  }
        long double yd()   const { return yard();                }
        long double mile() const { return feet() / feetPerMile;  }
        long double fm()   const { return _distance * 1.0E15;    }
        long double pm()   const { return _distance * 1.0E12;    }
        long double nm()   const { return _distance * 1.0E9;     }
        long double um()   const { return _distance * 1.0E6;     }
        long double mm()   const { return _distance * 1.0E3;     }
        long double cm()   const { return _distance * 100.0;     }
        long double dm()   const { return _distance * 10.0;      }
        long double m()    const { return _distance;             }
        long double dam()  const { return _distance / 10.0;      }
        long double hm()   const { return _distance / 100.0;     }
        long double km()   const { return _distance / 1.0E3;     }

        //special case to convert _distance to pixels
        int pix(int dpi)   const { return inch() * dpi; } 
    };


    /**
     * Units
     */
    inline long double operator""_mils (long double val) { return (val                          / cDistance::milsPerInch * cDistance::mmPerInch / 1000.0); }
    inline long double operator""_inch (long double val) { return (val                                                   * cDistance::mmPerInch / 1000.0); }
    inline long double operator""_feet (long double val) { return (val                          * cDistance::inchPerFoot * cDistance::mmPerInch / 1000.0); }
    inline long double operator""_ft   (long double val) { return (val                          * cDistance::inchPerFoot * cDistance::mmPerInch / 1000.0); }
    inline long double operator""_yard (long double val) { return (val * cDistance::feetPerYard * cDistance::inchPerFoot * cDistance::mmPerInch / 1000.0); }
    inline long double operator""_yd   (long double val) { return (val * cDistance::feetPerYard * cDistance::inchPerFoot * cDistance::mmPerInch / 1000.0); }
    inline long double operator""_mile (long double val) { return (val * cDistance::feetPerMile * cDistance::inchPerFoot * cDistance::mmPerInch / 1000.0); }
    inline long double operator""_nm   (long double val) { return (val / 1.0E9); }
    inline long double operator""_um   (long double val) { return (val / 1.0E6); }
    inline long double operator""_mm   (long double val) { return (val / 1.0E3); }
    inline long double operator""_cm   (long double val) { return (val / 100.0); }
    inline long double operator""_dm   (long double val) { return (val / 10.0 ); }
    inline long double operator""_m    (long double val) { return val;           }
    inline long double operator""_dam  (long double val) { return (val * 10.0 ); }
    inline long double operator""_hm   (long double val) { return (val * 100.0); }
    inline long double operator""_km   (long double val) { return (val * 1.0E3); }

    /**
     * @brief point of distance units
     */
    struct distancePoint
    {
        cDistance x,y;
    };

}}

#endif
