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


/* This example shows how to use an UNO R4 Wifi equipped with 4 buttons and 
   on small LCD display to display information about Analog and Digital Expansion
   attached to Opta Controller 
   The R4 display not only show information but also can be used to change
   values and configuration of other espansion
   
   to update the R4 display only 2 calls are needed 
   R4DisplayExpansion::updateDisplay(); -> this update the info to the display
   R4DisplayExpansion::updateExpansions(); -> this gets changes to be transferred 
                                              to the expansions

   
   This sketch also provide a Serial interface allowing to do the same, i.e.
   change the status or the configuration of channel expansions 
   All other function in this sketch are used to provide the serial interface */


#include "OptaBlue.h"
#include "R4DisplayExpansion.h"

/*________SERIAL INTERFACE: print the type of the expansion in a readable way */
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

/*______________________________SERIAL INTERFACE: print expansion information */
void printExpansionInfo() {
  
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

/*_______________________________SERIAL INTERFACE: get an integer from serial */
int getIntegerFromSerial() {
  /* basic function that get a integer from serial 
   * (can be improved....)*/
  
  /* this function returns -1 if the user just hit return without entering 
   * any number */
  int rv = -1;
  /* wait for user to write something on the serial line */
  while(!Serial.available()) {
  }
  
  /* get the number (everything is not a number is ignored) */
  while(Serial.available()) {
    
    int num = Serial.read();
    
    if( (num >= 0x30 && num <= 0x39) ) {
      if(rv == -1) {
        rv = 0;
      }
      rv *= 10;
      rv += num - 0x30;
    }
    else {
      
    }
  }
  
  return rv;
}

/*________________________SERIAL INTERFACE: get an integer from serial (wait) */
int getIntegerNonBlocking() {
   /* basic function that get a integer from serial 
   * (can be improved....)
   * it does not wait for input from serial and return -1 if*/
  int rv = -1;
  if(Serial.available()) {
    rv = 0;
  }
  
  while(Serial.available()) {
    int num = Serial.read();
    
    if( (num >= 0x30 && num <= 0x39) ) {
      rv *= 10;
      rv += num - 0x30;
    }
  }
  return rv;
}

/*___________________________________SERIAL INTERFACE: get a char from serial */
char getCharFromSerial() {
/* -------------------------------------------------------------------------- */
  /* basic function that get a char from serial 
   * (can be improved....)*/

  char rv = 255;
  while(!Serial.available()) {
  }
  
  bool read = true;

  while(Serial.available()) {
    
    int num = Serial.read();
    
    if(read) {
      read = false;
      rv = (char)num;
    }
  } 
  return rv;
}

/*__________________________________SERIAL INTERFACE: get a float from serial */
float getFloatFromSerial() {
/* -------------------------------------------------------------------------- */
  /* basic function that get a float from serial 
   * (can be improved....)*/

  float rv = 0;
  float decimal = 0;
  float dec_mult = 0.1;
  /* wait for user to write something on the serial line */
  while(!Serial.available()) {
  }
  
  bool integer_part = true;

  /* */
  while(Serial.available()) {
    
    int num = Serial.read();

    if(integer_part) {
      if( (num >= 0x30 && num <= 0x39) ) {
        rv *= 10;
        rv += (float)(num-0x30);
      }
      else if(num == '.') {
        integer_part = false;
      }
    }
    else {
      if( (num >= 0x30 && num <= 0x39) ) {
        float a = dec_mult * (float)(num-0x30);
        decimal += a;
        dec_mult /= 10;
      }
    }
  } 
  rv += decimal;
  return rv;
}


/* ____________________________________________________________SHOW MAIN MENU */
void showMainMenu() {
 
    Serial.println("\n****** Main menu: *******");
    Serial.println("  1.  CONFIGURE expansion (channel configuration for Analog)");
    Serial.println("  2.  CHANGE expansion outputs");
    Serial.println("  3   ADD Adc to Analog Expansion channels");
    Serial.println("  4.  Show expansions");
    Serial.println(":> ");
    
}






/*__________________________________________________________SERIAL INTERFACE: */
/* given a certain Analog expansion 'device' index this function ask 
 * the user to configure the 8 channels of the analog expansion by
 * choosing from a menu */
/* -------------------------------------------------------------------------- */
void configureChannels(uint8_t device) {
/* -------------------------------------------------------------------------- */
    if(device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {

      AnalogExpansion e = OptaController.getExpansion(device);

      if(e) {
        //e.setFailedCommCb(comm_timeout_cb);

        Serial.println("\n# Configure ANALOG expansion at index " + String(device));

        for(int ch = 0; ch < OA_AN_CHANNELS_NUM; ch++) {
            Serial.println();
            Serial.println("  1-> DAC VOLTAGE");
            Serial.println("  2-> DAC CURRENT");
            Serial.println("  3-> ADC VOLTAGE");
            Serial.println("  4-> ADC CURRENT");
            Serial.println("  5-> RTD 2 WIRES");
            Serial.println("  6-> RTD 3 WIRES");
            Serial.println("  7-> Digital Input");
            Serial.println("  8-> HIGH IMPEDENCE");

            Serial.println("Hit return to skip this channel configuration or...");
            Serial.println("Select function for CHANNEL  n. " + String(ch) + ":>");
            
            /* ask user for a choice */
            int sel = getIntegerFromSerial();

            if(sel == -1) {
              continue;
            }
            
            /* configure the channel  as requested by user */
            Serial.print(" CHANNEL " + String(ch) + " set as");
            if(sel == 1) {
              Serial.println(" DAC VOLTAGE");
              e.beginChannelAsVoltageDac(ch); // use default parameters
            }
            else if(sel == 2) {
              Serial.println(" DAC CURRENT");
              e.beginChannelAsCurrentDac(ch); // use default parameters
            }
            else if(sel == 3) {
              Serial.println(" ADC VOLTAGE");
              e.beginChannelAsVoltageAdc(ch); // use default parameters
            }
            else if(sel == 4) {
              Serial.println(" ADC CURRENT");
              e.beginChannelAsCurrentAdc(ch); // use default parameters
            }
            else if(sel == 5) {
              Serial.println(" RTD 2 WIRES");
              e.beginChannelAsRtd(ch,false,1.0); // third parameter is unused in
              // this case
            }
            else if(sel == 6) {
              Serial.println(" RTD 3 WIRES");
              e.beginChannelAsRtd(ch,true,1.5); 
            }
            else if(sel == 7) {
              Serial.println(" Digital Input");
              e.beginChannelAsDigitalInput(ch); 
            }
            else if(sel == 8) {
              Serial.println("  High Impedence");
              e.beginChannelAsHighImpedance(ch);
            }
            else {
              Serial.println("\nERROR: invalid choice, channel " + String(ch) + "will configured as High Impedance");
            }
        }
      }
      else {
       Serial.println("\nERROR: could not find an Analog Expansion at index " + String(device));
      }
   }
}



/*__________________________________________________________SERIAL INTERFACE: */
/* given a certain Analog expansion 'device' index this function ask 
 * the user to add an additional ADC (current or voltage) to that channel */
/* -------------------------------------------------------------------------- */
void addAdcToConfiguration(uint8_t device) {
/* -------------------------------------------------------------------------- */
  if(device >= 0 && device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
     
     Serial.println("+ Selected Analog Expansion at index " + String(device));
     AnalogExpansion e = OptaController.getExpansion(device);
     
     if(e) {
        for(int ch = 0; ch < OA_AN_CHANNELS_NUM; ch++) {
            bool addable = false; 
            if(e.isChVoltageDac(ch)) {
              addable = true;
              Serial.println("Channel " + String(ch) + " is DAC VOLTAGE");
            }
            else if(e.isChCurrentDac(ch)) {
              addable = true;
              Serial.println("Channel " + String(ch) + " is DAC CURRENT");
            }
            else if(e.isChDigitalInput(ch)) {
              addable = true;
              Serial.print("Channel " + String(ch) + " is DIGITAL INPUT -> ");
            }
            else if(e.isChHighImpedance(ch)) {
              addable = true;
              Serial.println("Channel " + String(ch) + " is HIGH IMPEDANCE");
            }

            if(addable) {
              Serial.println("Add an ADC measurement to channel " + String(ch) + "? [N/y]");
              char s = getCharFromSerial();
              if(s == 'y' || s == 'Y') {
                  Serial.println("Select the kind of channel:    1. VOLTAGE    2. CURRENT");
                  uint32_t st = 0;
                  st = getIntegerFromSerial();
                  if(st == 1) {
                    e.addVoltageAdcOnChannel(ch);
                  }
                  else if(st == 2) {
                    e.addCurrentAdcOnChannel(ch);
                  }
                  else {
                    Serial.println("ERROR: invalid option!");
                  }
              }
              
            }
        } 
     }
     else {
      Serial.println("ERROR: could not find an Analog Expansion at index " + String(device));
     }
  }
}

/*__________________________________________________________SERIAL INTERFACE: */
/* given a certain Analog expansion 'device' index this function ask 
 * the user to change output values for that expansion */
/* -------------------------------------------------------------------------- */
void changeExpansion(uint8_t device) {
/* -------------------------------------------------------------------------- */
  if(device >= 0 && device < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
     
     AnalogExpansion e = OptaController.getExpansion(device);
     if(e) {
        Serial.println(" Selected ANALOG expansion at index " + String(device));
        /* setting DAC values */
        for(int ch = 0; ch < OA_AN_CHANNELS_NUM; ch++) {

            if(e.isChVoltageDac(ch)) {
              Serial.println("Channel " + String(ch) + " is DAC VOLTAGE");
              
              Serial.println("  Enter the new value in Volts [0-11]: ");
              float v = getFloatFromSerial();
              Serial.println("  -> Entered voltage: " + String(v) + " V");

              if(v >= 0 && v <= 11.0) {
                Serial.println("     setting new value...");
                e.pinVoltage(ch,v);
              }
              else {
                Serial.println("ERROR: wrong value");
              }
            
            }
            else if(e.isChCurrentDac(ch)) {
              Serial.println("Channel " + String(ch) + " is DAC CURRENT");
              Serial.println("  Enter the new value in milli-Ampere [0-25]: ");
              float v = getFloatFromSerial();
              Serial.println("  -> Entered current: " + String(v) + " mA");
              if(v >= 0 && v <= 25.0) {
                Serial.println("      setting new value...");
                e.pinCurrent(ch,v);
              }
              else {
                Serial.println("ERROR: wrong value");
              }
            }
        }
        
        Serial.println("Changing PWM channels? [N/y]");
        char p = getCharFromSerial();
        
        if(p == 'y' || p == 'Y') {
          for(int ch = OA_PWM_CH_FIRST; ch <= OA_PWM_CH_LAST; ch++) {
              Serial.println("Configure PWM " + String(ch - OA_PWM_CH_FIRST) + "? [N/y]");
              char s = getCharFromSerial();
              if(s == 'y' || s == 'Y') {
                  uint32_t period = 0;
                  uint32_t pulse = 0;
                  Serial.println("Enter the new period [in micro seconds]: ");
                  period = getIntegerFromSerial();
                  Serial.println("Entered period is " + String(period) + " micro seconds");

                  Serial.println("Enter the new pulse [in micro seconds]: ");
                  pulse = getIntegerFromSerial();
                  Serial.println("Entered pulse is " + String(pulse) + " micro seconds");

                  e.setPwm(ch, period, pulse);
              }
          }
        }

        Serial.println("Changing LED status? [N/y]");
        p = getCharFromSerial();

        if(p == 'y' || p == 'Y') {
          for(int led = OA_LED_1; led < OA_LED_NUM; led++) {
            Serial.println("Changing LED number " + String(led) + "? [N/y]");
              char s = getCharFromSerial();
              if(s == 'y' || s == 'Y') {
                  Serial.println("Select the new status:    1. ON       2. OFF");
                  uint32_t st = 0;
                  st = getIntegerFromSerial();
                  if(st == 1) {
                    e.switchLedOn(led);
                  }
                  else if(st == 2) {
                    e.switchLedOff(led);
                  }
                  else {
                    Serial.println("ERROR: invalid option!");
                  }
              }
          }
        }
     }
     else {
      Serial.println("ERROR: could not find an Analog Expansion at index " + String(device));
     }
  }

  DigitalExpansion d = OptaController.getExpansion(device);
  if(d) {
    Serial.println(" Selected DIGITAL expansion at index " + String(device));
    for(int ch = 0; ch < OPTA_DIGITAL_OUT_NUM; ch++) {
      Serial.println("Please select the new value for output channel " + String(ch));
      Serial.println("1. On    2. Off    (Just return to keep current value) :> ");
      int st = getIntegerFromSerial();
      if(st == 1) {
        d.digitalWrite(ch, HIGH, true);
      }
      else if(st == 2) {
        d.digitalWrite(ch, LOW, true);
      }
      else {
        Serial.println("Keeping present output status");
      }
    }
  }
}

/*__________________________________________________________SERIAL INTERFACE: */
/* manage the user input via Serial line                                      */
/* -------------------------------------------------------------------------- */
void serialInterface() {
/* -------------------------------------------------------------------------- */
  /* init so that menu is displayed the first time */
  static int menu_command = 0;
  static uint8_t selected_expansion = 255;

  if(menu_command > -1) {
    showMainMenu();
  }
  /* getIngeterNonCBlocking returns -1 if there are not commands 
  so in case no commands are present the menu won't be shown again */
  menu_command = getIntegerNonBlocking();

  if(menu_command == 1) {
    /* configure channels (ONLY ANALOG) */
    Serial.println("-> Configure ANALOG expansion channels");
    Serial.println("Please select an expansion:> ");
    selected_expansion = getIntegerFromSerial();
    if(selected_expansion < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
      configureChannels(selected_expansion);
    }
    else {
      Serial.println("\nWARNING: wrong selection, max num of expansion is 5");
    }
  }
  else if(menu_command == 2) {
    Serial.println("-> Change expansion outputs");
    Serial.println("Please select an expansion:> ");
    if(selected_expansion < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
      changeExpansion(selected_expansion);
    }
    else {
      Serial.println("\nWARNING: wrong selection, max num of expansion is 5");
    }
  }
  else if(menu_command == 3) {
    Serial.println("-> ADD Adc over a configured Analog channel");
    Serial.println("Please select an expansion:> ");
    selected_expansion = getIntegerFromSerial();
    if(selected_expansion < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
      addAdcToConfiguration(selected_expansion);
    }
    else {
      Serial.println("\nWARNING: wrong selection, max num of expansion is 5");
    }

  }
  else if(menu_command == 4) {
    printExpansionInfo();
  }
  else if(menu_command > -1) {
    Serial.println("\nWARNING: wrong selection, unavailable option");
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

  /* !!!!!! Register the custom expansion !!!!!! */
  OptaController.registerCustomExpansion(R4DisplayExpansion::getProduct(),
                                         R4DisplayExpansion::makeExpansion,
                                         R4DisplayExpansion::startUp);
  
}


/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */    
  OptaController.update();

  /*
   * Configuring expansion's channels and outputs with serial interface
   */
  serialInterface();
  
  
  /*
   * Sending information to R4 display
   */
  R4DisplayExpansion::updateDisplay();

  /*
   * Getting changes from user from R4 display
   */
  R4DisplayExpansion::updateExpansions();
  
}





