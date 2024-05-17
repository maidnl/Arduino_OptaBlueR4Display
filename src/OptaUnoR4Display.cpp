/* -------------------------------------------------------------------------- */
/* FILE NAME:   OptaUnoR4Display.cpp
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240415
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#include "OptaUnoR4Display.h"

/* clause to avoid compilation of this file when the Opta Controller sketch
 * is compiled */

#ifdef ARDUINO_UNOR4_WIFI

/* > Instructions: 
 * CONSTRUCTOR: always call the base Module constructor specifying
 * - TwoWire Object to be used
 * - DETECT IN pin
 * - DETECT OUT pin */

/*_________________________________________________________________CONSTUCTOR */
OptaUnoR4Display::OptaUnoR4Display()
    : Module(&Wire1, DETECT_IN, DETECT_OUT), 
    btn_pressed(EVENT_NO_EVENT),
    display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET), 
    exp_selected(0),
    num_of_expansions(0) ,
    exp_selected_from_controller(255),
    reset_state_machine(false) {
}

/*_________________________________________________________________DESTRUCTOR */
OptaUnoR4Display::~OptaUnoR4Display() {}

/* > Instructions:
 * OVERRIDE virtual base methods: ALWAYS call BASE Module member function at
 * the beginning and then specialize the behaviour for your needs */

/*____________________________________________________________override: end() */
void OptaUnoR4Display::end() {
  /* ! Always call base methods */
  Module::end();
}

/*__________________________________________________________override: begin() */
void OptaUnoR4Display::begin() {
  /* ! ALWAYS call base method !*/
  Module::begin();

  /* then initialize your hardware */

  /* Wire to talk with small LCD display*/
  Wire.begin();
  /* four button analog input */
  pinMode(UNOR4_DIPLAY_FOUR_BUTTON_AIN, INPUT);
  /* RGB red LED */
  pinMode(UNOR4DISPLAY_LED_RED, OUTPUT);
  /* RGB red GREEN */
  pinMode(UNOR4DISPLAY_LED_GREEN, OUTPUT);
  /* RGB red BLUE */
  pinMode(UNOR4DISPLAY_LED_BLUE, OUTPUT);

  /* Initialize the display */
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed... looping forever"));
    for(;;); // Don't proceed, loop forever
  }

         
  display.clearDisplay();
  display.display();
}


void debugButtonEvent(BtnEvent_t ev);


/*_________________________________________________________override: update() */
void OptaUnoR4Display::update() {
  /* Always call base methods */
  Module::update();
  BtnEvent_t ev = button_pressed();
  if (btn_pressed == EVENT_NO_EVENT) {
    btn_pressed = ev;
  }
  debugReceivedMsg();
  debugButtonEvent(btn_pressed);

  main_state_machine();


}

/* ______________________________________________________override: parse_rx() */
int OptaUnoR4Display::parse_rx() {
 

  /* ALWAYS call base method !*/
  int rv = Module::parse_rx();
  /* If base module returns >= 0 then the message has been handled in the base
   * function... simply returns the value*/
  if (rv != -1) {
    if(address_set_up(true)) {
      reset_state_machine = true;
    }
    return rv;
  }

  rv = -1;

  /* PARSE SPECIFIC expansion messages */
   

  if (parse_get_selected_expansion()) {
    debug_msg = 1;
    return msg_ans_selected_expansion();
  } else if(parse_set_expansion_features()) {
    debug_msg = 2;
    return msg_ack();;
  } else if(parse_set_ch_configuration()) {
    debug_msg = 3;
    return msg_ack();;
  } else if(parse_set_num_of_expansion()) {
    debug_msg = 4;
    return msg_ack();;
  } 
  else {
    debug_msg = 25;
  }

  return rv;
}

void OptaUnoR4Display::debugReceivedMsg() {
  if (debug_msg == 1) {
    Serial.println("RX: get selected expansion");
  } else if(debug_msg == 2) {
    Serial.println("RX: set Expansion features");
      for(int i = 0; i < getExpectedAnsLen(EXP_FEATURES_Len); i++) {
        Serial.print(rx_buffer[i], HEX);
        Serial.print(" ");
      }
    Serial.println();
  
    
  } else if(debug_msg == 3) {
    Serial.println("RX: set Channel configuration");

      for(int i = 0; i < getExpectedAnsLen(CH_CFG_Len); i++) {
        Serial.print(rx_buffer[i], HEX);
        Serial.print(" ");
      }
    Serial.println();
  
    
  } else if(debug_msg == 4) {
    Serial.println("RX: set num of expansion");
  } else if(debug_msg == 25){
  
  for(int i = 0; i < 10; i++) {
    
    Serial.print(rx_buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  }
  debug_msg = 0;
}
/* > Instructions: 
 * WRITE implementation of your own pure virtual functions */

/* ____________________________________________________________get fw version */
uint8_t OptaUnoR4Display::getMajorFw() { return FW_VERSION_MAJOR; }
uint8_t OptaUnoR4Display::getMinorFw() { return FW_VERSION_MINOR; }
uint8_t OptaUnoR4Display::getReleaseFw() { return FW_VERSION_RELEASE; }

/* ________________________________________________________get product string */
std::string OptaUnoR4Display::getProduct() {
  std::string rv(UNOR4_DISPLAY_DESCRIPTION);
  return rv;
}

/* __________________________________________________________go in bootloader */
void OptaUnoR4Display::goInBootloaderMode() { /*goBootloader();*/ }

/* _______________________________________________________________FLASH write */
void OptaUnoR4Display::writeInFlash(uint16_t add, uint8_t *buffer,
                                    uint8_t dim) {
  if (dim > 32) {
    dim = 32;
  }
  uint8_t data[32];
  memset((uint8_t *)data, 0, dim);
  for (int i = 0; i < dim; i++) {
    data[i] = *(buffer + i);
  }
  EEPROM.put(add, data);
}

/* ________________________________________________________________FLASH read */
void OptaUnoR4Display::readFromFlash(uint16_t add, uint8_t *buffer,
                                     uint8_t dim) {
  if (dim > 32) {
    dim = 32;
  }
  uint8_t data[32];
  memset((uint8_t *)data, 0, dim);
  EEPROM.get(add, data);
  for (int i = 0; i < dim; i++) {
    *(buffer + i) = *(data + i);
  }
}

/* ______________________________________________________LEDs: initialization */
void OptaUnoR4Display::initStatusLED() {
  pinMode(UNOR4DISPLAY_LED_RED, OUTPUT);
  pinMode(UNOR4DISPLAY_LED_BLUE, OUTPUT);
  pinMode(UNOR4DISPLAY_LED_GREEN, OUTPUT);
  digitalWrite(UNOR4DISPLAY_LED_RED, LED_RGB_OFF);
  digitalWrite(UNOR4DISPLAY_LED_BLUE, LED_RGB_OFF);
  digitalWrite(UNOR4DISPLAY_LED_GREEN, LED_RGB_OFF);
}

/* _____________________________________________LEDs: ready to get an address */
void OptaUnoR4Display::setStatusLedReadyForAddress() {
  digitalWrite(UNOR4DISPLAY_LED_BLUE, LED_RGB_OFF);
  digitalWrite(UNOR4DISPLAY_LED_RED, LED_RGB_ON);
  digitalWrite(UNOR4DISPLAY_LED_GREEN, LED_RGB_OFF);
}

/* ______________________________________________LEDs: waiting for an address */
void OptaUnoR4Display::setStatusLedWaitingForAddress() {
  digitalWrite(UNOR4DISPLAY_LED_BLUE, LED_RGB_ON);
  digitalWrite(UNOR4DISPLAY_LED_RED, LED_RGB_OFF);
  digitalWrite(UNOR4DISPLAY_LED_GREEN, LED_RGB_OFF);
}

/* _________________________________________________________LEDs: has address */
void OptaUnoR4Display::setStatusLedHasAddress() {
  digitalWrite(UNOR4DISPLAY_LED_BLUE, LED_RGB_OFF);
  digitalWrite(UNOR4DISPLAY_LED_RED, LED_RGB_OFF);
  digitalWrite(UNOR4DISPLAY_LED_GREEN, LED_RGB_ON);
}

/* ########################################################################## */
/*                FROM HERE add your CUSTOM EXPANSION code                    */
/* ########################################################################## */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
                             BUTTONS FUNCTIONS
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* ___________________________________BUTTONs: debounce and fire button event */
BtnEvent_t OptaUnoR4Display::fire_button_event(BtnStatus_t st, int counter,
                                               bool &fired, bool &long_fired) {
  BtnEvent_t rv = EVENT_NO_EVENT;
  bool fire = false;
  bool long_fire = false;
  if (counter > EVENT_COUNTER_NUM && !fired) {
    fired = true;
    fire = true;
  } else if (counter > LONG_EVENT_COUNTER_NUM && !long_fired) {
    long_fired = true;
    long_fire = true;
  }
  if (fire) {
    if (st == BTN_DOWN) {
      rv = EVENT_DOWN;
    } else if (st == BTN_UP) {
      rv = EVENT_UP;
    } else if (st == BTN_LEFT) {
      rv = EVENT_LEFT;
    } else if (st == BTN_RIGHT) {
      rv = EVENT_RIGHT;
    }
  } else if (long_fire) {
    if (st == BTN_DOWN) {
      rv = EVENT_DOWN_LONG;
    } else if (st == BTN_UP) {
      rv = EVENT_UP_LONG;
    } else if (st == BTN_LEFT) {
      rv = EVENT_LEFT_LONG;
    } else if (st == BTN_RIGHT) {
      rv = EVENT_RIGHT_LONG;
    }
  } else {
    rv = EVENT_NO_EVENT;
  }
  return rv;
}

/* __________________________________BUTTONs: read the buttons phisical status */
BtnEvent_t OptaUnoR4Display::button_pressed() {
  static bool event_fired = false;
  static bool long_event_fired = false;
  static BtnStatus_t btn_st = BTN_IDLE;
  static int counter = 0;
  BtnStatus_t st = BTN_IDLE;
  static int counter_idle = 0;

  int a = analogRead(A0);
  if (a > 170 && a < 190) {
    st = BTN_DOWN;

    counter_idle = 0;
  } else if (a > 311 && a < 331) {
    st = BTN_RIGHT;
    counter_idle = 0;
  } else if (a > 600 && a < 620) {
    st = BTN_UP;
    counter_idle = 0;
  } else if (a > 830 && a < 850) {
    st = BTN_LEFT;
    counter_idle = 0;
  } else {
    st = BTN_IDLE;

    counter_idle++;
    counter = 0;
  }

  if (counter_idle > 50) {
    event_fired = false;
    long_event_fired = false;
  }

  if (btn_st != st) {
    counter = 0;
  } else {
    counter++;
  }
  btn_st = st;
  return fire_button_event(btn_st, counter, event_fired, long_event_fired);
}


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
                             I2C MESSAGES FUNCTIONS
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* _____________________________________________PARSE: get selected expansion */
bool OptaUnoR4Display::parse_get_selected_expansion() {
  return checkGetMsgReceived(rx_buffer, 
                             Req_GET_EXP,
                             Len_GET_EXP, 
                             GET_EXP_Len);
}

/* _______________________________________PREPARE ANS: get selected expansion */
uint8_t OptaUnoR4Display::msg_ans_selected_expansion() {
  tx_buffer[Ans_GET_EXP_ExpPos] = exp_selected;
  return prepareGetAns(tx_buffer, 
                       Ans_GET_EXP,
                       AnsLen_GET_EXP, 
                       Ans_GET_EXP_Len);
}

/* _______________________________________________________________PREPARE ACK */
uint8_t OptaUnoR4Display::msg_ack() {
  return prepareSetAns(tx_buffer, 
                       ACK_ARG,
                       Len_ACK, 
                       ACK_Len);
}
  
/* _____________________________________________PARSE: Set Expansion Features */
bool OptaUnoR4Display::parse_set_expansion_features() {
  if(checkSetMsgReceived(rx_buffer, 
                         Cmd_EXP_FEATURES,
                         Len_EXP_FEATURES, 
                         EXP_FEATURES_Len)) {
    if(use_expansion_features) {
      exp_selected_from_controller = rx_buffer[EXP_FEATURES_IndexPos];
      exp_type = rx_buffer[EXP_FEATURES_TypePos];
      exp_num_of_channels = rx_buffer[EXP_FEATURES_ChNumPos];
      use_expansion_features = false;
      for(int i = 0; i < MAX_CHANNEL_DISPLAYABLE; i++) {
         ch_cfg[i].type = CH_TYPE_NO_TYPE;
      }
    }

    

    return true;
  }

  return false;
}

/* _____________________________________________PARSE: Set Expansion Features */
bool OptaUnoR4Display::parse_set_num_of_expansion() {
  if(checkSetMsgReceived(rx_buffer, 
                         Cmd_EXP_NUM,
                         Len_EXP_NUM, 
                         EXP_NUM_Len)) {

    num_of_expansions = rx_buffer[EXP_NUM_NumPos];
    return true;
  }

  return false;
}

/* __________________________________________PARSE: Set Channel configuration */
bool OptaUnoR4Display::parse_set_ch_configuration() {
  if(checkSetMsgReceived(rx_buffer, 
                       Cmd_CH_CFG,
                       Len_CH_CFG, 
                       CH_CFG_Len)) {
    
    Float_u v;
    uint8_t ch = rx_buffer[CH_CFG_ChPos];
    if(ch < MAX_CHANNEL_DISPLAYABLE) {
      ch_cfg[ch].type = rx_buffer[CH_CFG_TypePos];
      
      v.bytes[0] = rx_buffer[CH_CFG_V1Pos_0];
      v.bytes[1] = rx_buffer[CH_CFG_V1Pos_1];
      v.bytes[2] = rx_buffer[CH_CFG_V1Pos_2];
      v.bytes[3] = rx_buffer[CH_CFG_V1Pos_3];
      ch_cfg[ch].values[0] = v.value;

      v.bytes[0] = rx_buffer[CH_CFG_V2Pos_0];
      v.bytes[1] = rx_buffer[CH_CFG_V2Pos_1];
      v.bytes[2] = rx_buffer[CH_CFG_V2Pos_2];
      v.bytes[3] = rx_buffer[CH_CFG_V2Pos_3];
      ch_cfg[ch].values[1] = v.value;

      ch_cfg[ch].units[0] = rx_buffer[CH_CFG_U1Pos];
      ch_cfg[ch].units[1] = rx_buffer[CH_CFG_U2Pos];
    }
    return true;
  }

  return false;
}

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
                             DRAWING FUNCTIONS
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


/* ________________________________________________________DRAW: welcome page */
void OptaUnoR4Display::draw_welcome_page() {
  display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  
  display.print(" Welcome! ");
  
  display.setTextSize(1);
  display.setCursor(0,16);

  display.println("R4 based expansion:");
  display.println("shows the status of ");
  display.println("other expansions");
  display.println();
  display.println("Use buttons to browse");

  display.display();
}

/* _________________________________________________DRAW: wait for expansions */
void OptaUnoR4Display::draw_wait_for_expansion_page() {

  display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  
  display.print(" ..wait..");
  
  display.setTextSize(1);
  display.setCursor(0,16);

  display.println("Wait for controller");
  display.println("to send available");
  display.println("expansions");
  display.println();
  display.println("Please wait...");

  display.display();

}

void OptaUnoR4Display::draw_wait_for_expansion_features() { 
display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  
  display.print(" ..wait..");
  
  display.setTextSize(1);
  display.setCursor(0,16);

  display.println("I am getting");
  display.println("selected expansion");
  display.println("features");
  display.println();
  display.println("Please wait...");

  display.display(); 
}

/* ______________________________________________ DRAW: select expansion page */
void OptaUnoR4Display::draw_select_expansion_menu(uint8_t n){
  display.clearDisplay();

  /* Expansion */
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F(" Expansion"));
  
  /* to  left arrow */
  display.fillTriangle(
      28 , 20,
      28,  51,
      2, 20+14, SSD1306_INVERSE);
  
  /* number of expansion */
  display.setTextSize(5);
  display.setCursor(53,19);
  display.print(n);
  
  /* to right arrow */
  display.fillTriangle(
      display.width() - 28 , 20,
      display.width() - 28,  51,
      display.width() - 2, 20+14, SSD1306_INVERSE);

  display.setTextSize(1);
  display.setCursor(0,display.height() - 7);
  display.print("hold 'down' to select");
  
  display.display();
}

/* ____________________________________________ DRAW: show expansion channels */
void OptaUnoR4Display::draw_expansion_page() {
  display.clearDisplay();

  /* Expansion */
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  
  Serial.println("Expansion type " +  String(exp_type));
  /* type of expansion */
  if(exp_type == EXPANSION_DIGITAL_INVALID) {
    display.print("Digital? ");
  }
  else if(exp_type == EXPANSION_OPTA_DIGITAL_MEC) {
    display.print("Dig MEC ");
  }
  else if(exp_type == EXPANSION_OPTA_DIGITAL_STS) {
    display.print("Dig STS ");
  }
  else if(exp_type == EXPANSION_OPTA_ANALOG) {
    display.print("Analog ");
  }
  else if(exp_type == UNO_R4_DISPLAY_ADDITIONAL_TYPE) {
    display.print("R4 Displ ");
  }

  /* number of expansion */
  display.println(exp_selected_from_controller);
  

  display.setTextSize(1);  
  display.setCursor(0,16);

  if(exp_num_of_channels < MAX_ROW_DISPLAYED_PER_PAGE) {
    final_row = exp_num_of_channels;
  }
  else {
    final_row = MAX_ROW_DISPLAYED_PER_PAGE;
  }
  
  while(selected_row < initial_row) {
    if(initial_row > 0) {
      initial_row--;
      final_row--;
    }
    else {
      selected_row = 0;
    }
  }
  if(selected_row >= exp_num_of_channels ) {
    selected_row = exp_num_of_channels - 1;
  }
  while(selected_row > final_row - 1) {
    if(final_row < exp_num_of_channels) {
      final_row++;
      initial_row++;
    }
  }
  Serial.println("selected row " + String(selected_row));
  for(int i = initial_row; i < final_row; i++) {
    if(selected_row == i) {
      display.print(">");
    }
    else {
      display.print(" ");
    }

    display.print(i);
    if(i < 10) {
      display.print(" ");
    }
    display.print(" ");
    if(ch_cfg[i].type == CH_TYPE_HIGH_IMPEDENCE ||
       ch_cfg[i].type == CH_TYPE_HIGH_IMPEDENCE_WITH_V_ADC) {
      display.print("HI ");
    }
    else if(ch_cfg[i].type == CH_TYPE_DIGITAL_IN ||
       ch_cfg[i].type == CH_TYPE_DIGITAL_IN_WITH_V_ADC ||
       ch_cfg[i].type == CH_TYPE_DIGITAL_IN_WITH_C_ADC) {
      display.print("DI ");
    }
    else if(ch_cfg[i].type == CH_TYPE_DIGITAL_OUT ||
       ch_cfg[i].type == CH_TYPE_DIGITAL_OUT_WITH_V_ADC ||
       ch_cfg[i].type == CH_TYPE_DIGITAL_OUT_WITH_C_ADC) {
      display.print("DO ");
    }
    else if(ch_cfg[i].type == CH_TYPE_V_ADC ||
       ch_cfg[i].type == CH_TYPE_V_ADC_WITH_C_ADC ) {
      display.print("VA ");
    }
    else if(ch_cfg[i].type == CH_TYPE_C_ADC ||
       ch_cfg[i].type == CH_TYPE_C_ADC_WITH_V_ADC ) {
      display.print("CA ");
    }
    else if(ch_cfg[i].type == CH_TYPE_PWM  ) {
      display.print("PW ");
    }
    else if(ch_cfg[i].type == CH_TYPE_RTD_2_WIRES ||
       ch_cfg[i].type == CH_TYPE_RTD_3_WIRES ) {
      display.print("RT ");
    }
    else if(ch_cfg[i].type == CH_TYPE_V_DAC ||
       ch_cfg[i].type == CH_TYPE_V_DAC_WITH_C_ADC ) {
      display.print("VD ");
    }
    else if(ch_cfg[i].type == CH_TYPE_C_DAC ||
      ch_cfg[i].type == CH_TYPE_C_DAC_WITH_V_ADC ) {
      display.print("CD ");
    }
    else if(ch_cfg[i].type == CH_TYPE_LED) {
      display.print("LE ");
    }
    else {
      display.print("-- ");
    }

    
    
    if(ch_cfg[i].units[0] == CH_UNIT_VOLT) {
      display.print(ch_cfg[i].values[0],1);
      display.print("V ");
    }
    else if(ch_cfg[i].units[0] == CH_UNIT_mVOLT) {
      display.print(ch_cfg[i].values[0],1);
      display.print("mV ");
    }
    else if(ch_cfg[i].units[0] == CH_UNIT_AMPERE) {
      display.print(ch_cfg[i].values[0],1);
      display.print("A ");
    }
    else if(ch_cfg[i].units[0] == CH_UNIT_mAMPERE) {
      display.print(ch_cfg[i].values[0],1);
      display.print("mA ");
    }
    else if(ch_cfg[i].units[0] == CH_UNIT_OHM) {
      display.print(ch_cfg[i].values[0],1);
      display.print("ohm ");
    }
    else if(ch_cfg[i].units[0] == CH_UNIT_DIGIT) {
      display.print(ch_cfg[i].values[0],0);
      display.print(" ");
    }
   
    
    if(ch_cfg[i].units[1] != CH_UNIT_NO_UNIT) {
    
      if(ch_cfg[i].units[1] == CH_UNIT_VOLT) {
        display.print(ch_cfg[i].values[1],1);
        display.print("V");
      }
      else if(ch_cfg[i].units[1] == CH_UNIT_mVOLT) {
        display.print(ch_cfg[i].values[1],1);
        display.print("mV");
      }
      else if(ch_cfg[i].units[1] == CH_UNIT_AMPERE) {
        display.print(ch_cfg[i].values[1],1);
        display.print("A");
      }
      else if(ch_cfg[i].units[1] == CH_UNIT_mAMPERE) {
        display.print(ch_cfg[i].values[1],1);
        display.print("mA");
      }
      else if(ch_cfg[i].units[1] == CH_UNIT_OHM) {
        display.print(ch_cfg[i].values[1],1);
        display.print("ohm");
      }
      else if(ch_cfg[i].units[1] == CH_UNIT_DIGIT) {
        display.print(ch_cfg[i].values[1],0);
        display.print("");
      }
    }
    display.println();

    

  } 
  display.println("Hold 'left' to back");
  display.display();

  
}

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
                             MAIN STATE MACHINE
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


typedef enum {
  STATE_WELCOME_DRAW,
  STATE_WELCOME_WAIT,
  STATE_WAIT_FOR_EXPANSION,
  STATE_SELECT_EXPANSION,
  STATE_SELECT_EXPANSION_WAIT_BUTTONS,
  STATE_WAIT_EXPANSION_FEATURES,
  STATE_SHOW_EXPANSION,
  STATE_SHOW_EXPANSION_BUTTON_HANDLE,
  STATE_MODIFY_EXPANSION_OUTPUT
} displayState_t;


#define WELCOME_PAGE_TIME 5000
#define REFRESH_STATUS 1000



/*_________________________________________________________MAIN STATE MACHINE */
/* handles different status of the display, showing different pages, depending
   upon the buttons pressed by the user */
void OptaUnoR4Display::main_state_machine() {
  static uint8_t current_expansion = 0;
  static displayState_t st = STATE_WELCOME_DRAW;
  static long long time = millis();
  /*
  for(int i = 0; i < exp_num_of_channels; i++) {
    Serial.print(i);
    Serial.print(" ");
    Serial.print("type " + String(ch_cfg[i].type) + " ");
    Serial.print("value1 " + String(ch_cfg[i].values[0]) + " ");
    Serial.print("unit1 " + String(ch_cfg[i].units[0]) + " ");
    Serial.print("value2 " + String(ch_cfg[i].values[2]) + " ");
    Serial.println("unit2 " + String(ch_cfg[i].units[2]) );
  }
  */
  //delay(1000);

  if(reset_state_machine) {
    current_expansion = 0;
    st = STATE_WELCOME_DRAW;
    reset_state_machine = false;
  }

  switch(st) {
  /*------------------------------------------------------------ Welcome page */ 
    case STATE_WELCOME_DRAW:
      //Serial.println("STATE_WELCOME_DRAW");
      draw_welcome_page();
      time = millis();
      st = STATE_WELCOME_WAIT;
    break;
  /*------------------------------------------------------------ Welcome wait */ 
    case STATE_WELCOME_WAIT:
      //Serial.println("STATE_WELCOME_WAIT");
      /* wait some time and then depending on the number of expansions make a 
         choice */
      if(millis() - time > WELCOME_PAGE_TIME) {
        if(num_of_expansions == 0) {
          st = STATE_WAIT_FOR_EXPANSION;
        }
        else {
          st = STATE_SELECT_EXPANSION;
        }
      }
    break;
  /*------------------------------------------------- Wait for expansion page */ 
    case STATE_WAIT_FOR_EXPANSION:
      //Serial.println("STATE_WAIT_FOR_EXPANSION");
      
      //Serial.println("n: " + String(num_of_expansions));
      if(num_of_expansions > 0) {
        st = STATE_SELECT_EXPANSION;
      }
      else {
        draw_wait_for_expansion_page();
      }
    break;
    /*--------------------------------------------------- Select an expansion */
    case STATE_SELECT_EXPANSION:
      use_expansion_features = false;
      exp_selected_from_controller = 255;
      //Serial.println("STATE_SELECT_EXPANSION");
      draw_select_expansion_menu(current_expansion);
      
      st = STATE_SELECT_EXPANSION_WAIT_BUTTONS;
      

    break;
    /*--------------------------------------Handle button to select expansion */
    case STATE_SELECT_EXPANSION_WAIT_BUTTONS:
      use_expansion_features = false;
      exp_selected_from_controller = 255;
      //Serial.println("STATE_SELECT_EXPANSION_WAIT_BUTTONS");
      if(btn_pressed == EVENT_LEFT) {
        current_expansion--;
        st = STATE_SELECT_EXPANSION;
      }
      if(btn_pressed == EVENT_RIGHT) {
        current_expansion++;
        st = STATE_SELECT_EXPANSION;
      }
      if(current_expansion == 255) {
        current_expansion = 0;
      }
      if(current_expansion >= num_of_expansions) {
        current_expansion = num_of_expansions - 1;
      }

      if(btn_pressed == EVENT_DOWN_LONG) {
        exp_selected = current_expansion;
        use_expansion_features = true;
        st = STATE_WAIT_EXPANSION_FEATURES;
      }
      break;

    case STATE_WAIT_EXPANSION_FEATURES:
      draw_wait_for_expansion_features();
      if(exp_selected_from_controller != 255) {
        st = STATE_SHOW_EXPANSION;
      }
      break;
      
    case STATE_SHOW_EXPANSION:
      draw_expansion_page();
      time = millis();
      
      st = STATE_SHOW_EXPANSION_BUTTON_HANDLE;
      

    break;
    case STATE_SHOW_EXPANSION_BUTTON_HANDLE:
      if(millis() - time > REFRESH_STATUS) {
        
          st = STATE_SHOW_EXPANSION;
        
      }
      if(btn_pressed == EVENT_DOWN) {
        selected_row++;
        st = STATE_SHOW_EXPANSION;
      }
      else if(btn_pressed == EVENT_UP) {
        selected_row--;
        st = STATE_SHOW_EXPANSION;
      }
      else if(btn_pressed == EVENT_LEFT_LONG) {
        st = STATE_WAIT_FOR_EXPANSION;
      }
      
      
      
      
      

    break;

    
    
    default:

    break;
  


  }

  /* the state machine consumed the event -> reset the button state */
  btn_pressed = EVENT_NO_EVENT;

}



void debugButtonEvent(BtnEvent_t ev) {
  if (ev != EVENT_NO_EVENT) {
    Serial.print("EVENT ");
    if (ev == EVENT_UP) {
      Serial.println("UP");

    } else if (ev == EVENT_DOWN) {
      Serial.println("DOWN");

    } else if (ev == EVENT_RIGHT) {
      Serial.println("RIGHT");

    } else if (ev == EVENT_LEFT) {
      Serial.println("LEFT");
    } else if (ev == EVENT_UP_LONG) {
      Serial.println("UP LONG");

    } else if (ev == EVENT_DOWN_LONG) {
      Serial.println("DOWN_LONG");

    } else if (ev == EVENT_RIGHT_LONG) {
      Serial.println("RIGHT_LONG");

    } else if (ev == EVENT_LEFT_LONG) {
      Serial.println("LEFT_LONG");
    }
  }
}





#endif
