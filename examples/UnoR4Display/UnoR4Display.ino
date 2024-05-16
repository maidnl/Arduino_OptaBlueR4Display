/* -------------------------------------------------------------------------- */
/* FILE NAME:   UnoR4Display.ino
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240418
   DESCRIPTION: 
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla 
                Public License (MPL), v 2.0. You can obtain a copy of the MPL 
                at http://mozilla.org/MPL/2.0/. 
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */


#include "OptaBlue.h"
#include "R4DisplayExpansion.h"

void printExpansionType(ExpansionType_t t) {
  if(t == EXPANSION_NOT_VALID) {
    Serial.print("Unknown!");
  }
  else if(t == EXPANSION_OPTA_DIGITAL_MEC) {
    Serial.print("Opta --- DIGITAL [Mechanical]  ---");
  }
  else if(t == EXPANSION_OPTA_DIGITAL_STS) {
    Serial.print("Opta --- DIGITAL [Solid State] ---");
  }
  else if(t == EXPANSION_DIGITAL_INVALID) {
    Serial.print("Opta --- DIGITAL [!!Invalid!!] ---");
  }
  else if(t == EXPANSION_OPTA_ANALOG) {
    Serial.print("Opta ~~~ ANALOG ~~~ ");
  }
  else if(t == OptaController.getExpansionType(R4DisplayExpansion::getProduct())) {
	 Serial.print("*** UnoR4 Display ***");
  }
  else {
    Serial.print("Unknown!");
  }
}


void printExpansionInfo() {

static unsigned long long start = millis() + 11000;
if(millis() - start  > 5000) {

  start = millis();
  Serial.print("\nNumber of expansions: ");
  Serial.println(OptaController.getExpansionNum());

  for(int i = 0; i < OptaController.getExpansionNum(); i++) {
    Serial.print("Expansion n. ");
    Serial.print(i);
    Serial.print(" type ");
    printExpansionType(OptaController.getExpansionType(i));
    Serial.print(" I2C address ");
    Serial.print(OptaController.getExpansionI2Caddress(i));
    uint8_t M,m,r;
    if(OptaController.getFwVersion(i,M,m,r)) {
      Serial.print(" FW version: ");
      Serial.print(M);
      Serial.print('.');
      Serial.print(m);
      Serial.print('.');
      Serial.print(r);
      Serial.println();
    }
    else {
      Serial.println(" Unable to get FW version");
    }
  }
  }
}

/* -------------------------------------------------------------------------- */
/*                                 SETUP                                      */
/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */    
  Serial.begin(115200);
  delay(3000);

  OptaController.begin();
  OptaController.registerCustomExpansion(R4DisplayExpansion::getProduct(),
                                         R4DisplayExpansion::makeExpansion,
                                         R4DisplayExpansion::startUp);
  printExpansionInfo();
}

/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */    
  OptaController.update();
  printExpansionInfo();

  for(int i = 0; i < OPTA_CONTROLLER_MAX_EXPANSION_NUM; i++) {
  R4DisplayExpansion r4 = OptaController.getExpansion(i);
  if(r4) {
     
	   uint8_t selected_expansion = r4.getSelectedExpansion(); 
		
		}
  }
  
  
}




