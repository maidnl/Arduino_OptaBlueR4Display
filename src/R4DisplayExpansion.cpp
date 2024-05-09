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

/* makeExpansion MANDATORY! (and always used)*/
Expansion *R4DisplayExpansion::makeExpansion() {
  return new R4DisplayExpansion();
}

/* getProduct MANDATORY! (and always used) */
std::string R4DisplayExpansion::getProduct() {
  std::string rv(UNOR4_DISPLAY_DESCRIPTION);
  return rv;
}

/* startUp MANDATORY (but not used in this example)*/
void R4DisplayExpansion::startUp(Controller *ptr) { (void)ptr; }

/* copy constructor MANDATORY! */
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
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

uint8_t R4DisplayExpansion::msg_get_buttons_status() {
  if (ctrl != nullptr) {
    return prepareGetMsg(ctrl->getTxBuffer(), ARG_R4DISPLAY_GET_BTN,
                         LEN_R4DISPLAY_GET_BTN, R4DISPLAY_GET_BTN_LEN);
  }
  return 0;
}

bool R4DisplayExpansion::parse_ans_get_buttons_status() {
  if (ctrl != nullptr) {
    /*
    for (int i = 0; i < ANS_R4DISPLAY_GET_BTN_LEN; i++) {
      Serial.print(ctrl->getRxBuffer()[i], HEX);
      Serial.print(" ");
    }

    Serial.println();
    */
    if (checkAnsGetReceived(ctrl->getRxBuffer(), ANS_ARG_R4DISPLAY_GET_BTN,
                            ANS_LEN_R4DISPLAY_GET_BTN,
                            ANS_R4DISPLAY_GET_BTN_LEN)) {
      iregs[ADD_BUTTONS_STATUS] = ctrl->getRx(ANS_BUTTON_STATUS_POS);

      return true;
    }
    return false;
  }
  return false;
}

BtnEvent_t R4DisplayExpansion::getButtonsStatus() {
  // Serial.println("Get button status");
  execute(OPERATION_GET_BUTTON_STATUS);
  return (BtnEvent_t)iregs[ADD_BUTTONS_STATUS];
}

unsigned int R4DisplayExpansion::execute(uint32_t what) {
  unsigned int rv = EXECUTE_OK;
  if (ctrl != nullptr) {
    switch (what) {
    /* ------------------------------------------------------------------- */
    case OPERATION_GET_BUTTON_STATUS:
      rv = i2c_transaction(&R4DisplayExpansion::msg_get_buttons_status,
                           &R4DisplayExpansion::parse_ans_get_buttons_status,
                           ANS_R4DISPLAY_GET_BTN_LEN + 1);
      // Serial.println("GET button rv = " + String(rv));
      break;
    /* ------------------------------------------------------------------- */
    default:
      rv = Expansion::execute(what);
      break;
    }
    ctrl->updateRegs(*this);
  } else {
    rv = EXECUTE_ERR_NO_CONTROLLER;
  }
  return rv;
}

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
#endif
