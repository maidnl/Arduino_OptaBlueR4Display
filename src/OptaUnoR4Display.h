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


#include "UnoR4DisplayOtherClasses.h"

using namespace std;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define SPECIAL_CH_VALUE_NO_SPECIAL 0
#define SPECIAL_CH_VALUE_PWM_FREQ 0
#define SPECIAL_CH_VALUE_PWM_DUTY 1


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
  ~OptaUnoR4Display();
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
  
  volatile uint8_t channels_number = 0;
  r4display::Channel channels[MAX_NUMBER_OF_CHANNELS];
  r4display::DisplayExpansion *dexp = nullptr;
  float old_channel_value_to_restore = 0.0;
  uint8_t selected_function = 0;
  volatile bool do_not_update_values_from_controller = false;


  r4display::ChFunction *function_to_changed = nullptr;
  
  /* i2c_exp_selected_transmitted is sent to controller to inform it that the 
     user has selected an expansion (255 means that the user do not select 
     any expansion */
  volatile uint8_t i2c_exp_selected_transmitted = UNOR4_DISPLAY_NO_SELECTION;
  /* i2c_exp_selected_received is transmitted from the controller to acknowledge
     the fact that it has understood the user selection */
  volatile uint8_t i2c_exp_selected_received = UNOR4_DISPLAY_NO_SELECTION;
  volatile uint8_t i2c_num_of_exp = UNOR4_DISPLAY_NO_SELECTION;
  volatile uint8_t i2c_exp_type = UNOR4_DISPLAY_NO_SELECTION;
  volatile uint8_t i2c_exp_channel_num = 0;
  
 
  uint8_t exp_selected = UNOR4_DISPLAY_NO_SELECTION;
  
  volatile uint8_t i2c_change_expansion_index = UNOR4_DISPLAY_NO_SELECTION;
  volatile uint8_t i2c_change_expansion_type = UNOR4_DISPLAY_NO_SELECTION;
  volatile uint8_t i2c_change_channel_index = UNOR4_DISPLAY_NO_SELECTION;
  volatile float i2c_change_channel_value = 0.0;
  volatile uint8_t i2c_change_channel_config = 0;

  volatile uint8_t i2c_change_expansion_index_config = UNOR4_DISPLAY_NO_SELECTION;
  volatile uint8_t i2c_change_expansion_type_config = UNOR4_DISPLAY_NO_SELECTION;
  volatile uint8_t i2c_change_channel_index_config = UNOR4_DISPLAY_NO_SELECTION;

  volatile int8_t ch_info_received = -1;

  
  int8_t sel_ch_cfg = 0;
  uint8_t start_ch_cfg = 0;
  uint8_t stop_ch_cfg = 4;
  
  volatile bool reset_state_machine;
  uint8_t selected_channel = 0;

/* ---------------------- functions member ---------------------------------- */
  
  /* BUTTONs: debounce and fire button event */
  BtnEvent_t fire_button_event(BtnStatus_t st, int counter, bool &fired,
                               bool &long_fired);
  /* BUTTONs: read the buttons phisical status */
  BtnEvent_t button_pressed();

/* _____ I2C related functions_______*/
  
  /* tell the controller which is the selected expansion */
  bool parse_get_selected_expansion();
  uint8_t msg_ans_selected_expansion();
  /* tell the controller the new value of the channel to be changed */
  bool parse_get_ch_change_value();
  uint8_t msg_get_ch_change_value();
  /* tell the controller the new configuration of the channel to be changed */
  bool parse_get_ch_change_config();
  uint8_t msg_get_ch_change_config();
  /* get from the controller info about expansion selected */
  bool parse_set_expansion_features();
  /* get from the controller how many expansion are present */
  bool parse_set_num_of_expansion();
  /* get from expansion the configuration for each channel */
  bool parse_set_ch_configuration();
  /* generic acknowledge message for messages without a proper answer */
  uint8_t msg_ack();
  
/* _____ DRAWING functions_______*/  
  
  /* draw_ functions -> draw an entire page on the display */
  void draw_welcome_page();
  void draw_wait_for_expansion_page();
  void draw_wait_for_expansion_features();
  void draw_select_expansion_menu(uint8_t n);
  void draw_expansion_page();
  void draw_info_channel_page(uint8_t ch);
  void draw_change_value_page(uint8_t ch);
  void draw_change_channel_config(uint8_t ch);
  void draw_wait_change_value(uint8_t ch);

  /* display_ function -> draw a certain portion of a page */
  void display_expansion_type_as_title();
  void display_cursor_selection(uint8_t r, uint8_t s);
 
/* _______MAIN STATE MACHINE FUNCTION: the actual brain of this class ________*/
  
  void main_state_machine();
  
/* _____ DEBUGGING functions_______*/   
  #ifdef DEBUG_I2C_MESSAGES
  volatile uint8_t debug_msg = 0;
  void debugReceivedMsg();
  #endif
};
#endif
#endif
