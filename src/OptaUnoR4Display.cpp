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
    exp_selected(255),
    i2c_num_of_exp(0) ,
    i2c_exp_selected(255),
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
  //debugReceivedMsg();
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

/* ________________________________________________PARSE: get change ch value */
bool OptaUnoR4Display::parse_get_ch_change_value() {
  return checkGetMsgReceived(rx_buffer, 
                             Cmd_GET_CH_VALUE,
                             Len_GET_CH_VALUE, 
                             GET_CH_VALUE_Len);

}

/* __________________________________________PREPARE ANS: get change ch value */
uint8_t OptaUnoR4Display::msg_get_ch_change_value(){
  tx_buffer[Ans_GET_CH_VALUE_IndexPos] = i2c_ch_value_index;
  tx_buffer[Ans_GET_CH_VALUE_ExpTypePos] = i2c_ch_value_ex_type;
  tx_buffer[Ans_GET_CH_VALUE_ChannelPos] = i2c_ch_value_channel;
  Float_u v;
  v.value = i2c_ch_value_value;
  tx_buffer[Ans_GET_CH_VALUE_ValuePos + 0] = v.bytes[0];
  tx_buffer[Ans_GET_CH_VALUE_ValuePos + 1] = v.bytes[1];
  tx_buffer[Ans_GET_CH_VALUE_ValuePos + 2] = v.bytes[2];
  tx_buffer[Ans_GET_CH_VALUE_ValuePos + 3] = v.bytes[3];
  return prepareGetAns(tx_buffer, 
                       Ans_GET_CH_VALUE,
                       AnsLen_GET_CH_VALUE, 
                       Ans_GET_CH_VALUE_Len);

}

/* _______________________________________________PARSE: get change ch config */
bool OptaUnoR4Display::parse_get_ch_change_config(){
  
  return checkGetMsgReceived(rx_buffer, 
                             Cmd_GET_CH_CONFIG,
                             Len_GET_CH_CONFIG, 
                             GET_CH_CONFIG_Len);

}

/* _________________________________________PREPARE ANS: get change ch config */
uint8_t OptaUnoR4Display::msg_get_ch_change_config(){
  tx_buffer[Ans_GET_CH_CONFIG_IndexPos] = i2c_ch_config_index;
  tx_buffer[Ans_GET_CH_CONFIG_ExpTypePos] = i2c_ch_config_ex_type;
  tx_buffer[Ans_GET_CH_CONFIG_ChannelPos] = i2c_ch_config_channel;
  tx_buffer[Ans_GET_CH_CONFIG_ConfigPos] = i2c_ch_config_value;
  return prepareGetAns(tx_buffer, 
                       Ans_GET_CH_CONFIG,
                       AnsLen_GET_CH_CONFIG, 
                       Ans_GET_CH_CONFIG_Len);

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
    
    i2c_exp_selected = rx_buffer[EXP_FEATURES_IndexPos];
    i2c_exp_type = rx_buffer[EXP_FEATURES_TypePos];
    i2c_exp_channel_num = rx_buffer[EXP_FEATURES_ChNumPos];
    
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

    i2c_num_of_exp = rx_buffer[EXP_NUM_NumPos];
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

/* ___________Write the type of the expansion and the index on expansion page */
void OptaUnoR4Display::write_expansion_page_title() {
  /* Expansion */
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  
  Serial.println("Expansion type " +  String(exp_type));
  /* type of expansion */
  if(exp_type == EXPANSION_DIGITAL_INVALID) {
    display.print("Digital ");
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
  display.println(exp_selected);
}


void OptaUnoR4Display::write_cursor_and_ch_index(uint8_t r, uint8_t s) {
  if(r == s) {
      display.print(">");
  }
  else {
    display.print(" ");
  }
  display.print(r);
  if(r < 10) {
    display.print(" ");
  }
  display.print(" ");
}

uint8_t OptaUnoR4Display::how_many_row_take_channel(uint8_t ch) {
  uint8_t rv = 1;
  if(ch < MAX_CHANNEL_DISPLAYABLE) {
    if(ch_cfg[ch].units[1] != CH_UNIT_NO_UNIT) {
      rv++;
    }
  }
  else {
   rv =  0;
  }
  return rv;
}

void OptaUnoR4Display::display_new_value_channel(uint8_t ch, uint8_t index) {
  if(index >= 2) {
    return;
  }
  if(ch_cfg[ch].units[index] == CH_UNIT_VOLT) {
      display.print(new_ch_value,1);
      display.println("V ");   
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_mVOLT) {
    display.print(new_ch_value,1);
    display.println("mV ");
    
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_AMPERE) {
    display.print(new_ch_value,1);
    display.println("A ");
    
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_mAMPERE) {
    display.print(new_ch_value,1);
    display.println("mA ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_OHM) {
    display.print(new_ch_value,1);
    display.println("ohm ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_DIGIT) {
    display.print(new_ch_value,0);
    display.println(" ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_HERTZ) {
    display.print(new_ch_value,1);
    display.println("Hz ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_mHERTZ) {
    display.print(new_ch_value,1);
    display.println("mHz ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_KHERTZ) {
    display.print(new_ch_value,1);
    display.println("KHz ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_MHERTZ) {
    display.print(new_ch_value,1);
    display.println("MHz ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_PERC) {
    display.print(new_ch_value,1);
    display.println("% ");
  }
  else if(new_ch_value == CH_UNIT_NO_VALUE) {
    display.println(" ");
  }
}

void OptaUnoR4Display::display_value_of_channel(uint8_t ch, uint8_t index) {
  if(index >= 2) {
    return;
  }
  if(ch_cfg[ch].units[index] == CH_UNIT_VOLT) {
      display.print(ch_cfg[ch].values[index],1);
      display.println("V ");   
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_mVOLT) {
    display.print(ch_cfg[ch].values[index],1);
    display.println("mV ");
    
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_AMPERE) {
    display.print(ch_cfg[ch].values[index],1);
    display.println("A ");
    
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_mAMPERE) {
    display.print(ch_cfg[ch].values[index],1);
    display.println("mA ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_OHM) {
    display.print(ch_cfg[ch].values[index],1);
    display.println("ohm ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_DIGIT) {
    display.print(ch_cfg[ch].values[index],0);
    display.println(" ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_HERTZ) {
    display.print(ch_cfg[ch].values[index],1);
    display.println("Hz ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_mHERTZ) {
    display.print(ch_cfg[ch].values[index],1);
    display.println("mHz ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_KHERTZ) {
    display.print(ch_cfg[ch].values[index],1);
    display.println("KHz ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_MHERTZ) {
    display.print(ch_cfg[ch].values[index],1);
    display.println("MHz ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_PERC) {
    display.print(ch_cfg[ch].values[index],1);
    display.println("% ");
  }
  else if(ch_cfg[ch].units[index] == CH_UNIT_NO_VALUE) {
    display.println(" ");
  }
}


/* write on the display the row r (1 or 2) of the channel ch 
   return true if the row is written (consumed) */
bool OptaUnoR4Display::display_row_channel(uint8_t ch, uint8_t r) {
  if(ch >= MAX_CHANNEL_DISPLAYABLE) {
    return false;
  }
  
  if(r == 1) {

    /* channel type  */
    if(ch_cfg[ch].type == CH_TYPE_HIGH_IMPEDENCE ||
       ch_cfg[ch].type == CH_TYPE_HIGH_IMPEDENCE_WITH_V_ADC) {
      display.print("HIM  ");
    }
    else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_IN ||
       ch_cfg[ch].type == CH_TYPE_DIGITAL_IN_WITH_V_ADC ||
       ch_cfg[ch].type == CH_TYPE_DIGITAL_IN_WITH_C_ADC) {
      display.print("DIN  ");
    }
    else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT ||
       ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT_WITH_V_ADC ||
       ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT_WITH_C_ADC) {
      display.print("DOUT ");
    }
    else if(ch_cfg[ch].type == CH_TYPE_V_ADC ||
       ch_cfg[ch].type == CH_TYPE_V_ADC_WITH_C_ADC ) {
      display.print("VADC ");
    }
    else if(ch_cfg[ch].type == CH_TYPE_C_ADC ||
       ch_cfg[ch].type == CH_TYPE_C_ADC_WITH_V_ADC ) {
      display.print("CADC ");
    }
    else if(ch_cfg[ch].type == CH_TYPE_PWM  ) {
      display.print("PWM  ");
    }
    else if(ch_cfg[ch].type == CH_TYPE_RTD_2_WIRES ||
       ch_cfg[ch].type == CH_TYPE_RTD_3_WIRES ) {
      display.print("RTD  ");
    }
    else if(ch_cfg[ch].type == CH_TYPE_V_DAC ||
       ch_cfg[ch].type == CH_TYPE_V_DAC_WITH_C_ADC ) {
      display.print("VDAC ");
    }
    else if(ch_cfg[ch].type == CH_TYPE_C_DAC ||
      ch_cfg[ch].type == CH_TYPE_C_DAC_WITH_V_ADC ) {
      display.print("CDAC ");
    }
    else if(ch_cfg[ch].type == CH_TYPE_LED) {
      display.print("LED  ");
    }
    else {
      display.println("---- ");
      return true;
    }
    display_value_of_channel(ch, 0);
    return true;

  }
  else if(r == 2) {
    if(ch_cfg[ch].units[1] != CH_UNIT_NO_UNIT) {
      display.print("         ");
      display_value_of_channel(ch, 1);
      return true;
    }

  }
  return false;
}


/* ____________________________________________ DRAW: show expansion channels */
void OptaUnoR4Display::draw_expansion_page() {
  static uint8_t first_ch_displayed = 0;
  static uint8_t last_ch_displayed = 0;

  display.clearDisplay();

  write_expansion_page_title();

  display.setTextSize(1);  
  display.setCursor(0,16);

  Serial.println("selected row: " + String(selected_channel) + " first: " + String(first_ch_displayed) + " last: " + String(last_ch_displayed));
  
  while(selected_channel >= exp_channel_num) {
    selected_channel--;
  }
  
  while(selected_channel < 0) {
    selected_channel++;
  }

  if(selected_channel > last_ch_displayed) {
    first_ch_displayed++;
  }

  if(selected_channel < first_ch_displayed) {
    first_ch_displayed--;
  }
  
  int r = -1;
  uint8_t current_ch_displayed = first_ch_displayed;;
  while(current_ch_displayed < exp_channel_num) {
    Serial.println("  current ch: " + String(current_ch_displayed));
    write_cursor_and_ch_index(current_ch_displayed, selected_channel);
    
    if(display_row_channel(current_ch_displayed, 1)) {
      r++;
    }
    if(display_row_channel(current_ch_displayed, 2)) {
      r++;
    }
    last_ch_displayed = current_ch_displayed;
    current_ch_displayed++;

    Serial.println("  row = " + String(r));

    if(how_many_row_take_channel(current_ch_displayed) == 0 || r + how_many_row_take_channel(current_ch_displayed) >= 5) {
      break;
    }
  }
  if(r == 3) {
    display.println(" ");
  }

  display.println("left back, right select");
  display.display();
  
}

/* returns true if the channel is changeable */
bool OptaUnoR4Display::display_channel_info(uint8_t ch) {
  bool rv = false;
  
  if(ch_cfg[ch].type == CH_TYPE_HIGH_IMPEDENCE) {
    display.println("+ High impedence");
  }
  else if(ch_cfg[ch].type == CH_TYPE_HIGH_IMPEDENCE_WITH_V_ADC) {
    display.println("+ High impedence");
    display.print("+ Voltage ADC: ");
    display_value_of_channel(ch, 1);
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_IN) {
    display.print("+ Digital IN: ");
    display_value_of_channel(ch, 0);
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_IN_WITH_V_ADC) {
    display.print("+ Digital IN: ");
    display_value_of_channel(ch, 0); 
    display.print("+ Voltage ADC: ");
    display_value_of_channel(ch, 1);
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_IN_WITH_C_ADC) {
    display.print("+ Digital IN: ");
    display_value_of_channel(ch, 0); 
    display.print("+ Current ADC: ");
    display_value_of_channel(ch, 1);
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT) {
    display.print("+ Digital OUT: ");
    display_value_of_channel(ch, 0); 
    rv = true;
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT_WITH_V_ADC) {
    display.print("+ Digital OUT: ");
    display_value_of_channel(ch, 0); 
    display.print("+ Voltage ADC: ");
    display_value_of_channel(ch, 1);
    rv = true;
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT_WITH_C_ADC) {
    display.print("+ Digital OUT: ");
    display_value_of_channel(ch, 0); 
    display.print("+ Current ADC: ");
    display_value_of_channel(ch, 1);
    rv = true;
  }
  else if(ch_cfg[ch].type == CH_TYPE_V_ADC) {
    display.print("+ Voltage ADC: ");
    display_value_of_channel(ch, 0);  
  }
  else if(ch_cfg[ch].type == CH_TYPE_C_ADC) {
    display.print("+ Current ADC: ");
    display_value_of_channel(ch, 0); 
  }
  else if(ch_cfg[ch].type == CH_TYPE_V_ADC_WITH_C_ADC) {
    display.print("+ Voltage ADC: ");
    display_value_of_channel(ch, 0); 
    display.print("+ Current ADC: ");
    display_value_of_channel(ch, 1);
  }
  else if(ch_cfg[ch].type == CH_TYPE_C_ADC_WITH_V_ADC) {
    display.print("+ Current ADC: ");
    display_value_of_channel(ch, 0); 
    display.print("+ Voltage ADC: ");
    display_value_of_channel(ch, 1);
  }
  else if(ch_cfg[ch].type == CH_TYPE_PWM) {
    display.print("+ PWM Freq: ");
    display_value_of_channel(ch, 0); 
    display.print("+ PWM Duty: ");
    display_value_of_channel(ch, 1);
    rv = true;
  }
  else if(ch_cfg[ch].type == CH_TYPE_RTD_2_WIRES) {
    display.print("+ RTD: ");
    display_value_of_channel(ch, 0); 
  }
  else if(ch_cfg[ch].type == CH_TYPE_RTD_3_WIRES) {
    display.print("+ RTD: ");
    display_value_of_channel(ch, 0); 
  }
  else if(ch_cfg[ch].type == CH_TYPE_V_DAC) {
    display.print("+ Voltage DAC: ");
    display_value_of_channel(ch, 0); 
    rv = true;
  }
  else if(ch_cfg[ch].type == CH_TYPE_C_DAC) {
    display.print("+ Current DAC: ");
    display_value_of_channel(ch, 0); 
    rv = true;
  }
  else if(ch_cfg[ch].type == CH_TYPE_V_DAC_WITH_C_ADC) {
    display.print("+ Voltage DAC: ");
    display_value_of_channel(ch, 0); 
    display.print("+ Current ADC: ");
    display_value_of_channel(ch, 1);  
    rv = true;
  }
  else if(ch_cfg[ch].type == CH_TYPE_C_DAC_WITH_V_ADC) {
    display.print("+ Current DAC: ");
    display_value_of_channel(ch, 0); 
    display.print("+ Voltage ADC: ");
    display_value_of_channel(ch, 1);   
    rv = true; 
  }
  else if(ch_cfg[ch].type == CH_TYPE_LED) {
    display.print("+ LED, ");
    display_value_of_channel(ch, 0);  
    rv = true;
  }
  return rv;
}


void OptaUnoR4Display::draw_change_channel_page(uint8_t ch) {
  display.clearDisplay();

  write_expansion_page_title();

  display.setTextSize(1);  
  display.setCursor(0,18);

  display.print("- Selected ch: ");
  display.println(ch);

  if(display_channel_info(ch)) {
    if(ch_cfg[ch].type == CH_TYPE_PWM) {
      display.println("Change freg: up");
      display.println("Change duty: down");
    }
    else {
      display.println("right/down ch/value");
      display.println("Go back: left");
    }
  }

  display.display();

}

float OptaUnoR4Display::get_min_ch_value(uint8_t ex, uint8_t ch, uint8_t special) {
  if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT) {
    return 0.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT_WITH_V_ADC) {
    return 0.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT_WITH_C_ADC) {
    return 0.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_PWM) {
    if(special == SPECIAL_CH_VALUE_PWM_FREQ) {
      return 1.0;
    }
    else if(special == SPECIAL_CH_VALUE_PWM_DUTY) {
      return 0.0;
    }
    return 0.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_V_DAC) {
    if(ex == EXPANSION_OPTA_ANALOG) {
      return 0.0;
    } 
  }
  else if(ch_cfg[ch].type == CH_TYPE_C_DAC) {
    if(ex == EXPANSION_OPTA_ANALOG) {
      return 0.0;
    }
  }
  else if(ch_cfg[ch].type == CH_TYPE_V_DAC_WITH_C_ADC) {
     if(ex == EXPANSION_OPTA_ANALOG) {
      return 0.0;
    } 
  }
  else if(ch_cfg[ch].type == CH_TYPE_C_DAC_WITH_V_ADC) {
    if(ex == EXPANSION_OPTA_ANALOG) {
      return 0.0;
    }
  }
  else if(ch_cfg[ch].type == CH_TYPE_LED) {
    return 0.0;
  }
  return 0.0;
}



float OptaUnoR4Display::get_max_ch_value(uint8_t ex, uint8_t ch, uint8_t special) {
  if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT) {
    return 1.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT_WITH_V_ADC) {
    return 1.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT_WITH_C_ADC) {
    return 1.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_PWM) {
    if(special == SPECIAL_CH_VALUE_PWM_FREQ) {
      return 500.0;
    }
    else if(special == SPECIAL_CH_VALUE_PWM_DUTY) {
      return 100.0;
    }
    return 0.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_V_DAC) {
    if(ex == EXPANSION_OPTA_ANALOG) {
      return 11.0;
    } 
  }
  else if(ch_cfg[ch].type == CH_TYPE_C_DAC) {
    if(ex == EXPANSION_OPTA_ANALOG) {
      return 25.0;
    }
  }
  else if(ch_cfg[ch].type == CH_TYPE_V_DAC_WITH_C_ADC) {
     if(ex == EXPANSION_OPTA_ANALOG) {
      return 11.0;
    } 
  }
  else if(ch_cfg[ch].type == CH_TYPE_C_DAC_WITH_V_ADC) {
    if(ex == EXPANSION_OPTA_ANALOG) {
      return 25.0;
    }
  }
  else if(ch_cfg[ch].type == CH_TYPE_LED) {
    return 1.0;
  }
  return 0.0;
}
  
float OptaUnoR4Display::get_step_ch_value(uint8_t ex, uint8_t ch, uint8_t special) {
  if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT) {
    return 1.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT_WITH_V_ADC) {
    return 1.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT_WITH_C_ADC) {
    return 1.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_PWM) {
    if(special == SPECIAL_CH_VALUE_PWM_FREQ) {
      return 1.0;
    }
    else if(special == SPECIAL_CH_VALUE_PWM_DUTY) {
      return 1.0;
    }
    return 0.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_V_DAC) {
    if(ex == EXPANSION_OPTA_ANALOG) {
      return 0.1;
    } 
  }
  else if(ch_cfg[ch].type == CH_TYPE_C_DAC) {
    if(ex == EXPANSION_OPTA_ANALOG) {
      return 0.1;
    }
  }
  else if(ch_cfg[ch].type == CH_TYPE_V_DAC_WITH_C_ADC) {
     if(ex == EXPANSION_OPTA_ANALOG) {
      return 0.1;
    } 
  }
  else if(ch_cfg[ch].type == CH_TYPE_C_DAC_WITH_V_ADC) {
    if(ex == EXPANSION_OPTA_ANALOG) {
      return 0.1;
    }
  }
  else if(ch_cfg[ch].type == CH_TYPE_LED) {
    return 1.0;
  }
  return 0.0;

}

float OptaUnoR4Display::get_long_step_ch_value(uint8_t ex, uint8_t ch,  uint8_t special) {
  if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT) {
    return 0.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT_WITH_V_ADC) {
    return 0.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_DIGITAL_OUT_WITH_C_ADC) {
    return 0.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_PWM) {
    if(special == SPECIAL_CH_VALUE_PWM_FREQ) {
      return 10.0;
    }
    else if(special == SPECIAL_CH_VALUE_PWM_DUTY) {
      return 5.0;
    }
    return 0.0;
  }
  else if(ch_cfg[ch].type == CH_TYPE_V_DAC) {
    if(ex == EXPANSION_OPTA_ANALOG) {
      return 1.0;
    } 
  }
  else if(ch_cfg[ch].type == CH_TYPE_C_DAC) {
    if(ex == EXPANSION_OPTA_ANALOG) {
      return 1.0;
    }
  }
  else if(ch_cfg[ch].type == CH_TYPE_V_DAC_WITH_C_ADC) {
     if(ex == EXPANSION_OPTA_ANALOG) {
      return 1.0;
    } 
  }
  else if(ch_cfg[ch].type == CH_TYPE_C_DAC_WITH_V_ADC) {
    if(ex == EXPANSION_OPTA_ANALOG) {
      return 1.0;
    }
  }
  else if(ch_cfg[ch].type == CH_TYPE_LED) {
    return 0.0;
  }
  return 0.0;
}

void OptaUnoR4Display::draw_change_value_page(uint8_t ch, uint8_t special) {
  display.clearDisplay();

  write_expansion_page_title();

  display.setTextSize(1);  
  display.setCursor(0,18);

  display.print("- Selected ch: ");
  display.println(ch);
  display.println("Up/Down change value");
  display.print("New value: ");
  display_new_value_channel(ch,special); 
  display.println("Right confirm");
  display.println("Left back");
  

  display.display();

}


void OptaUnoR4Display::display_change_ch_config(uint8_t ex, uint8_t ch) {
  display.clearDisplay();

  write_expansion_page_title();

  display.setTextSize(1);  
  display.setCursor(0,18);

  display.print("- Selected ch: ");
  display.println(ch);



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
  STATE_CHANGE_CHANNEL,
  STATE_CHANGE_CHANNEL_BUTTON_HANDLE,
  STATE_CHANGE_VALUE,
  STATE_CHANGE_VALUE_BUTTON_HANDLE
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
  static uint8_t special = SPECIAL_CH_VALUE_NO_SPECIAL;

  float min_val = 0.0;
  float max_val = 0.0;
  float inc_val = 0.0;
  float long_inc_val = 0.0;
  /*
  for(int i = 0; i < exp_channel_num; i++) {
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
  /* ________________________________________________________________________ */  
  /* Show the welcome page and immediately go in STATE_WELCOME_WAIT */ 
    case STATE_WELCOME_DRAW:
      draw_welcome_page();
      time = millis();
      st = STATE_WELCOME_WAIT;
    break;
  /* ________________________________________________________________________ */
  /* Wait some time to display the Welcome page, when WELCOME_PAGE_TIME it checks
     if the number of expansion communicated by the controller, if greater than
     0 it goes in STATE_SELECT_EXPANSION otherwise it goes in 
     STATE_WAIT_FOR_EXPANSION */ 
    case STATE_WELCOME_WAIT:
      if(millis() - time > WELCOME_PAGE_TIME) {
        st = (i2c_num_of_exp == 0) ? STATE_WAIT_FOR_EXPANSION : STATE_SELECT_EXPANSION;
      }
    break;
  /* ________________________________________________________________________ */
  /* Wait for the controller to send the number of expansion present, as soon as
     this happens (and the number is > 0) it goes in STATE_SELECT_EXPANSION */ 
    case STATE_WAIT_FOR_EXPANSION:
      if(i2c_num_of_exp > 0) {
        st = STATE_SELECT_EXPANSION;
      }
      else {
        draw_wait_for_expansion_page();
      }
    break;
  /* ________________________________________________________________________ */
  /* Reset the selected expansion to a non valid number (255) 
     Reset the list of channel in order to be ready for the next selected expansion
     Shows the page to select an expansion  */ 
    case STATE_SELECT_EXPANSION:
      exp_selected = 255;
      for(int i = 0; i < MAX_CHANNEL_DISPLAYABLE; i++) {
         ch_cfg[i].type = CH_TYPE_NO_TYPE;
      }
      draw_select_expansion_menu(current_expansion);
      st = STATE_SELECT_EXPANSION_WAIT_BUTTONS;
    break;
  /* ________________________________________________________________________ */
  /* handle the button left and right, every time a button is pressed the 
     current_expansion is updated and the state is set to STATE_SELECT_EXPANSION
     to show the page with the updated number */
    case STATE_SELECT_EXPANSION_WAIT_BUTTONS:
      if(btn_pressed == EVENT_LEFT) {
        current_expansion--;
        st = STATE_SELECT_EXPANSION;
      }
      if(btn_pressed == EVENT_RIGHT) {
        current_expansion++;
        st = STATE_SELECT_EXPANSION;
      }
      
      current_expansion = (current_expansion == 255) ? 0 : current_expansion;
      current_expansion = (current_expansion >= i2c_num_of_exp) ? i2c_num_of_exp - 1 : current_expansion;
      
      /* a long press on the DOWN button trigger the change of status in STATE_WAIT_EXPANSION_FEATURES */
      if(btn_pressed == EVENT_DOWN_LONG) {
        exp_selected = current_expansion;
        
        st = STATE_WAIT_EXPANSION_FEATURES;
      }
      break;
  /* ________________________________________________________________________ */
  /* wait for expansion feature message sent by the controller */
    case STATE_WAIT_EXPANSION_FEATURES:
      exp_selected = current_expansion;
      draw_wait_for_expansion_features();
      /* as soon as the message arrives "it confirms" the expansion selected 
         so the feature are copied */
      if(i2c_exp_selected == exp_selected) {
        st = STATE_SHOW_EXPANSION;
        exp_type = i2c_exp_type;
        exp_channel_num = i2c_exp_channel_num;
      }
      break;
  /* ________________________________________________________________________ */  
  /* Show the status of the expansion channel                                 */  
    case STATE_SHOW_EXPANSION:
      channel_selected_to_change = 255;
      draw_expansion_page();
      time = millis();
      st = STATE_SHOW_EXPANSION_BUTTON_HANDLE;
    break;
  /* ________________________________________________________________________ */  
  /* handle the button pressed when channels status are displayed              */   
    case STATE_SHOW_EXPANSION_BUTTON_HANDLE:
      if(millis() - time > REFRESH_STATUS) {
          st = STATE_SHOW_EXPANSION;
      }
      if(btn_pressed == EVENT_DOWN) {
        selected_channel++;
        st = STATE_SHOW_EXPANSION;
      }
      else if(btn_pressed == EVENT_UP) {
        selected_channel--;
        st = STATE_SHOW_EXPANSION;
      }
      else if(btn_pressed == EVENT_LEFT_LONG) {
        st = STATE_WAIT_FOR_EXPANSION;
      }
      else if(btn_pressed == EVENT_RIGHT_LONG) {
        Serial.println("LONG RIGHT PRESSURE");
        channel_selected_to_change = selected_channel;
        new_ch_value = ch_cfg[channel_selected_to_change].values[0];
        st = STATE_CHANGE_CHANNEL;
      }
    break;
  /* ________________________________________________________________________ */  
  /* display status / change of single channel                                */   
    case STATE_CHANGE_CHANNEL:
      Serial.println("CHANGE CHANNEL");
      draw_change_channel_page(channel_selected_to_change);
      st = STATE_CHANGE_CHANNEL_BUTTON_HANDLE;
    break;
  /* ________________________________________________________________________ */
  /* handle the button pressed when channel status are displayed              */   
    case STATE_CHANGE_CHANNEL_BUTTON_HANDLE:
    if(btn_pressed == EVENT_LEFT_LONG) {
        st = STATE_SHOW_EXPANSION;
    }
    else if(ch_cfg[channel_selected_to_change].type == CH_TYPE_PWM) {
      if(btn_pressed == EVENT_UP_LONG) {
        special = SPECIAL_CH_VALUE_PWM_FREQ;
        st = STATE_CHANGE_VALUE;
        
      }
      else if(btn_pressed == EVENT_DOWN_LONG) {
        special = SPECIAL_CH_VALUE_PWM_DUTY;
        st = STATE_CHANGE_VALUE;
      }
    }
    else if(btn_pressed == EVENT_RIGHT_LONG) {
      special = SPECIAL_CH_VALUE_NO_SPECIAL;
      st = STATE_CHANGE_VALUE;
    }
    else if(btn_pressed == EVENT_DOWN_LONG) {
      if(exp_type == EXPANSION_OPTA_ANALOG) {

      }
      
    }
    break;

  case STATE_CHANGE_VALUE:
    draw_change_value_page(channel_selected_to_change, special);
    st = STATE_CHANGE_VALUE_BUTTON_HANDLE;
    break;
  /* ________________________________________________________________________ */
    case STATE_CHANGE_VALUE_BUTTON_HANDLE:
      min_val = get_min_ch_value(exp_type, channel_selected_to_change, special);
      max_val = get_max_ch_value(exp_type, channel_selected_to_change, special);
      inc_val = get_step_ch_value(exp_type, channel_selected_to_change,  special);
      long_inc_val = get_long_step_ch_value(exp_type, channel_selected_to_change,special);
      if(btn_pressed == EVENT_DOWN) {
        new_ch_value -= inc_val;
        st = STATE_CHANGE_VALUE;
      }
      else if(btn_pressed == EVENT_DOWN_LONG) {
        new_ch_value -= btn_pressed;
        st = STATE_CHANGE_VALUE;
      }
      else if(btn_pressed == EVENT_UP) {
        new_ch_value += inc_val;
        st = STATE_CHANGE_VALUE;
      }
      else if(btn_pressed == EVENT_UP_LONG) {
        new_ch_value += long_inc_val;
        st = STATE_CHANGE_VALUE;
      }
      else if(btn_pressed == EVENT_LEFT_LONG) {
        st = STATE_SHOW_EXPANSION;
      }

      new_ch_value = (new_ch_value < min_val) ? min_val : new_ch_value;
      new_ch_value = (new_ch_value > max_val) ? max_val : new_ch_value;



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
