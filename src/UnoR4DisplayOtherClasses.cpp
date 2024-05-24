/* -------------------------------------------------------------------------- */
/* FILE NAME:   UnoR4DisplayOtherClasses.cpp
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240524
   DESCRIPTION: 
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla 
                Public License (MPL), v 2.0. You can obtain a copy of the MPL 
                at http://mozilla.org/MPL/2.0/. 
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#include "UnoR4DisplayOtherClasses.h"

namespace r4display {

ChFunction dummyFunction{CH_UNIT_NO_UNIT};
ChFunction Channel::getChangeableFunction(uint8_t f /*= 0*/) {
   if(functions[f] != nullptr) {
        return *functions[f];
   }
      return dummyFunction;
}


DisplayExpansion *factoryExpansion(uint8_t type) {
   DisplayExpansion *rv = nullptr;
   if(type == EXPANSION_OPTA_ANALOG) {
      rv =  new DisplayExpansionAnalog();
   }
   else if(type == EXPANSION_DIGITAL_INVALID) {
      rv =  new DisplayExpansionDigital();
   }
   else if(type == EXPANSION_OPTA_DIGITAL_MEC) {
      rv =  new DisplayExpansionDigitalMec();
   }
   else if(type == EXPANSION_OPTA_DIGITAL_STS) {
      rv =  new DisplayExpansionDigitalSts();
   }
   else if(type == UNO_R4_DISPLAY_ADDITIONAL_TYPE) {
      rv =  new DisplayR4Display();
   }
   if(rv != nullptr) {
      rv->setType(type);
   }
   return rv;
}

}



