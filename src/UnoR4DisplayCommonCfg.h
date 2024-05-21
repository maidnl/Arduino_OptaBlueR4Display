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

/* CONFIGURATION FILE used to share information between FW and Expansion 
  "controller" class */

#define UNOR4_DISPLAY_DESCRIPTION "ARDUINO UNO R4 DISPLAY"

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

#define OPERATION_GET_BUTTON_STATUS 255

#define EXPANSION_NOT_SELECTED            255

#define MAX_CHANNEL_DISPLAYABLE           32

#define CH_TYPE_HIGH_IMPEDENCE             0
#define CH_TYPE_HIGH_IMPEDENCE_WITH_V_ADC  1
#define CH_TYPE_DIGITAL_IN                 2
#define CH_TYPE_DIGITAL_IN_WITH_V_ADC      3
#define CH_TYPE_DIGITAL_IN_WITH_C_ADC      4
#define CH_TYPE_DIGITAL_OUT                5
#define CH_TYPE_DIGITAL_OUT_WITH_V_ADC     6
#define CH_TYPE_DIGITAL_OUT_WITH_C_ADC     7
#define CH_TYPE_V_ADC                      8
#define CH_TYPE_C_ADC                      9
#define CH_TYPE_V_ADC_WITH_C_ADC           10
#define CH_TYPE_C_ADC_WITH_V_ADC           11
#define CH_TYPE_PWM                        12
#define CH_TYPE_RTD_2_WIRES                13
#define CH_TYPE_RTD_3_WIRES                14
#define CH_TYPE_V_DAC                      15
#define CH_TYPE_C_DAC                      16
#define CH_TYPE_V_DAC_WITH_C_ADC           17
#define CH_TYPE_C_DAC_WITH_V_ADC           18
#define CH_TYPE_LED                        19

#define CH_TYPE_NO_TYPE (255)

typedef uint8_t chType_t;

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


#define UNO_R4_DISPLAY_ADDITIONAL_TYPE   255


class ChCfg {
public:
  chType_t type;
  float values[2];
  
  chUnit_t units[2];
  ChCfg(): type(CH_TYPE_NO_TYPE) {
    values[0] = 0.0;
    values[1] = 0.0;
    units[0] = CH_UNIT_NO_UNIT;
    units[1] = CH_UNIT_NO_UNIT;
  }

};

#endif
