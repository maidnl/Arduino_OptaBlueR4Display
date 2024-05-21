/* -------------------------------------------------------------------------- */
/* FILE NAME:   R4DisplayAddress.h
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

#ifndef R4DISPLAYREGISTER
#define R4DISPLAYREGISTER

#define  R4DISPLAY_EXPANSION_ADDRESS 2000

#define ADD_SELECTED_EXPANSION        (R4DISPLAY_EXPANSION_ADDRESS + 0)
#define ADD_NUM_OF_EXPANSIONS         (R4DISPLAY_EXPANSION_ADDRESS + 1)
#define ADD_EXPANSION_TYPE            (R4DISPLAY_EXPANSION_ADDRESS + 2)
#define ADD_EXPANSION_NUM_OF_CHANNELS (R4DISPLAY_EXPANSION_ADDRESS + 3)

#define ADD_SELECTED_CHANNEL      (R4DISPLAY_EXPANSION_ADDRESS + 4)
#define ADD_SELECTED_CHANNEL_TYPE (R4DISPLAY_EXPANSION_ADDRESS + 5)

#define ADD_CHANNEL_V1 (R4DISPLAY_EXPANSION_ADDRESS + 6)
#define ADD_CHANNEL_U1 (R4DISPLAY_EXPANSION_ADDRESS + 7)
#define ADD_CHANNEL_V2 (R4DISPLAY_EXPANSION_ADDRESS + 8)
#define ADD_CHANNEL_U2 (R4DISPLAY_EXPANSION_ADDRESS + 9)

#define ADD_CH_VALUE_EXPANSION
#define ADD_CH_VALUE_EXPANSION_INDEX

#endif
