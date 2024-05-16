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

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* CONSTRUCTOR: always call the base Module constructor specifying
 * - TwoWire Object to be used
 * - DETECT IN pin
 * - DETECT OUT pin */

OptaUnoR4Display::OptaUnoR4Display()
    : Module(&Wire1, DETECT_IN, DETECT_OUT), 
    btn_pressed(EVENT_NO_EVENT),
    display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET), 
    exp_selected(EXPANSION_NOT_SELECTED),
    num_of_expansions(EXPANSION_NOT_SELECTED) {
  //
}

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* OVERRIDE virtual base methods: ALWAYS call BASE Module member function at
 * the beginning and then specialize the behaviour for your needs */

void OptaUnoR4Display::end() {
  /* Always call base methods */
  Module::end();
}

/* .......................................................................... */

void OptaUnoR4Display::begin() {
  /* ALWAYS call base method !*/
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


  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed... looping forever"));
    for(;;); // Don't proceed, loop forever
  }

  delay(2000); // Pause for 2 seconds        
  display.clearDisplay();

  draw_select_expansion_menu(0);
  delay(2000);
  draw_expansion_menu(0);
  
  
}

/* .......................................................................... */

void OptaUnoR4Display::update() {
  //
  /* Always call base methods */
  Module::update();
  BtnEvent_t ev = button_pressed();
  if (btn_pressed == EVENT_NO_EVENT) {
    
    btn_pressed = ev;
  }

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

/* ________________________________________________________________PARSE_RX() */
int OptaUnoR4Display::parse_rx() {
  /*
  for (int i = 0; i < 20; i++) {
    Serial.print(rx_buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  */

  /* ALWAYS call base method !*/
  int rv = Module::parse_rx();
  /* If base module returns >= 0 then the message has been handled in the base
   * function... simply returns the value*/
  if (rv != -1) {
    return rv;
  }

  rv = -1;

  /* PARSE SPECIFIC expansion messages */

  if (parse_get_selected_expansion()) {
    Serial.println("RX: get selected expansion");
    return msg_ans_selected_expansion();
  } else if(parse_set_expansion_features()) {
    Serial.println("RX: set Expansion features");
    return msg_ack();;
  } else if(parse_set_ch_configuration()) {
    Serial.println("RX: set Channel configuration");
    return msg_ack();;
  } else if(parse_set_num_of_expansion()) {
    Serial.println("RX: set num of expansion");
    return msg_ack();;
  } 

  return rv;
}

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* WRITE you own PURE VIRTUAL Base functions */

/* .......................................................................... */

uint8_t OptaUnoR4Display::getMajorFw() { return FW_VERSION_MAJOR; }
uint8_t OptaUnoR4Display::getMinorFw() { return FW_VERSION_MINOR; }
uint8_t OptaUnoR4Display::getReleaseFw() { return FW_VERSION_RELEASE; }

/* .......................................................................... */

std::string OptaUnoR4Display::getProduct() {
  std::string rv(UNOR4_DISPLAY_DESCRIPTION);
  return rv;
}

/* .......................................................................... */

void OptaUnoR4Display::goInBootloaderMode() { goBootloader(); }

/* .......................................................................... */

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

/* .......................................................................... */

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

/* .......................................................................... */

void OptaUnoR4Display::initStatusLED() {
  pinMode(UNOR4DISPLAY_LED_RED, OUTPUT);
  pinMode(UNOR4DISPLAY_LED_BLUE, OUTPUT);
  pinMode(UNOR4DISPLAY_LED_GREEN, OUTPUT);
  digitalWrite(UNOR4DISPLAY_LED_RED, LED_RGB_OFF);
  digitalWrite(UNOR4DISPLAY_LED_BLUE, LED_RGB_OFF);
  digitalWrite(UNOR4DISPLAY_LED_GREEN, LED_RGB_OFF);
}

/* .......................................................................... */

void OptaUnoR4Display::setStatusLedReadyForAddress() {
  digitalWrite(UNOR4DISPLAY_LED_BLUE, LED_RGB_OFF);
  digitalWrite(UNOR4DISPLAY_LED_RED, LED_RGB_ON);
  digitalWrite(UNOR4DISPLAY_LED_GREEN, LED_RGB_OFF);
}

/* .......................................................................... */

void OptaUnoR4Display::setStatusLedWaitingForAddress() {
  digitalWrite(UNOR4DISPLAY_LED_BLUE, LED_RGB_ON);
  digitalWrite(UNOR4DISPLAY_LED_RED, LED_RGB_OFF);
  digitalWrite(UNOR4DISPLAY_LED_GREEN, LED_RGB_OFF);
}

/* .......................................................................... */

void OptaUnoR4Display::setStatusLedHasAddress() {
  digitalWrite(UNOR4DISPLAY_LED_BLUE, LED_RGB_OFF);
  digitalWrite(UNOR4DISPLAY_LED_RED, LED_RGB_OFF);
  digitalWrite(UNOR4DISPLAY_LED_GREEN, LED_RGB_ON);
}

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* FROM HERE add your CUSTOM EXPANSION code */

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

void OptaUnoR4Display::testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.println("SDFASDF");
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x01);
  //delay(2000);
  //display.stopscroll();
  //delay(1000);
  //display.startscrollleft(0x00, 0x0F);
  //delay(2000);
  //display.stopscroll();
  //delay(1000);
  //display.startscrolldiagright(0x00, 0x07);
  //delay(2000);
  //display.startscrolldiagleft(0x00, 0x07);
  //delay(2000);
  //display.stopscroll();
  delay(1000);
}


void OptaUnoR4Display::draw_select_expansion_menu(uint8_t n){
  display.clearDisplay();

  /* Expansion */
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F(" Expansion"));
  
  /* to  left arrow */
  display.fillTriangle(
      28 , 25,
      28,  56,
      2, 25+14, SSD1306_INVERSE);
  
  /* number of expansion */
  display.setTextSize(5);
  display.setCursor(53,24);
  display.print(n);
  
  /* to right arrow */
  display.fillTriangle(
      display.width() - 28 , 25,
      display.width() - 28,  56,
      display.width() - 2, 25+14, SSD1306_INVERSE);
  
  display.display();
}

void OptaUnoR4Display::draw_expansion_menu(uint8_t n) {
  display.clearDisplay();

  /* Expansion */
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  
  display.print(" Analog ");
                //Digital
  display.println(n);
  

  display.setTextSize(1);  
  display.setCursor(0,16);
  for(int i = 0; i < 8; i++) {
    display.print("  ");
    display.print(i);
    display.print(") ");
    display.print("ADC");
    display.print(" ");
    display.println("value");

  } 
  display.display();

  display.startscrollleft(0x00, 0x01);
}


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

    exp_selected = rx_buffer[EXP_FEATURES_IndexPos];
    exp_type = rx_buffer[EXP_FEATURES_TypePos];
    exp_num_of_channels = rx_buffer[EXP_FEATURES_ChNumPos];

    for(int i = 0; i < MAX_CHANNEL_DISPLAYABLE; i++) {
      ch_cfg[i].type = CH_TYPE_NO_TYPE;
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
      ch_cfg[ch].units[1] = rx_buffer[CH_CFG_U4Pos];
    }
    return true;
  }

  return false;
}

#endif
