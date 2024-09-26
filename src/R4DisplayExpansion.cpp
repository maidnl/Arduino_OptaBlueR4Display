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
  
  uint8_t rv = prepareGetMsg(ctrl->getTxBuffer(),
                       Req_GET_EXP,
                       Len_GET_EXP);
  
  return rv;

}
/* ______________________________________________PREPARE: get change ch value */
uint8_t R4DisplayExpansion::msg_get_ch_change_value() {
  if (ctrl == nullptr) return 0;
  
  uint8_t rv = prepareGetMsg(ctrl->getTxBuffer(),
                              Cmd_GET_CH_VALUE,
                              Len_GET_CH_VALUE);
  return rv;
}

/* ________________________________________________PARSE: get change ch value */  
bool R4DisplayExpansion::parse_get_ch_change_value() {
  //Serial.print("PARSE parse_get_ch_change_value: ");
  if(checkAnsGetReceived(ctrl->getRxBuffer(), 
                         Ans_GET_CH_VALUE,
                         AnsLen_GET_CH_VALUE)) {

    /* This message (answer fo the get changed channel value contains the 
       following information: */  
    /* 1. the index of the expansion the user want to change */
    //Serial.print("RICEVUTO ");
    iregs[ADD_SELECTED_EXPANSION_INDEX] = ctrl->getRxBuffer()[Ans_GET_CH_VALUE_IndexPos];
    //Serial.print("INDEX " +  String(iregs[ADD_SELECTED_EXPANSION_INDEX]));
    /* 2. the type of the expansion the user want to change */
    iregs[ADD_SELECTED_EXPANSION_TYPE] = ctrl->getRxBuffer()[Ans_GET_CH_VALUE_ExpTypePos];
    //Serial.print(" TYPE " +  String(iregs[ADD_SELECTED_EXPANSION_TYPE]));
    /* 3. the channel the user want to change */
    iregs[ADD_SELECTED_EXPANSION_CHANNEL] = ctrl->getRxBuffer()[Ans_GET_CH_VALUE_ChannelPos];
    //Serial.print(" CHANNEL " +  String(iregs[ADD_SELECTED_EXPANSION_CHANNEL]));
    
    /* 4. the new value of the channel the user want to change */
    Float_u v;
    v.bytes[0] = ctrl->getRxBuffer()[Ans_GET_CH_VALUE_ValuePos + 0];
    v.bytes[1] = ctrl->getRxBuffer()[Ans_GET_CH_VALUE_ValuePos + 1];
    v.bytes[2] = ctrl->getRxBuffer()[Ans_GET_CH_VALUE_ValuePos + 2];
    v.bytes[3] = ctrl->getRxBuffer()[Ans_GET_CH_VALUE_ValuePos + 3];
    fregs[ADD_SELECTED_EXPANSION_CHANNEL_VALUE] = v.value;
    //Serial.println(" VALUE " +  String(fregs[ADD_SELECTED_EXPANSION_CHANNEL_VALUE]));
    return true;
  }
  return false;
}

/* ____________________________________________PREPARE : get change ch config */
uint8_t R4DisplayExpansion::msg_get_ch_change_config(){
  if (ctrl == nullptr) return 0;
  
  uint8_t rv =  prepareGetMsg(ctrl->getTxBuffer(),
                              Cmd_GET_CH_CONFIG,
                              Len_GET_CH_CONFIG);
  return rv;
}

/* _______________________________________________PARSE: get change ch config */
bool R4DisplayExpansion::parse_get_ch_change_config() {
  
  //Serial.print("PARSE parse_get_ch_change_config: ");
  //for(int i = 0; i < getExpectedAnsLen(Ans_GET_CH_CONFIG_Len); i++) {  
    //Serial.print(ctrl->getRxBuffer()[i], HEX);
    //Serial.print(" ");
  //}
  //Serial.println();
  
  if(checkAnsGetReceived(ctrl->getRxBuffer(), 
                         Ans_GET_CH_CONFIG,
                         AnsLen_GET_CH_CONFIG)) {
    
    //Serial.print("RICEVUTO ");
    iregs[ADD_SELECTED_EXPANSION_INDEX] = ctrl->getRxBuffer()[Ans_GET_CH_CONFIG_IndexPos];
    //Serial.print("INDEX " +  String(iregs[ADD_SELECTED_EXPANSION_INDEX]));
    iregs[ADD_SELECTED_EXPANSION_TYPE] = ctrl->getRxBuffer()[Ans_GET_CH_CONFIG_ExpTypePos];
    //Serial.print(" TYPE " +  String(iregs[ADD_SELECTED_EXPANSION_TYPE]));
    iregs[ADD_SELECTED_EXPANSION_CHANNEL] = ctrl->getRxBuffer()[Ans_GET_CH_CONFIG_ChannelPos];
    //Serial.print(" CHANNEL " +  String(iregs[ADD_SELECTED_EXPANSION_CHANNEL]));
    iregs[ADD_SELECTED_EXPANSION_CHANNEL_CONFIG] = ctrl->getRxBuffer()[Ans_GET_CH_CONFIG_ConfigPos];
    //Serial.println(" CONFIG " +  String(iregs[ADD_SELECTED_EXPANSION_CHANNEL_CONFIG]));

    return true;
  }
  return false;
}




/* _________________________________________PARSE: ans get selected expansion */
bool R4DisplayExpansion::parse_ans_get_selected_expansion() {
  if (ctrl == nullptr) return false;
  /*
  Serial.print("PARSE get selected expansion: ");
  for(int i = 0; i < getExpectedAnsLen(AnsLen_GET_EXP); i++) {  
    Serial.print(ctrl->getRxBuffer()[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  */
  if(checkAnsGetReceived(ctrl->getRxBuffer(), 
                         Ans_GET_EXP,
                         AnsLen_GET_EXP)) {
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
                             Len_ACK);

}

/* ___________________________________PREPARE: NUMBER OF EXPANSION DISCOVERED */
uint8_t R4DisplayExpansion::msg_set_num_of_expansions() {
  if (ctrl == nullptr) return 0;

  ctrl->getTxBuffer()[EXP_NUM_NumPos] = iregs[ADD_NUM_OF_EXPANSIONS];
  uint8_t rv = prepareSetMsg(ctrl->getTxBuffer(), 
                       Cmd_EXP_NUM,
                       Len_EXP_NUM);
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
                       Len_EXP_FEATURES);
  return rv;

}

/* ____________________________________________PREPARE: CHANNEL CONFIGURATION */
uint8_t R4DisplayExpansion::msg_set_channel_configuration() {
  if (ctrl == nullptr) return 0;

  Float_u v;

  ctrl->getTxBuffer()[CH_CFG_ChPos] = iregs[ADD_SELECTED_CHANNEL];
  v.value = fregs[ADD_CHANNEL_V1];
  ctrl->getTxBuffer()[CH_CFG_Func1Pos] = iregs[ADD_SELECTED_CH_FUNC1];
  ctrl->getTxBuffer()[CH_CFG_Type1Pos] = iregs[ADD_SELECTED_CH_TYPE1];
  ctrl->getTxBuffer()[CH_CFG_V1Pos_0] = v.bytes[0];
  ctrl->getTxBuffer()[CH_CFG_V1Pos_1] = v.bytes[1];
  ctrl->getTxBuffer()[CH_CFG_V1Pos_2] = v.bytes[2];
  ctrl->getTxBuffer()[CH_CFG_V1Pos_3] = v.bytes[3];
  ctrl->getTxBuffer()[CH_CFG_U1Pos] = iregs[ADD_CHANNEL_U1];
  v.value = fregs[ADD_CHANNEL_V2];
  ctrl->getTxBuffer()[CH_CFG_Func2Pos] = iregs[ADD_SELECTED_CH_FUNC2];
  ctrl->getTxBuffer()[CH_CFG_Type2Pos] = iregs[ADD_SELECTED_CH_TYPE2];
  ctrl->getTxBuffer()[CH_CFG_V2Pos_0] = v.bytes[0];
  ctrl->getTxBuffer()[CH_CFG_V2Pos_1] = v.bytes[1];
  ctrl->getTxBuffer()[CH_CFG_V2Pos_2] = v.bytes[2];
  ctrl->getTxBuffer()[CH_CFG_V2Pos_3] = v.bytes[3];
  ctrl->getTxBuffer()[CH_CFG_U2Pos] = iregs[ADD_CHANNEL_U2];
  uint8_t rv = prepareSetMsg(ctrl->getTxBuffer(), 
                       Cmd_CH_CFG,
                       Len_CH_CFG);
  return rv;
}



/* __________________________________________________________________ EXECUTE */
unsigned int R4DisplayExpansion::execute(uint32_t what) {
  i2c_rv = EXECUTE_OK;
  if (ctrl != nullptr) {
    
    switch (what) {
    /* ------------------------------------------------------------------- */
    case EXECUTE_GET_SELECTED_EXPANSION:
      I2C_TRANSACTION(msg_get_selected_expansion,
                      parse_ans_get_selected_expansion,
                      getExpectedAnsLen(AnsLen_GET_EXP));
      break;
    /* ------------------------------------------------------------------- */
    case EXECUTE_SET_NUM_OF_EXPANSION:
      I2C_TRANSACTION(msg_set_num_of_expansions,
                      parse_ack,
                      getExpectedAnsLen(Len_ACK));
      break;
    /* ------------------------------------------------------------------- */
    case EXECUTE_SET_EXPANSION_FEATURES:
      I2C_TRANSACTION(msg_set_expansion_features,
                      parse_ack,
                      getExpectedAnsLen(Len_ACK));
      break;
    /* ------------------------------------------------------------------- */
    case EXECUTE_SET_CHANNEL_CONFIGURATION:
      I2C_TRANSACTION(msg_set_channel_configuration,
                      parse_ack,
                      getExpectedAnsLen(Len_ACK));
      break;
    /* ------------------------------------------------------------------- */
    case EXECUTE_GET_CH_VALUE:
      I2C_TRANSACTION(msg_get_ch_change_value,
                      parse_get_ch_change_value,
                      getExpectedAnsLen(AnsLen_GET_CH_VALUE));
      break;  
    /* ------------------------------------------------------------------- */
    case EXECUTE_GET_CH_CONFIG:
      I2C_TRANSACTION(msg_get_ch_change_config,
                      parse_get_ch_change_config,
                      getExpectedAnsLen(AnsLen_GET_CH_CONFIG));
      break;    
    /* ------------------------------------------------------------------- */  
    default:
      i2c_rv = Expansion::execute(what);
      break;
    }
    /* IMPORTANT: call this if you want to keep your expansion copies in sync */
    ctrl->updateRegs(*this);
  } else {
    i2c_rv = EXECUTE_ERR_NO_CONTROLLER;
  }
  return i2c_rv;
}

} // namespace Opta



/* ************************************************************************** */
/* SPECIFIC PUBLIC FUNCTION FOR THIS EXPANSION                                */
/* ************************************************************************** */

/*_____________________________________________________GET SELECTED EXPANSION */
uint8_t R4DisplayExpansion::getSelectedExpansion(){
  /*uint8_t err =*/ execute(EXECUTE_GET_SELECTED_EXPANSION);
  //Serial.println("Execute get seletecte expansion err = " + String(err));
  unsigned int rv = 0;
  read(ADD_SELECTED_EXPANSION,rv);

  return (uint8_t)rv;
}

/*_____________________________________________SET NUM OF AVAILABLE EXPANSINS */
void R4DisplayExpansion::setNumOfExpansions(uint8_t n){
  write(ADD_NUM_OF_EXPANSIONS,(unsigned int)n);
  /*uint8_t err = */ execute(EXECUTE_SET_NUM_OF_EXPANSION);
  //Serial.println("Execute num of expansion err = " + String(err));

}

/*________________________________________SET THE SELECTED EXPANSION FEATURES */ 
void R4DisplayExpansion::setExpansionFeatures(uint8_t type, 
                                              uint8_t index, 
                                              uint8_t n_channels){
  write(ADD_EXPANSION_TYPE,(unsigned int)type);
  write(ADD_SELECTED_EXPANSION,(unsigned int)index);
  write(ADD_EXPANSION_NUM_OF_CHANNELS,(unsigned int)n_channels);
  /*uint8_t err =*/ execute(EXECUTE_SET_EXPANSION_FEATURES);
  //Serial.println("Execute set expansion setExpansionFeatures err = " + String(err));

}

/*___________________________________________SET SINGLE CHANNEL CONFIGURATION */
void R4DisplayExpansion::setChannelConfiguration(uint8_t ch, 
                                                 chFun_t f1,
                                                 chType_t  t1,
                                                 float v1, 
                                                 chUnit_t u1,
                                                 chFun_t f2,
                                                 chType_t  t2, 
                                                 float v2, 
                                                 chUnit_t u2){
  write(ADD_SELECTED_CHANNEL,(unsigned int)ch);
  write(ADD_SELECTED_CH_FUNC1,(unsigned int)f1);
  write(ADD_SELECTED_CH_TYPE1,(unsigned int)t1);
  write(ADD_CHANNEL_V1,v1);
  write(ADD_CHANNEL_U1,(unsigned int)u1);
  write(ADD_SELECTED_CH_FUNC2,(unsigned int)f2);
  write(ADD_SELECTED_CH_TYPE2,(unsigned int)t2);
  write(ADD_CHANNEL_V2,v2);
  write(ADD_CHANNEL_U2,(unsigned int)u2);
  /*uint8_t err = */execute(EXECUTE_SET_CHANNEL_CONFIGURATION);
  //Serial.println("Execute set channel configuration err = " + String(err));

}

/* __________________________________________________________GET UPDATE VALUE */
  bool R4DisplayExpansion::getUpdateChValue(ChangeChValue &cfg) {
    if(execute(EXECUTE_GET_CH_VALUE) == EXECUTE_OK) {
      if(iregs[ADD_SELECTED_EXPANSION_INDEX] < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
        cfg.exp_index = iregs[ADD_SELECTED_EXPANSION_INDEX];
        cfg.exp_type = iregs[ADD_SELECTED_EXPANSION_TYPE];
        cfg.exp_channel = iregs[ADD_SELECTED_EXPANSION_CHANNEL];
        cfg.value = fregs[ADD_SELECTED_EXPANSION_CHANNEL_VALUE];

        return true;
      }
    }

    return false;  
  }

/*___________________________________________________________GET UPDATE CONFI */
  bool R4DisplayExpansion::getUpdateChConfig(ChangeChConfig &cfg){
    if(execute(EXECUTE_GET_CH_CONFIG) == EXECUTE_OK) {
      if(iregs[ADD_SELECTED_EXPANSION_INDEX] < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
        cfg.exp_index = iregs[ADD_SELECTED_EXPANSION_INDEX];
        cfg.exp_type = iregs[ADD_SELECTED_EXPANSION_TYPE];
        cfg.exp_channel = iregs[ADD_SELECTED_EXPANSION_CHANNEL];
        cfg.config = iregs[ADD_SELECTED_EXPANSION_CHANNEL_CONFIG];
        return true;
      }
    }
    return false;  
  }




void R4DisplayExpansion::send_dig_exp_info_2_r4(uint8_t index) {
  /* handle selected expansion - if digital*/
  DigitalExpansion de = OptaController.getExpansion(index);
  if(de) {
    /* Telling r4 it has to display a digital expansion 
      using EXPANSION_DIGITAL_INVALID as generic digital expansion type */
    setExpansionFeatures(EXPANSION_DIGITAL_INVALID, index, 8+16);
   
    /* informing r4 about the channel status of digital output */
    for(int k = 0; k < DIGITAL_OUT_NUM; k++) {
      setChannelConfiguration(k, 
                                 CH_FUNCTION_DIGITAL,
                                 CH_TYPE_OUTPUT,
                                 (float)de.digitalOutRead(k), 
                                 CH_UNIT_DIGIT,
                                 CH_FUNCTION_UNAVAILABLE,
                                 CH_TYPE_NO_TYPE, 
                                 0.0, 
                                 CH_UNIT_NO_UNIT);

      
    }
    /* informing r4 about the channel status of digital/analog input */
    for(int k = 0; k < DIGITAL_IN_NUM; k++) {
      
      setChannelConfiguration(k + DIGITAL_OUT_NUM, 
                                 CH_FUNCTION_DIGITAL,
                                 CH_TYPE_INPUT,
                                 (float)de.digitalRead(k,true), 
                                 CH_UNIT_DIGIT,
                                 CH_FUNCTION_ADC,
                                 CH_TYPE_VOLTAGE, 
                                 (float)de.pinVoltage(k,true), 
                                 CH_UNIT_VOLT);
    }
  }
}


void R4DisplayExpansion::send_analog_info_2_r4(uint8_t index) {
  /* handle selected expansion - if analog */
   AnalogExpansion ae = OptaController.getExpansion(index);
   if(ae) {
      setExpansionFeatures(EXPANSION_OPTA_ANALOG, index, 8+4);
      for(int k = 0; k < OA_AN_CHANNELS_NUM; k++) {
        /* HIGH IMPEDENCE */
        if(ae.isChHighImpedance(k)) {
          if(ae.isChVoltageAdc(k)) {
            setChannelConfiguration(k, 
                                       CH_FUNCTION_HIGH_IMPEDENCE,
                                       CH_TYPE_NO_TYPE,
                                       0.0, 
                                       CH_UNIT_NO_UNIT,
                                       CH_FUNCTION_ADC,
                                       CH_TYPE_VOLTAGE, 
                                       (float)ae.pinVoltage(k,true), 
                                       CH_UNIT_VOLT);
          }
          else {
            setChannelConfiguration(k, 
                                       CH_FUNCTION_HIGH_IMPEDENCE,
                                       CH_TYPE_NO_TYPE,
                                       0.0, 
                                       CH_UNIT_NO_UNIT,
                                       CH_FUNCTION_UNAVAILABLE,
                                       CH_TYPE_NO_TYPE, 
                                       0.0, 
                                       CH_UNIT_NO_UNIT);
          }
        }
        /* DIGITAL INPUT */
        else if(ae.isChDigitalInput(k)) {
          if(ae.isChVoltageAdc(k)) {
            setChannelConfiguration(k, 
                                       CH_FUNCTION_DIGITAL,
                                       CH_TYPE_INPUT,
                                       (float)ae.digitalRead(k,true), 
                                       CH_UNIT_DIGIT,
                                       CH_FUNCTION_ADC,
                                       CH_TYPE_VOLTAGE, 
                                       (float)ae.pinVoltage(k,true), 
                                       CH_UNIT_VOLT);
          }
          else if(ae.isChCurrentAdc(k)) {
            setChannelConfiguration(k, 
                                       CH_FUNCTION_DIGITAL,
                                       CH_TYPE_INPUT,
                                       (float)ae.digitalRead(k,true), 
                                       CH_UNIT_DIGIT,
                                       CH_FUNCTION_ADC,
                                       CH_TYPE_CURRENT, 
                                       (float)ae.pinCurrent(k,true), 
                                       CH_UNIT_mAMPERE);
          }
          else {
            setChannelConfiguration(k, 
                                       CH_FUNCTION_DIGITAL,
                                       CH_TYPE_INPUT,
                                       (float)ae.digitalRead(k,true), 
                                       CH_UNIT_DIGIT,
                                       CH_FUNCTION_UNAVAILABLE,
                                       CH_TYPE_NO_TYPE, 
                                       0.0, 
                                       CH_UNIT_NO_UNIT);
          }
        }
        /* VOLTAGE ADC */
        else if(ae.isChVoltageAdc(k)) {
          if(ae.isChCurrentAdc(k)) {
            setChannelConfiguration(k, 
                                       CH_FUNCTION_ADC,
                                       CH_TYPE_VOLTAGE, 
                                       (float)ae.pinVoltage(k,true), 
                                       CH_UNIT_VOLT,
                                       CH_FUNCTION_ADC,
                                       CH_TYPE_CURRENT, 
                                       (float)ae.pinCurrent(k,true), 
                                       CH_UNIT_mAMPERE);
          }
          else {

            setChannelConfiguration(k, 
                                       CH_FUNCTION_ADC,
                                       CH_TYPE_VOLTAGE, 
                                       (float)ae.pinVoltage(k,true), 
                                       CH_UNIT_VOLT,
                                       CH_FUNCTION_UNAVAILABLE,
                                       CH_TYPE_NO_TYPE, 
                                       0.0, 
                                       CH_UNIT_NO_UNIT);
          }
        }
        /* CURRENT ADC */
        else if(ae.isChCurrentAdc(k)) {
          if(ae.isChVoltageAdc(k)) {
            setChannelConfiguration(k, 
                                       CH_FUNCTION_ADC,
                                       CH_TYPE_CURRENT, 
                                       (float)ae.pinCurrent(k,true), 
                                       CH_UNIT_mAMPERE,
                                       CH_FUNCTION_ADC,
                                       CH_TYPE_VOLTAGE, 
                                       (float)ae.pinVoltage(k,true), 
                                       CH_UNIT_VOLT);
          }
          else {
            setChannelConfiguration(k, 
                                       CH_FUNCTION_ADC,
                                       CH_TYPE_CURRENT, 
                                       (float)ae.pinCurrent(k,true), 
                                       CH_UNIT_mAMPERE,
                                       CH_FUNCTION_UNAVAILABLE,
                                       CH_TYPE_NO_TYPE, 
                                       0.0, 
                                       CH_UNIT_NO_UNIT);
          }
        }
        /* VOLTAGE DAC */
        else if(ae.isChVoltageDac(k)) {
          if(ae.isChCurrentAdc(k)) {
            setChannelConfiguration(k, 
                                       CH_FUNCTION_DAC,
                                       CH_TYPE_VOLTAGE, 
                                       (float)ae.pinVoltage(k,true), 
                                       CH_UNIT_VOLT,
                                       CH_FUNCTION_ADC,
                                       CH_TYPE_CURRENT, 
                                       (float)ae.pinCurrent(k,true), 
                                       CH_UNIT_mAMPERE);
          }
          else {
            setChannelConfiguration(k, 
                                       CH_FUNCTION_DAC,
                                       CH_TYPE_VOLTAGE, 
                                       (float)ae.pinVoltage(k,true), 
                                       CH_UNIT_VOLT,
                                       CH_FUNCTION_UNAVAILABLE,
                                       CH_TYPE_NO_TYPE, 
                                       0.0, 
                                       CH_UNIT_NO_UNIT);
          }

        }
        /* CURRENT DAC */
        else if(ae.isChCurrentDac(k)) {
          if(ae.isChVoltageAdc(k)) {
            setChannelConfiguration(k, 
                                       CH_FUNCTION_DAC,
                                       CH_TYPE_CURRENT, 
                                       (float)ae.pinCurrent(k,true), 
                                       CH_UNIT_mAMPERE,
                                       CH_FUNCTION_ADC,
                                       CH_TYPE_VOLTAGE, 
                                       (float)ae.pinVoltage(k,true), 
                                       CH_UNIT_VOLT);
          }
          else {
            setChannelConfiguration(k, 
                                       CH_FUNCTION_DAC,
                                       CH_TYPE_CURRENT, 
                                       (float)ae.pinCurrent(k,true), 
                                       CH_UNIT_mAMPERE,
                                       CH_FUNCTION_UNAVAILABLE,
                                       CH_TYPE_NO_TYPE, 
                                       0.0, 
                                       CH_UNIT_NO_UNIT);
          }
        }
        /* RTD */
        else if(ae.isChRtd2Wires(k)) {
          setChannelConfiguration(k, 
                                     CH_FUNCTION_RTD,
                                     CH_TYPE_2_WIRES, 
                                     (float)ae.getRtd(k), 
                                     CH_UNIT_OHM,
                                     CH_FUNCTION_UNAVAILABLE,
                                     CH_TYPE_NO_TYPE, 
                                     0.0, 
                                     CH_UNIT_NO_UNIT);
        }
        else if(ae.isChRtd3Wires(k)) {
          setChannelConfiguration(k, 
                                     CH_FUNCTION_RTD,
                                     CH_TYPE_3_WIRES, 
                                     (float)ae.getRtd(k), 
                                     CH_UNIT_OHM,
                                     CH_FUNCTION_UNAVAILABLE,
                                     CH_TYPE_NO_TYPE, 
                                     0.0, 
                                     CH_UNIT_NO_UNIT);
        }




        for(int k = OA_FIRST_PWM_CH; k <= OA_LAST_PWM_CH; k++) {
          setChannelConfiguration(k, 
                                     CH_FUNCTION_PWM,
                                     CH_TYPE_FREQ, 
                                     (float)ae.getPwmFreqHz(k), 
                                     CH_UNIT_HERTZ,
                                     CH_FUNCTION_PWM,
                                     CH_TYPE_DUTY, 
                                     (float)ae.getPwmPulsePerc(k), 
                                     CH_UNIT_PERC);
        }

      }
   }
}

void R4DisplayExpansion::send_r4_info_2_r4(uint8_t index) {
  R4DisplayExpansion re = OptaController.getExpansion(index);
  if(re) {
    setExpansionFeatures(UNO_R4_DISPLAY_ADDITIONAL_TYPE, index, 0);
  }
}


void R4DisplayExpansion::manage_chg_value() {
  ChangeChValue chg;
  if(getUpdateChValue(chg)) {
    
    if(chg.exp_type == EXPANSION_OPTA_ANALOG) {
      AnalogExpansion ae = OptaController.getExpansion(chg.exp_index);
      if(ae) {
        
        /* VOLTAGE DAC */
        if(ae.isChVoltageDac(chg.exp_channel)) {
          
          ae.pinVoltage(chg.exp_channel,chg.value);
        }
        /* CURRENT DAC */
        else if(ae.isChCurrentDac(chg.exp_channel)) {
          ae.pinCurrent(chg.exp_channel,chg.value);
        }
      }
    }
    else if(chg.exp_type == EXPANSION_DIGITAL_INVALID || 
            chg.exp_type == EXPANSION_OPTA_DIGITAL_MEC ||
            chg.exp_type == EXPANSION_OPTA_DIGITAL_STS) {
      
      DigitalExpansion de = OptaController.getExpansion(chg.exp_index);
      if(de) {
        
        de.digitalWrite(chg.exp_channel, (PinStatus)chg.value,true);
      }
    }

  }
}


void R4DisplayExpansion::manage_chg_config() {
  ChangeChConfig chg;
  if(getUpdateChConfig(chg)) {
    
    if(chg.exp_type == EXPANSION_OPTA_ANALOG) {
      
      AnalogExpansion ae = OptaController.getExpansion(chg.exp_index);
      
      if(ae) {
        
        if(chg.config == CH_CONFIG_DAC_VOLTAGE) {
          ae.beginChannelAsVoltageDac(chg.exp_channel);
        }
        else if(chg.config == CH_CONFIG_DAC_CURRENT) {
          ae.beginChannelAsCurrentDac(chg.exp_channel);
        }
        else if(chg.config == CH_CONFIG_ADC_VOLTAGE) {
          ae.beginChannelAsVoltageAdc(chg.exp_channel);
        }
        else if(chg.config == CH_CONFIG_ADC_CURRENT) {
          ae.beginChannelAsCurrentAdc(chg.exp_channel);
        }
        else if(chg.config == CH_CONFIG_RTD_2_WIRES) {
          ae.beginChannelAsRtd(chg.exp_channel, false, 1.2);
        }
        else if(chg.config == CH_CONFIG_RTD_3_WIRES) {
          ae.beginChannelAsRtd(chg.exp_channel, true, 1.2);
        }
        else if(chg.config == CH_CONFIG_DIGTAL_INP) {
          ae.beginChannelAsDigitalInput(chg.exp_channel);
        }
        else if(chg.config == CH_CONFIG_RTD_HIGH_IM) {
          ae.beginChannelAsHighImpedance(chg.exp_channel);
        }
      }
    }

  }
}


void R4DisplayExpansion::updateDisplay() {
  
      /* telling R4 how many expansion are present */
      setNumOfExpansions(OptaController.getExpansionNum());
      /* getting from R4 if user has selected an expansion */
      selected_expansion = getSelectedExpansion(); 
      /* send information to be displayed about selected expansion */
      send_dig_exp_info_2_r4(selected_expansion);
      send_analog_info_2_r4(selected_expansion);
      send_r4_info_2_r4(selected_expansion);
}

void R4DisplayExpansion::updateExpansions() {
  
      /* get if user want to change a channel value */
      manage_chg_value();
      /* get if user want to change a channel value */
      manage_chg_config();
       
}


#endif
