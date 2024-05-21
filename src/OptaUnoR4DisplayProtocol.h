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
#define GET_EXP_Len      (BP_HEADER_DIM + Len_GET_EXP)
  
/* ____________________________________________ANSWER: GET SELECTED EXPANSION */
#define Ans_GET_EXP         Req_GET_EXP
#define AnsLen_GET_EXP      0x01
#define Ans_GET_EXP_ExpPos  BP_HEADER_DIM
#define Ans_GET_EXP_Len     (BP_HEADER_DIM + AnsLen_GET_EXP)

#define GET_EXP_AnsLen      Ans_GET_EXP_Len

/* _______________ACK: GENERIC ACK used for command that do not use an answer */
#define ACK_ARG          0xFA
#define Len_ACK          0x00
#define ACK_Len          (BP_HEADER_DIM + Len_ACK)


/* ______________________________________COMMAND: SELECTED EXPANSION FEATURES */
#define Cmd_EXP_FEATURES         0x04
#define Len_EXP_FEATURES         0x03
#define EXP_FEATURES_TypePos     (BP_HEADER_DIM + 0)
#define EXP_FEATURES_IndexPos    (BP_HEADER_DIM + 1)
#define EXP_FEATURES_ChNumPos    (BP_HEADER_DIM + 2)
#define EXP_FEATURES_Len         (BP_HEADER_DIM + Len_EXP_FEATURES)

#define EXP_FEATURES_AnsLen      ACK_Len

/* ____________________________________________COMMAND: CHANNEL CONFIGURATION */
#define Cmd_CH_CFG               0x02
#define Len_CH_CFG               12
#define CH_CFG_ChPos             (BP_HEADER_DIM + 0)
#define CH_CFG_TypePos           (BP_HEADER_DIM + 1)
#define CH_CFG_V1Pos_0           (BP_HEADER_DIM + 2) 
#define CH_CFG_V1Pos_1           (BP_HEADER_DIM + 3) 
#define CH_CFG_V1Pos_2           (BP_HEADER_DIM + 4) 
#define CH_CFG_V1Pos_3           (BP_HEADER_DIM + 5) 
#define CH_CFG_U1Pos             (BP_HEADER_DIM + 6) 
#define CH_CFG_V2Pos_0           (BP_HEADER_DIM + 7) 
#define CH_CFG_V2Pos_1           (BP_HEADER_DIM + 8) 
#define CH_CFG_V2Pos_2           (BP_HEADER_DIM + 9) 
#define CH_CFG_V2Pos_3           (BP_HEADER_DIM + 10) 
#define CH_CFG_U2Pos             (BP_HEADER_DIM + 11) 
#define CH_CFG_Len               (BP_HEADER_DIM + Len_CH_CFG)

#define CH_CFG_AnsLen            ACK_Len

/* ___________________________________COMMAND: NUMBER OF EXPANSION DISCOVERED */
#define Cmd_EXP_NUM               0x03
#define Len_EXP_NUM               1
#define EXP_NUM_NumPos            (BP_HEADER_DIM + 0)
#define EXP_NUM_Len               (BP_HEADER_DIM + Len_EXP_NUM)

#define EXP_NUM_AnsLen            ACK_Len

/*__________________________________________COMMAND: GET CHANNEL CHANGE VALUE */
#define Cmd_GET_CH_VALUE          0x05
#define Len_GET_CH_VALUE          0
#define GET_CH_VALUE_Len          (BP_HEADER_DIM + Len_GET_CH_VALUE)

/* __________________________________________ANSWER: GET CHANNEL CHANGE VALUE */
#define Ans_GET_CH_VALUE                Cmd_GET_CH_VALUE                                   
#define AnsLen_GET_CH_VALUE             7
#define Ans_GET_CH_VALUE_IndexPos       (BP_HEADER_DIM + 0)
#define Ans_GET_CH_VALUE_ExpTypePos     (BP_HEADER_DIM + 1)
#define Ans_GET_CH_VALUE_ChannelPos     (BP_HEADER_DIM + 2)
#define Ans_GET_CH_VALUE_ValuePos       (BP_HEADER_DIM + 3)
#define Ans_GET_CH_VALUE_Len            (BP_HEADER_DIM + AnsLen_GET_CH_VALUE)

#define GET_CH_VALUE_AnsLen             Ans_GET_CH_VALUE_Len

/*_________________________________________COMMAND: GET CHANNEL CHANGE CONFIG */
#define Cmd_GET_CH_CONFIG          0x06
#define Len_GET_CH_CONFIG          0
#define GET_CH_CONFIG_Len          (BP_HEADER_DIM + Len_GET_CH_VALUE)

/* __________________________________________ANSWER: GET CHANNEL CHANGE VALUE */
#define Ans_GET_CH_CONFIG                Cmd_GET_CH_CONFIG                                   
#define AnsLen_GET_CH_CONFIG             4
#define Ans_GET_CH_CONFIG_IndexPos       (BP_HEADER_DIM + 0)
#define Ans_GET_CH_CONFIG_ExpTypePos     (BP_HEADER_DIM + 1)
#define Ans_GET_CH_CONFIG_ChannelPos     (BP_HEADER_DIM + 2)
#define Ans_GET_CH_CONFIG_ConfigPos      (BP_HEADER_DIM + 3)
#define Ans_GET_CH_CONFIG_Len            (BP_HEADER_DIM + AnsLen_GET_CH_CONFIG)

#define GET_CH_CONFIG_AnsLen             Ans_GET_CH_CONFIG_Len

#endif
