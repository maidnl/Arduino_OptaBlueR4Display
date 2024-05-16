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
#include "OptaUnoR4DisplayProtocol.h"
#include "UnoR4DisplayCommonCfg.h"
#include "OptaBluePrintCfg.h"
#include "boot.h"
#include <stdint.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <vector>

using namespace std;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

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
  Adafruit_SSD1306 display;

  uint8_t exp_selected;
  uint8_t num_of_expansions;
  uint8_t exp_type;
  uint8_t exp_num_of_channels;

  ChCfg ch_cfg[MAX_CHANNEL_DISPLAYABLE];

  bool parse_get_selected_expansion();
  uint8_t msg_ans_selected_expansion();

  uint8_t msg_ack();
  bool parse_set_expansion_features();
  bool parse_set_ch_configuration();
  bool parse_set_num_of_expansion();

  void draw_select_expansion_menu(uint8_t n);
  void draw_expansion_menu(uint8_t n);
  void testscrolltext();



  BtnEvent_t fire_button_event(BtnStatus_t st, int counter, bool &fired,
                               bool &long_fired);
  BtnEvent_t button_pressed();
};
#endif
#endif
