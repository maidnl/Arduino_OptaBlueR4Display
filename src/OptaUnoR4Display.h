/* -------------------------------------------------------------------------- */
/* FILE NAME:   OptaUnoR4Display.h
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

#ifndef OPTAUNOR4DISPLAY
#define OPTAUNOR4DISPLAY

#ifdef ARDUINO_UNOR4_WIFI
#include "Arduino.h"
#include "EEPROM.h"
#include "OptaBlueModule.h"
#include "OptaUnoR4DisplayCfg.h"
#include "UnoR4DisplayCommon.h"
#include "boot.h"
#include <stdint.h>

typedef enum { BTN_IDLE, BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT } BtnStatus_t;

class OptaUnoR4Display : public Module {
public:
  OptaUnoR4Display();
  virtual void end() override;
  virtual void begin() override;
  virtual void update() override;
  virtual int parse_rx() override;

  uint8_t getMajorFw();
  uint8_t getMinorFw();
  uint8_t getReleaseFw();
  std::string getProduct();
  void goInBootloaderMode();
  void readFromFlash(uint16_t add, uint8_t *buffer, uint8_t dim);
  void writeInFlash(uint16_t add, uint8_t *buffer, uint8_t dim);
  void initStatusLED();
  void setStatusLedReadyForAddress();
  void setStatusLedWaitingForAddress();
  void setStatusLedHasAddress();

protected:
  BtnEvent_t btn_pressed;
  int msg_ans_get_btn_status();

  BtnEvent_t fire_button_event(BtnStatus_t st, int counter, bool &fired,
                               bool &long_fired);
  BtnEvent_t button_pressed();
};
#endif
#endif
