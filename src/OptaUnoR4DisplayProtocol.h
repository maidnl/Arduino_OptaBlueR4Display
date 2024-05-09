/* -------------------------------------------------------------------------- */
/* FILE NAME:   OptaUnoR4DisplayProtocol.h
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240509
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef OPTAUNOR4DISPLAYPROTOCOL
#define OPTAUNOR4DISPLAYPROTOCOL

#define ARG_R4DISPLAY_GET_BTN (uint8_t)0xFA
#define LEN_R4DISPLAY_GET_BTN (uint8_t)0x00
#define R4DISPLAY_GET_BTN_LEN                                                  \
  (uint8_t)(HEADER_DIMENSION + LEN_R4DISPLAY_GET_BTN)

/* ANSWER from expansion: get DI value */
#define ANS_ARG_R4DISPLAY_GET_BTN (uint8_t) ARG_R4DISPLAY_GET_BTN
#define ANS_LEN_R4DISPLAY_GET_BTN (uint8_t)0x01
#define ANS_BUTTON_STATUS_POS (uint8_t)0x03
#define ANS_R4DISPLAY_GET_BTN_LEN                                              \
  (uint8_t)(HEADER_DIMENSION + ANS_LEN_R4DISPLAY_GET_BTN)

#endif
