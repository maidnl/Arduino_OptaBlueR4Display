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

#define UNOR4_DISPLAY_DESCRIPTION "ARDUINO UNO R4 DISPLAY"

#define HEADER_DIMENSION 3

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


#endif
