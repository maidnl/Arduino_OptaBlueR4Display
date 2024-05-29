/* -------------------------------------------------------------------------- */
/* FILE NAME:   CommonOptaUnoR4Display.h
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240417
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef COMMONOPTAUNOR4DISPLAY
#define COMMONOPTAUNOR4DISPLAY

#include <stdint.h>

/* CONFIGURATION FILE used to share information between FW and Expansion 
  "controller" class */



#define UNOR4_DISPLAY_DESCRIPTION "ARDUINO UNO R4 DISPLAY"

#define UNOR4_DISPLAY_NO_SELECTION 255

typedef enum {
  EVENT_NO_EVENT,
  EVENT_UP,
  EVENT_DOWN,
  EVENT_LEFT,
  EVENT_RIGHT,
  EVENT_UP_LONG,
  EVENT_DOWN_LONG,
  EVENT_LEFT_LONG,
  EVENT_RIGHT_LONG
} BtnEvent_t;

#define UNO_R4_DISPLAY_ADDITIONAL_TYPE   50

#define OPERATION_GET_BUTTON_STATUS 255

#define EXPANSION_NOT_SELECTED            255

#define MAX_CHANNEL_DISPLAYABLE           32

#define CH_UNIT_NO_UNIT  0
#define CH_UNIT_VOLT     1
#define CH_UNIT_mVOLT    2
#define CH_UNIT_AMPERE   3
#define CH_UNIT_mAMPERE  4
#define CH_UNIT_OHM      5
#define CH_UNIT_DIGIT    6
#define CH_UNIT_HERTZ    7
#define CH_UNIT_mHERTZ   8
#define CH_UNIT_KHERTZ   9
#define CH_UNIT_MHERTZ   10
#define CH_UNIT_PERC     11
#define CH_UNIT_NO_VALUE 12

typedef uint8_t chUnit_t;


#define CH_CONFIG_DAC_VOLTAGE 0
#define CH_CONFIG_DAC_CURRENT 1
#define CH_CONFIG_ADC_VOLTAGE 2
#define CH_CONFIG_ADC_CURRENT 3
#define CH_CONFIG_RTD_2_WIRES 4
#define CH_CONFIG_RTD_3_WIRES 5
#define CH_CONFIG_DIGTAL_INP 6
#define CH_CONFIG_RTD_HIGH_IM 7
#define CH_CONFIG_NUM         8

typedef uint8_t chCfg_t;

#define CH_FUNCTION_UNAVAILABLE    0
#define CH_FUNCTION_HIGH_IMPEDENCE 1
#define CH_FUNCTION_DIGITAL        2
#define CH_FUNCTION_ADC            3
#define CH_FUNCTION_DAC            4
#define CH_FUNCTION_PWM            5
#define CH_FUNCTION_RTD            6

typedef uint8_t chFun_t;

#define CH_TYPE_NO_TYPE            0
/* for digital */
#define CH_TYPE_INPUT              1
#define CH_TYPE_OUTPUT             2
#define CH_TYPE_LED                3
/* for DAC and ADC */
#define CH_TYPE_VOLTAGE            4
#define CH_TYPE_CURRENT            5
/* */
#define CH_TYPE_2_WIRES            6
#define CH_TYPE_3_WIRES            7

#define CH_TYPE_FREQ               8
#define CH_TYPE_DUTY               9
#define CH_TYPE_TIME               10

typedef uint8_t chType_t;



#endif
