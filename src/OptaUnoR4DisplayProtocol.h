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

/* FILE CONTAINING THE I2C messages */

#include "OptaBlueProtocol.h"

/* ___________________________________________REQUEST: GET SELECTED EXPANSION */
#define Req_GET_EXP      0x01
#define Len_GET_EXP      0x00

  
/* ____________________________________________ANSWER: GET SELECTED EXPANSION */
#define Ans_GET_EXP         Req_GET_EXP
#define AnsLen_GET_EXP      0x01
#define Ans_GET_EXP_ExpPos  BP_HEADER_DIM


/* _______________ACK: GENERIC ACK used for command that do not use an answer */
#define ACK_ARG          0xFA
#define Len_ACK          0x00



/* ______________________________________COMMAND: SELECTED EXPANSION FEATURES */
#define Cmd_EXP_FEATURES         0x04
#define Len_EXP_FEATURES         0x03
#define EXP_FEATURES_TypePos     (BP_HEADER_DIM + 0)
#define EXP_FEATURES_IndexPos    (BP_HEADER_DIM + 1)
#define EXP_FEATURES_ChNumPos    (BP_HEADER_DIM + 2)


/* ____________________________________________COMMAND: CHANNEL CONFIGURATION */
#define Cmd_CH_CFG               0x02
#define Len_CH_CFG               15
#define CH_CFG_ChPos             (BP_HEADER_DIM + 0)
#define CH_CFG_Func1Pos          (BP_HEADER_DIM + 1)
#define CH_CFG_Type1Pos          (BP_HEADER_DIM + 2)
#define CH_CFG_V1Pos_0           (BP_HEADER_DIM + 3) 
#define CH_CFG_V1Pos_1           (BP_HEADER_DIM + 4) 
#define CH_CFG_V1Pos_2           (BP_HEADER_DIM + 5) 
#define CH_CFG_V1Pos_3           (BP_HEADER_DIM + 6) 
#define CH_CFG_U1Pos             (BP_HEADER_DIM + 7)
#define CH_CFG_Func2Pos          (BP_HEADER_DIM + 8) 
#define CH_CFG_Type2Pos          (BP_HEADER_DIM + 9)
#define CH_CFG_V2Pos_0           (BP_HEADER_DIM + 10) 
#define CH_CFG_V2Pos_1           (BP_HEADER_DIM + 11) 
#define CH_CFG_V2Pos_2           (BP_HEADER_DIM + 12) 
#define CH_CFG_V2Pos_3           (BP_HEADER_DIM + 13) 
#define CH_CFG_U2Pos             (BP_HEADER_DIM + 14) 


/* ___________________________________COMMAND: NUMBER OF EXPANSION DISCOVERED */
#define Cmd_EXP_NUM               0x03
#define Len_EXP_NUM               1
#define EXP_NUM_NumPos            (BP_HEADER_DIM + 0)


/*__________________________________________COMMAND: GET CHANNEL CHANGE VALUE */
#define Cmd_GET_CH_VALUE          0x05
#define Len_GET_CH_VALUE          0


/* __________________________________________ANSWER: GET CHANNEL CHANGE VALUE */
#define Ans_GET_CH_VALUE                Cmd_GET_CH_VALUE                                   
#define AnsLen_GET_CH_VALUE             7
#define Ans_GET_CH_VALUE_IndexPos       (BP_HEADER_DIM + 0)
#define Ans_GET_CH_VALUE_ExpTypePos     (BP_HEADER_DIM + 1)
#define Ans_GET_CH_VALUE_ChannelPos     (BP_HEADER_DIM + 2)
#define Ans_GET_CH_VALUE_ValuePos       (BP_HEADER_DIM + 3)


/*_________________________________________COMMAND: GET CHANNEL CHANGE CONFIG */
#define Cmd_GET_CH_CONFIG          0x06
#define Len_GET_CH_CONFIG          0


/* __________________________________________ANSWER: GET CHANNEL CHANGE VALUE */
#define Ans_GET_CH_CONFIG                Cmd_GET_CH_CONFIG                                   
#define AnsLen_GET_CH_CONFIG             4
#define Ans_GET_CH_CONFIG_IndexPos       (BP_HEADER_DIM + 0)
#define Ans_GET_CH_CONFIG_ExpTypePos     (BP_HEADER_DIM + 1)
#define Ans_GET_CH_CONFIG_ChannelPos     (BP_HEADER_DIM + 2)
#define Ans_GET_CH_CONFIG_ConfigPos      (BP_HEADER_DIM + 3)


#endif
