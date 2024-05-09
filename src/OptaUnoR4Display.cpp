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
    : Module(&Wire1, DETECT_IN, DETECT_OUT), btn_pressed(EVENT_NO_EVENT) {
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

/* .......................................................................... */

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

  if (checkGetMsgReceived(rx_buffer, ARG_R4DISPLAY_GET_BTN,
                          LEN_R4DISPLAY_GET_BTN, R4DISPLAY_GET_BTN_LEN)) {
    return msg_ans_get_btn_status();
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

int OptaUnoR4Display::msg_ans_get_btn_status() {

  tx_buffer[ANS_BUTTON_STATUS_POS] = btn_pressed;
  btn_pressed = EVENT_NO_EVENT;
  return prepareGetAns(tx_buffer, ANS_ARG_R4DISPLAY_GET_BTN,
                       ANS_LEN_R4DISPLAY_GET_BTN, ANS_R4DISPLAY_GET_BTN_LEN);
}

#endif
