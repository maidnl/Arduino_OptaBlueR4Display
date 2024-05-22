/* -------------------------------------------------------------------------- */
/* FILE NAME:   R4DisplayExpansion.h
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

#ifndef R4DISPLAYEXPANSION
#define R4DISPLAYEXPANSION

#include "OptaController.h"
#include "OptaExpansion.h"
#include "OptaUnoR4DisplayProtocol.h"
#include "R4DisplayAddress.h"
#include "UnoR4DisplayCommonCfg.h"
#include <stdint.h>


#define EXECUTE_GET_SELECTED_EXPANSION     255
#define EXECUTE_SET_NUM_OF_EXPANSION       254
#define EXECUTE_SET_EXPANSION_FEATURES     253
#define EXECUTE_SET_CHANNEL_CONFIGURATION  252
#define EXECUTE_GET_CH_VALUE               251
#define EXECUTE_GET_CH_CONFIG              250

namespace Opta {

class ChangeChValue {
public:
  uint8_t exp_index;
  uint8_t exp_type;
  uint8_t exp_channel;
  float value;
};

class ChangeChConfig {
public:
  uint8_t exp_index;
  uint8_t exp_type;
  uint8_t exp_channel;
  uint8_t config;
};


class R4DisplayExpansion : public Expansion {

public:
  R4DisplayExpansion();
  ~R4DisplayExpansion();
  R4DisplayExpansion(Expansion &other);
  unsigned int execute(uint32_t what) override;
  /* get from r4 display what expansion the user has selected */
  uint8_t getSelectedExpansion();
  /* tells the R4 display how many expansion are available */
  void setNumOfExpansions(uint8_t n);
  /* tells the R4 display the basic features of the selected expansion */
  void setExpansionFeatures(uint8_t type, uint8_t index, uint8_t n_channels);
  /* tells the R4 display how is the status of a single channel 
     each channel can have 2 values (for example digital input that is also
     adc */
  void setChannelConfiguration(uint8_t ch, 
                               chType_t  type, 
                               float v1, 
                               chUnit_t u1,
                               float v2, 
                               chUnit_t u2);

  /* if nothing has to be changed false
     if something has to be changed it return true and the configuration */
  bool getUpdateChValue(ChangeChValue &cfg);
  /* if nothing has to be changed false
     if something has to be changed it return true and the configuration */
  bool getUpdateChConfig(ChangeChConfig &cfg);


  /* static mandatory function to be implemented by all expansions! */
  static Expansion *makeExpansion();
  static std::string getProduct();
  static void startUp(Controller *ptr);

protected:
  


  unsigned int i2c_transaction(uint8_t (R4DisplayExpansion::*prepare)(),
                               bool (R4DisplayExpansion::*parse)(),
                               int rx_bytes);
  uint8_t msg_get_selected_expansion();
  bool parse_ans_get_selected_expansion();
  bool parse_ack();
  uint8_t msg_set_num_of_expansions();
  uint8_t msg_set_expansion_features();
  uint8_t msg_set_channel_configuration();

  uint8_t msg_get_ch_change_config();
  bool parse_get_ch_change_config();

  uint8_t msg_get_ch_change_value();
  bool parse_get_ch_change_value();
};

} // namespace Opta
#endif
