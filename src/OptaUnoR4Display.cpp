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
/* >>>>>>>>>>>>>>>>
 * > Instructions: 
 * >>>>>>>>>>>>>>>>
 * CONSTRUCTOR: always call the base Module constructor specifying
 * - TwoWire Object to be used
 * - DETECT IN pin
 * - DETECT OUT pin */

/*_________________________________________________________________CONSTUCTOR */
OptaUnoR4Display::OptaUnoR4Display()
    : Module(&Wire1, DETECT_IN, DETECT_OUT), 
    btn_pressed(EVENT_NO_EVENT),
    display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET), 
    i2c_exp_selected_transmitted(255),
    i2c_num_of_exp(0) ,
    i2c_exp_selected_received(255),
    reset_state_machine(false),
    function_to_changed(nullptr) {
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

#ifdef DEBUG_BUTTONS
void debugButtonEvent(BtnEvent_t ev);
#endif


/*_________________________________________________________override: update() */
void OptaUnoR4Display::update() {
  /* Always call base methods */
  Module::update();
  
  BtnEvent_t ev = button_pressed();
  if (btn_pressed == EVENT_NO_EVENT) {
    btn_pressed = ev;
  }

  #ifdef DEBUG_I2C_MESSAGES
  debugReceivedMsg();
  #endif
  #ifdef DEBUG_BUTTONS
  debugButtonEvent(btn_pressed);
  #endif

  /* manage buttons and show page on the display */
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
    #ifdef DEBUG_I2C_MESSAGES
    debug_msg = 1;
    #endif
    return msg_ans_selected_expansion();
  } else if(parse_set_expansion_features()) {
    #ifdef DEBUG_I2C_MESSAGES
    debug_msg = 2;
    #endif
    return msg_ack();;
  } else if(parse_set_ch_configuration()) {
    #ifdef DEBUG_I2C_MESSAGES
    debug_msg = 3;
    #endif
    return msg_ack();;
  } else if(parse_set_num_of_expansion()) {
    #ifdef DEBUG_I2C_MESSAGES
    debug_msg = 4;
    #endif
    return msg_ack();;
  } else if(parse_get_ch_change_value()) {
    #ifdef DEBUG_I2C_MESSAGES
    debug_msg = 5;
    #endif
    return msg_get_ch_change_value();
  } else if(parse_get_ch_change_config()) {
    #ifdef DEBUG_I2C_MESSAGES
    debug_msg = 6;
    #endif
    return msg_get_ch_change_config();
  }
  else {
    #ifdef DEBUG_I2C_MESSAGES
    debug_msg = 25;
    #endif
  }

  return rv;
}

#ifdef DEBUG_I2C_MESSAGES
void OptaUnoR4Display::debugReceivedMsg() {
  if (debug_msg == 1) {
    Serial.println("RX: get selected expansion");
  } else if(debug_msg == 2) {
    Serial.println("RX: set Expansion features");  
  } else if(debug_msg == 3) {
    Serial.println("RX: set Channel configuration");
  } else if(debug_msg == 4) {
    Serial.println("RX: set num of expansion");
  } else if(debug_msg == 5) {
    Serial.println("RX: get change ch value");
  } else if(debug_msg == 6) {
    Serial.println("RX: get change ch config");
  } else if(debug_msg == 25) {
  
  }
  debug_msg = 0;
}
#endif

/* >>>>>>>>>>>>>>>>
 * > Instructions: 
 * >>>>>>>>>>>>>>>>
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

/* _________________________________BUTTONs: read the buttons phisical status */
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
  tx_buffer[Ans_GET_EXP_ExpPos] = i2c_exp_selected_transmitted;
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
  tx_buffer[Ans_GET_CH_VALUE_IndexPos] = i2c_change_expansion_index;
  tx_buffer[Ans_GET_CH_VALUE_ExpTypePos] = i2c_change_expansion_type;
  tx_buffer[Ans_GET_CH_VALUE_ChannelPos] = i2c_change_channel_index;
  Float_u v;
  v.value = i2c_change_channel_value;
  tx_buffer[Ans_GET_CH_VALUE_ValuePos + 0] = v.bytes[0];
  tx_buffer[Ans_GET_CH_VALUE_ValuePos + 1] = v.bytes[1];
  tx_buffer[Ans_GET_CH_VALUE_ValuePos + 2] = v.bytes[2];
  tx_buffer[Ans_GET_CH_VALUE_ValuePos + 3] = v.bytes[3];
  i2c_change_expansion_index = 255;
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
  tx_buffer[Ans_GET_CH_CONFIG_IndexPos] = i2c_change_expansion_index;
  tx_buffer[Ans_GET_CH_CONFIG_ExpTypePos] = i2c_change_expansion_type;
  tx_buffer[Ans_GET_CH_CONFIG_ChannelPos] = i2c_change_channel_index;
  tx_buffer[Ans_GET_CH_CONFIG_ConfigPos] = i2c_change_channel_config;
  i2c_change_expansion_index = 255;
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
    
    i2c_exp_selected_received = rx_buffer[EXP_FEATURES_IndexPos];
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
    if(ch < MAX_NUMBER_OF_CHANNELS && !do_not_update_values_from_controller) {

      channels[ch].makeFunction(0, 
                                rx_buffer[CH_CFG_Func1Pos], 
                                rx_buffer[CH_CFG_Type1Pos], 
                                rx_buffer[CH_CFG_U1Pos]);
      channels[ch].makeFunction(1, 
                                rx_buffer[CH_CFG_Func2Pos], 
                                rx_buffer[CH_CFG_Type2Pos], 
                                rx_buffer[CH_CFG_U2Pos]);

      v.bytes[0] = rx_buffer[CH_CFG_V1Pos_0];
      v.bytes[1] = rx_buffer[CH_CFG_V1Pos_1];
      v.bytes[2] = rx_buffer[CH_CFG_V1Pos_2];
      v.bytes[3] = rx_buffer[CH_CFG_V1Pos_3];

      channels[ch].setValue(0, v.value);
      

      v.bytes[0] = rx_buffer[CH_CFG_V2Pos_0];
      v.bytes[1] = rx_buffer[CH_CFG_V2Pos_1];
      v.bytes[2] = rx_buffer[CH_CFG_V2Pos_2];
      v.bytes[3] = rx_buffer[CH_CFG_V2Pos_3];
      channels[ch].setValue(1, v.value);
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

/* ____________________________________________________DRAW: wait for expansions 
   This page is displayed if the expansion has not yet sent the number of 
   expansion to the R4 display                                                */
void OptaUnoR4Display::draw_wait_for_expansion_page() {

  display.clearDisplay();
  /* TITLE of the page */
  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE);        
  display.setCursor(0,0);             
  display.print(" ..wait..");
  
  /* "body" of the page */
  display.setTextSize(1);
  display.setCursor(0,16);
  display.println("Wait for controller");
  display.println("to send available");
  display.println("expansions");
  display.println();
  display.println("Please wait...");

  display.display();
}



/*___ _________________________________________________DRAW: wait for expansions 
  This page is displayed in case there is a delay between the selection of the
  expansion and the controller sending information about that expansion       */
void OptaUnoR4Display::draw_wait_for_expansion_features() { 
display.clearDisplay();

  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE);        
  display.setCursor(0,0);             
  
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
  display.setTextSize(2);            
  display.setTextColor(SSD1306_WHITE);        
  display.setCursor(0,0);
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

/* __Write the type of the expansion and the index on expansion page as title */
void OptaUnoR4Display::display_expansion_type_as_title() {
  /* Expansion */
  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE);        
  display.setCursor(0,0);
  
  if(dexp != nullptr) {
     dexp->display(display,exp_selected);
  }
}

/* Display a > indicating that that is the current selection and the number 
   of the selection (r is the current row, s is the selected one) */
void OptaUnoR4Display::display_cursor_selection(uint8_t r, uint8_t s) {
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

/* ____________________________________________ DRAW: show expansion channels */
void OptaUnoR4Display::draw_expansion_page() {
  static uint8_t first_ch_displayed = 0;
  static uint8_t last_ch_displayed = 0;

  display.clearDisplay();

  display_expansion_type_as_title();

  display.setTextSize(1);  
  display.setCursor(0,16);

  //Serial.println("selected row: " + String(selected_channel) + " first: " + String(first_ch_displayed) + " last: " + String(last_ch_displayed));
  
  while(selected_channel >= channels_number) {
    selected_channel--;
  }
  
  while(selected_channel < 0) {
    selected_channel++;
  }

  if(selected_channel > last_ch_displayed) {
    first_ch_displayed++;
  }

  while(selected_channel < first_ch_displayed) {
    first_ch_displayed--;
  }
  
  int r = -1;
  uint8_t current_ch_displayed = first_ch_displayed;;
  while(current_ch_displayed < channels_number) {
    display_cursor_selection(current_ch_displayed, selected_channel);
    
    if(current_ch_displayed < channels_number) {
      channels[current_ch_displayed].displayChannel(display);
      r += channels[current_ch_displayed].getNumOfFunctions();
      last_ch_displayed = current_ch_displayed;
    }  
    else {
      display.println("[...wait...]");
    }

    current_ch_displayed++;

    if(current_ch_displayed >= channels_number) {
      break;
    }
    if((r + channels[current_ch_displayed].getNumOfFunctions()) >= 5) {
      break;
    }
  }
  if(r == 3) {
    display.println(" ");
  }

  display.println("left back, right select");
  display.display();
  
}

/* draw detailed info about selected channel and menu to tell user what can be
   changed or configured */
void OptaUnoR4Display::draw_info_channel_page(uint8_t ch) {
  display.clearDisplay();

  display_expansion_type_as_title();

  display.setTextSize(1);  
  display.setCursor(0,16);

  display.print("* Selected ch: ");
  display.println(ch);
  
  if(ch < MAX_NUMBER_OF_CHANNELS) 
    channels[ch].displayChannelInfo(display);

  if(channels[ch].isPwm()) {
    display.println("Up: freq/Down: duty");
  }
  else if(channels[ch].isChangeable() && channels[ch].isConfigurable()) {
    display.println("Right:val/Down:cfg");
  }
  else if(channels[ch].isChangeable()) {
    display.println("Right: value");
  }
  else if(channels[ch].isConfigurable()) {
    display.println("Down: config");
  }

  display.println("Left: back");
  display.display();
}

/* draw a wait page when the user change a value, until the value is not actually
   sent to the controller */
void OptaUnoR4Display::draw_wait_change_value(uint8_t ch) {
  display.clearDisplay();

  display_expansion_type_as_title();

  display.setTextSize(1);  
  display.setCursor(0,16);

  display.print("* Selected ch: ");
  display.println(ch);

  display.println("I am sending new");
  display.println("value to controller");
  display.println();
  display.println("Please wait...");

  display.display();

}

/* __________________________________________________ DRAW: change value page */
void OptaUnoR4Display::draw_change_value_page(uint8_t ch) {
  display.clearDisplay();

  display_expansion_type_as_title();

  display.setTextSize(1);  
  display.setCursor(0,18);

  display.print("* Selected ch: ");
  display.println(ch);
  
  display.setTextSize(2);
  display.print("#: ");
  if(ch < MAX_NUMBER_OF_CHANNELS) 
    channels[ch].displayValue(selected_function,display);
  display.println();
  
  display.setTextSize(1);
  display.println("Up/Down: change value");
  display.println("Right:chg/Left:back");
  
  display.display();

}

/* _________________________________________________ DRAW: change config page */
void OptaUnoR4Display::draw_change_channel_config(uint8_t ch) {
  display.clearDisplay();

  display_expansion_type_as_title();

  display.setTextSize(1);  
  display.setCursor(0,18);

  display.print("* Selected ch: ");
  display.println(ch);

  if(sel_ch_cfg >= CH_CONFIG_NUM) {
    sel_ch_cfg = CH_CONFIG_NUM -1;
  }

  if(sel_ch_cfg < 0) {
    sel_ch_cfg = 0;
  }

  while(sel_ch_cfg > stop_ch_cfg) {
    start_ch_cfg++;
    stop_ch_cfg++;
  }

  while(sel_ch_cfg < start_ch_cfg) {
    start_ch_cfg--;
    stop_ch_cfg--;
  }

  bool ok = false;

  if(dexp != nullptr) {
    if(dexp->isConfigurable()) {
      if(channels[selected_channel].isConfigurable()) {
        for(int i = start_ch_cfg; i < stop_ch_cfg; i++ ) {
          display_cursor_selection(i, sel_ch_cfg);
          dexp->displayConfiguration(display, i, selected_channel);
        }
        display.println("Left:back/Right:sel");
        ok = true;
      }
    }
  }

  if(!ok) {
    display.println("PLEASE NOTE:");
    display.println("This channel config");
    display.println("cannot be changed");
    display.println("Left: back");
  }

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
  STATE_CHANGE_CHANNEL,
  STATE_CHANGE_CHANNEL_BUTTON_HANDLE,
  STATE_CHANGE_VALUE,
  STATE_CHANGE_VALUE_BUTTON_HANDLE,
  STATE_SET_CHANGE_VALUE,
  STATE_CHANGE_CONFIG,
  STATE_CHANGE_CONFIG_BUTTON_HANDLE,
  STATE_SET_CHANGE_CONFIG
} displayState_t;






/*_________________________________________________________MAIN STATE MACHINE */
/* handles different status of the display, showing different pages, depending
   upon the buttons pressed by the user */
void OptaUnoR4Display::main_state_machine() {
  static uint8_t current_expansion = 0;
  static displayState_t st = STATE_WELCOME_DRAW;
  static long long time = millis();

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
      
      selected_channel = 0;
      i2c_exp_selected_transmitted = UNOR4_DISPLAY_NO_SELECTION;
      exp_selected = UNOR4_DISPLAY_NO_SELECTION;
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
      /* a long press on the DOWN button trigger the change of status in 
         STATE_WAIT_EXPANSION_FEATURES */
      if(btn_pressed == EVENT_DOWN_LONG) {
        i2c_exp_selected_transmitted = current_expansion;
        st = STATE_WAIT_EXPANSION_FEATURES;
      }
      break;
  /* ________________________________________________________________________ */
  /* wait for expansion feature message sent by the controller */
    case STATE_WAIT_EXPANSION_FEATURES:
      draw_wait_for_expansion_features();
      /* as soon as the message arrives "it confirms" the expansion selected 
         so the feature are copied */
      if(i2c_exp_selected_received < UNOR4_DISPLAY_NO_SELECTION &&
         i2c_exp_selected_received == i2c_exp_selected_transmitted) {
        
        st = STATE_SHOW_EXPANSION;
        
        if(dexp != nullptr) {
          delete dexp;
        }
        dexp = r4display::factoryExpansion(i2c_exp_type);
        channels_number = i2c_exp_channel_num;
        exp_selected = i2c_exp_selected_received;
      }
      break;
  /* ________________________________________________________________________ */  
  /* Show the status of the expansion channel                                 */  
    case STATE_SHOW_EXPANSION:
      draw_expansion_page();
      time = millis();
      st = STATE_SHOW_EXPANSION_BUTTON_HANDLE;
    break;
  /* ________________________________________________________________________ */  
  /* handle the button pressed when channels status are displayed             */   
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
        st = STATE_CHANGE_CHANNEL;
      }
    break;
  /* ________________________________________________________________________ */  
  /* display status / change of single channel                                */   
    case STATE_CHANGE_CHANNEL:
      draw_info_channel_page(selected_channel);
      st = STATE_CHANGE_CHANNEL_BUTTON_HANDLE;
    break;
  /* ________________________________________________________________________ */
  /* handle the button pressed when channel status are displayed              */   
    case STATE_CHANGE_CHANNEL_BUTTON_HANDLE:
    if(btn_pressed == EVENT_LEFT_LONG) {
        st = STATE_SHOW_EXPANSION;
    }
    else if(channels[selected_channel].isPwm()) {
      if(btn_pressed == EVENT_UP_LONG) {
        selected_function = 0;
        old_channel_value_to_restore = channels[selected_channel].getValue(0);
        st = STATE_CHANGE_VALUE;
      }
      else if(btn_pressed == EVENT_DOWN_LONG) {
        selected_function = 1;
        old_channel_value_to_restore = channels[selected_channel].getValue(1);
        st = STATE_CHANGE_VALUE;
      }
    }
    else if(btn_pressed == EVENT_RIGHT_LONG) {
      selected_function = 0;
      old_channel_value_to_restore = channels[selected_channel].getValue(0);
      st = STATE_CHANGE_VALUE;
    }
    else if(btn_pressed == EVENT_DOWN_LONG) {
      st = STATE_CHANGE_CONFIG;
    }
    break;
  /* ________________________________________________________________________ */
  case STATE_CHANGE_VALUE:
    do_not_update_values_from_controller = true;
    draw_change_value_page(selected_channel);
    st = STATE_CHANGE_VALUE_BUTTON_HANDLE;
    break;
  /* ________________________________________________________________________ */
    case STATE_CHANGE_VALUE_BUTTON_HANDLE:
      if(btn_pressed == EVENT_DOWN) {
        channels[selected_channel].decrementValue(selected_function);
        st = STATE_CHANGE_VALUE;
      }
      else if(btn_pressed == EVENT_DOWN_LONG) {
        channels[selected_channel].decrementBigValue(selected_function);
        st = STATE_CHANGE_VALUE;
      }
      else if(btn_pressed == EVENT_UP) {
        channels[selected_channel].incrementValue(selected_function);
        st = STATE_CHANGE_VALUE;
      }
      else if(btn_pressed == EVENT_UP_LONG) {
        channels[selected_channel].incrementBigValue(selected_function);
        st = STATE_CHANGE_VALUE;
      }
      else if(btn_pressed == EVENT_LEFT_LONG) {
        channels[selected_channel].setValue(selected_function,old_channel_value_to_restore);
        st = STATE_SHOW_EXPANSION;
      }
      else if(btn_pressed == EVENT_RIGHT_LONG) {
        st = STATE_SET_CHANGE_VALUE;
      }
      break;
  /* ________________________________________________________________________ */
    case STATE_SET_CHANGE_VALUE:
      i2c_change_expansion_index = i2c_exp_selected_transmitted;
      if(dexp != nullptr) 
        i2c_change_expansion_type = dexp->getType(); 
      i2c_change_channel_index = selected_channel;
      i2c_change_channel_value = channels[selected_channel].getValue(selected_function);
      draw_wait_change_value(selected_channel);
      while(i2c_change_expansion_index != 255) {

      }
      st = STATE_SHOW_EXPANSION;
    break;
  /* ________________________________________________________________________ */
    case STATE_CHANGE_CONFIG:
      draw_change_channel_config(selected_channel);
      st = STATE_CHANGE_CONFIG_BUTTON_HANDLE;
    break;
  /* ________________________________________________________________________ */  
    case STATE_CHANGE_CONFIG_BUTTON_HANDLE:
      if(btn_pressed == EVENT_DOWN) {
        sel_ch_cfg++;
        st = STATE_CHANGE_CONFIG;
      }
      else if(btn_pressed == EVENT_UP) {
        sel_ch_cfg--;
        st = STATE_CHANGE_CONFIG;
      }
      else if(btn_pressed == EVENT_LEFT_LONG) {
        st = STATE_SHOW_EXPANSION;
      }
      else if(btn_pressed == EVENT_RIGHT_LONG) {
        st = STATE_SET_CHANGE_CONFIG;
      }
    break;
  /* ________________________________________________________________________ */  
    case STATE_SET_CHANGE_CONFIG:
      i2c_change_expansion_index = i2c_exp_selected_transmitted;
      if(dexp != nullptr) 
        i2c_change_expansion_type = dexp->getType(); 
      i2c_change_channel_index = selected_channel;
      i2c_change_channel_config = sel_ch_cfg;
      st = STATE_SHOW_EXPANSION;
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
