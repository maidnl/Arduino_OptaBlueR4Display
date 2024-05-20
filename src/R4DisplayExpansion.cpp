/* -------------------------------------------------------------------------- */
/* FILE NAME:   R4DisplayExpansion.cpp
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

#include "R4DisplayExpansion.h"
#include "R4DisplayAddress.h"

/* clause to compile only for OPTA CONTROLLER */
#ifdef ARDUINO_OPTA

namespace Opta {
/* Constructor */
R4DisplayExpansion::R4DisplayExpansion() {}
/* Destructor */
R4DisplayExpansion::~R4DisplayExpansion() {}

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* WRITE mandatory functions */

/* ________________________________makeExpansion MANDATORY! (and always used) */
Expansion *R4DisplayExpansion::makeExpansion() {
  return new R4DisplayExpansion();
}

/* ___________________________________getProduct MANDATORY! (and always used) */
std::string R4DisplayExpansion::getProduct() {
  std::string rv(UNOR4_DISPLAY_DESCRIPTION);
  return rv;
}

/* __________________________startUp MANDATORY (but not used in this example) */
void R4DisplayExpansion::startUp(Controller *ptr) { (void)ptr; }

/* _______________________________________________copy constructor MANDATORY! */
R4DisplayExpansion::R4DisplayExpansion(Expansion &other) {
  R4DisplayExpansion &de = (R4DisplayExpansion &)other;

  type = EXPANSION_NOT_VALID;
  i2c_address = 0;
  ctrl = other.getCtrl();
  index = 255;

  if (ctrl != nullptr) {
    if (other.getType() ==
        ctrl->getExpansionType(R4DisplayExpansion::getProduct())) {
      iregs = de.iregs;
      fregs = de.fregs;
      type = other.getType();
      i2c_address = other.getI2CAddress();
      ctrl = other.getCtrl();
      index = other.getIndex();
    }
  }
}

/* ___________________________________________PREPARE: get selected expansion */
uint8_t R4DisplayExpansion::msg_get_selected_expansion() {
  if (ctrl == nullptr) return 0;
  
  uint8_t rv =  prepareGetMsg(ctrl->getTxBuffer(),
                       Req_GET_EXP,
                       Len_GET_EXP, 
                       GET_EXP_Len);
  /*
  Serial.print("get selected expansion: ");
  for(int i = 0; i < getExpectedAnsLen(GET_EXP_Len); i++) {  
    Serial.print(ctrl->getTxBuffer()[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  */
  return rv;

}

/* _________________________________________PARSE: ans get selected expansion */
bool R4DisplayExpansion::parse_ans_get_selected_expansion() {
  if (ctrl == nullptr) return false;
  /*
  Serial.print("PARSE get selected expansion: ");
  for(int i = 0; i < getExpectedAnsLen(Ans_GET_EXP_Len); i++) {  
    Serial.print(ctrl->getRxBuffer()[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  */
  if(checkAnsGetReceived(ctrl->getRxBuffer(), 
                         Ans_GET_EXP,
                         AnsLen_GET_EXP, 
                         Ans_GET_EXP_Len)) {
    iregs[ADD_SELECTED_EXPANSION] = ctrl->getRxBuffer()[Ans_GET_EXP_ExpPos];
    return true;
  }
  return false;

}

/* _________________________________________________________________PARSE ACK */
bool R4DisplayExpansion::parse_ack() {
  if (ctrl == nullptr) return false;

  return checkAnsSetReceived(ctrl->getRxBuffer(), 
                             ACK_ARG,
                             Len_ACK, 
                             ACK_Len);

}

/* ___________________________________PREPARE: NUMBER OF EXPANSION DISCOVERED */
uint8_t R4DisplayExpansion::msg_set_num_of_expansions() {
  if (ctrl == nullptr) return 0;

  ctrl->getTxBuffer()[EXP_NUM_NumPos] = iregs[ADD_NUM_OF_EXPANSIONS];
  uint8_t rv = prepareSetMsg(ctrl->getTxBuffer(), 
                       Cmd_EXP_NUM,
                       Len_EXP_NUM, 
                       EXP_NUM_Len);
  /*
  Serial.print("set num of expansion: ");
  for(int i = 0; i < getExpectedAnsLen(EXP_NUM_Len); i++) {  
    Serial.print(ctrl->getTxBuffer()[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  */
  return rv;
}

/* ______________________________________PREPARE: SELECTED EXPANSION FEATURES */
uint8_t R4DisplayExpansion::msg_set_expansion_features() {
  if (ctrl == nullptr) return 0;

  ctrl->getTxBuffer()[EXP_FEATURES_TypePos] = iregs[ADD_EXPANSION_TYPE];
  ctrl->getTxBuffer()[EXP_FEATURES_IndexPos] = iregs[ADD_SELECTED_EXPANSION];
  ctrl->getTxBuffer()[EXP_FEATURES_ChNumPos] = iregs[ADD_EXPANSION_NUM_OF_CHANNELS];
  uint8_t rv = prepareSetMsg(ctrl->getTxBuffer(), 
                       Cmd_EXP_FEATURES,
                       Len_EXP_FEATURES, 
                       EXP_FEATURES_Len);
  /*
  Serial.print("set expansion features: ");
  for(int i = 0; i < getExpectedAnsLen(EXP_FEATURES_Len); i++) {  
    Serial.print(ctrl->getTxBuffer()[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  */
  return rv;

}

/* ____________________________________________PREPARE: CHANNEL CONFIGURATION */
uint8_t R4DisplayExpansion::msg_set_channel_configuration() {
  if (ctrl == nullptr) return 0;

  Float_u v;

  ctrl->getTxBuffer()[CH_CFG_ChPos] = iregs[ADD_SELECTED_CHANNEL];
  v.value = fregs[ADD_CHANNEL_V1];
  ctrl->getTxBuffer()[CH_CFG_TypePos] = iregs[ADD_SELECTED_CHANNEL_TYPE];
  ctrl->getTxBuffer()[CH_CFG_V1Pos_0] = v.bytes[0];
  ctrl->getTxBuffer()[CH_CFG_V1Pos_1] = v.bytes[1];
  ctrl->getTxBuffer()[CH_CFG_V1Pos_2] = v.bytes[2];
  ctrl->getTxBuffer()[CH_CFG_V1Pos_3] = v.bytes[3];
  ctrl->getTxBuffer()[CH_CFG_U1Pos] = iregs[ADD_CHANNEL_U1];
  v.value = fregs[ADD_CHANNEL_V2];
  ctrl->getTxBuffer()[CH_CFG_V2Pos_0] = v.bytes[0];
  ctrl->getTxBuffer()[CH_CFG_V2Pos_1] = v.bytes[1];
  ctrl->getTxBuffer()[CH_CFG_V2Pos_2] = v.bytes[2];
  ctrl->getTxBuffer()[CH_CFG_V2Pos_3] = v.bytes[3];
  ctrl->getTxBuffer()[CH_CFG_U2Pos] = iregs[ADD_CHANNEL_U2];
  uint8_t rv = prepareSetMsg(ctrl->getTxBuffer(), 
                       Cmd_CH_CFG,
                       Len_CH_CFG, 
                       CH_CFG_Len);
  /*
  Serial.print("set channel cfg: ");
  for(int i = 0; i < getExpectedAnsLen(CH_CFG_Len); i++) {  
    Serial.print(ctrl->getTxBuffer()[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  */
}



/* __________________________________________________________________ EXECUTE */
unsigned int R4DisplayExpansion::execute(uint32_t what) {
  unsigned int rv = EXECUTE_OK;
  if (ctrl != nullptr) {
    
    switch (what) {
    /* ------------------------------------------------------------------- */
    case EXECUTE_GET_SELECTED_EXPANSION:
      
      rv = i2c_transaction(&R4DisplayExpansion::msg_get_selected_expansion,
                           &R4DisplayExpansion::parse_ans_get_selected_expansion,
                           getExpectedAnsLen(GET_EXP_AnsLen));
      break;
    /* ------------------------------------------------------------------- */
    case EXECUTE_SET_NUM_OF_EXPANSION:
      
      rv = i2c_transaction(&R4DisplayExpansion::msg_set_num_of_expansions,
                           &R4DisplayExpansion::parse_ack,
                           getExpectedAnsLen(ACK_Len));
      break;
    /* ------------------------------------------------------------------- */
    case EXECUTE_SET_EXPANSION_FEATURES:
      
      rv = i2c_transaction(&R4DisplayExpansion::msg_set_expansion_features,
                           &R4DisplayExpansion::parse_ack,
                           getExpectedAnsLen(ACK_Len));
      break;
    /* ------------------------------------------------------------------- */
    case EXECUTE_SET_CHANNEL_CONFIGURATION:
     
      rv = i2c_transaction(&R4DisplayExpansion::msg_set_channel_configuration,
                           &R4DisplayExpansion::parse_ack,
                           getExpectedAnsLen(ACK_Len));
      break;
    /* ------------------------------------------------------------------- */  
    default:
      rv = Expansion::execute(what);
      break;
    }
    /* IMPORTANT: call this if you want to keep your expansion copies in sync */
    ctrl->updateRegs(*this);
  } else {
    rv = EXECUTE_ERR_NO_CONTROLLER;
  }
  return rv;
}

/* __________________________________________________________ I2C TRANSACTION */
unsigned int
R4DisplayExpansion::i2c_transaction(uint8_t (R4DisplayExpansion::*prepare)(),
                                    bool (R4DisplayExpansion::*parse)(),
                                    int rx_bytes) {
  


  if (prepare != nullptr) {
    uint8_t err =
        ctrl->send(i2c_address, index, type, (this->*prepare)(), rx_bytes);
    if (err == SEND_RESULT_OK) {
      if (parse != nullptr) {
        if (!(this->*parse)()) {
          return EXECUTE_ERR_PROTOCOL;
        }
        return EXECUTE_OK;
      }
    } else if (err == SEND_RESULT_COMM_TIMEOUT) {
      if (com_timeout != nullptr) {
        com_timeout(index, ctrl->getLastTxArgument());
      }
    }

    return EXECUTE_ERR_I2C_COMM;
  }
  return EXECUTE_ERR_SINTAX;
}

} // namespace Opta

/* ************************************************************************** */
/* SPECIFIC PUBLIC FUNCTION FOR THIS EXPANSION                                */
/* ************************************************************************** */

/*_____________________________________________________GET SELECTED EXPANSION */
uint8_t R4DisplayExpansion::getSelectedExpansion(){
  uint8_t err = execute(EXECUTE_GET_SELECTED_EXPANSION);
  //Serial.println("Execute get seletecte expansion err = " + String(err));
  unsigned int rv = 0;
  read(ADD_SELECTED_EXPANSION,rv);

  return (uint8_t)rv;
}

/*_____________________________________________SET NUM OF AVAILABLE EXPANSINS */
void R4DisplayExpansion::setNumOfExpansions(uint8_t n){
  write(ADD_NUM_OF_EXPANSIONS,(unsigned int)n);
  uint8_t err = execute(EXECUTE_SET_NUM_OF_EXPANSION);
  //Serial.println("Execute num of expansion err = " + String(err));

}

/*________________________________________SET THE SELECTED EXPANSION FEATURES */ 
void R4DisplayExpansion::setExpansionFeatures(uint8_t type, 
                                              uint8_t index, 
                                              uint8_t n_channels){
  write(ADD_EXPANSION_TYPE,(unsigned int)type);
  write(ADD_SELECTED_EXPANSION,(unsigned int)index);
  write(ADD_EXPANSION_NUM_OF_CHANNELS,(unsigned int)n_channels);
  uint8_t err = execute(EXECUTE_SET_EXPANSION_FEATURES);
  //Serial.println("Execute set expansion setExpansionFeatures err = " + String(err));

}

/*___________________________________________SET SINGLE CHANNEL CONFIGURATION */
void R4DisplayExpansion::setChannelConfiguration(uint8_t ch, 
                                                 chType_t  type, 
                                                 float v1, 
                                                 chUnit_t u1,
                                                 float v2, 
                                                 chUnit_t u2){
  write(ADD_SELECTED_CHANNEL,(unsigned int)ch);
  write(ADD_SELECTED_CHANNEL_TYPE,(unsigned int)type);
  write(ADD_CHANNEL_V1,v1);
  write(ADD_CHANNEL_U1,(unsigned int)u1);
  write(ADD_CHANNEL_V2,v2);
  write(ADD_CHANNEL_U2,(unsigned int)u2);
  uint8_t err = execute(EXECUTE_SET_CHANNEL_CONFIGURATION);
  //Serial.println("Execute set channel configuration err = " + String(err));

}
#endif
