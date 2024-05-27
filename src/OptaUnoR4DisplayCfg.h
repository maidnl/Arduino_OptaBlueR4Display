/* -------------------------------------------------------------------------- */
/* FILE NAME:   OptaUnoR4DisplayCfg.h
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

#ifndef OPTAUNOR4DISPLAYCFG
#define OPTAUNOR4DISPLAYCFG

#define FW_VERSION_MAJOR 0
#define FW_VERSION_MINOR 0
#define FW_VERSION_RELEASE 1

#define DETECT_IN 2
#define DETECT_OUT 3

#define UNOR4DISPLAY_LED_RED A3
#define UNOR4DISPLAY_LED_GREEN A1
#define UNOR4DISPLAY_LED_BLUE A2

#define LED_RGB_ON HIGH
#define LED_RGB_OFF LOW

#define UNOR4_DIPLAY_FOUR_BUTTON_AIN A0

#define EVENT_COUNTER_NUM 50
#define LONG_EVENT_COUNTER_NUM 600

#define MAX_ROW_DISPLAYED_PER_PAGE 5

#define WELCOME_PAGE_TIME 1000
#define REFRESH_STATUS 1000

#define MAX_NUMBER_OF_CHANNELS 32

//#define DEBUG_BUTTONS
//#define DEBUG_I2C_MESSAGES

#endif
