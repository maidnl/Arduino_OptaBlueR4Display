/* -------------------------------------------------------------------------- */
/* FILE NAME:   UnoR4DisplayOtherClasses.h
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240523
   DESCRIPTION: 
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla 
                Public License (MPL), v 2.0. You can obtain a copy of the MPL 
                at http://mozilla.org/MPL/2.0/. 
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */


#ifndef UNOR4DISPLAYOTHERCLASSES
#define UNOR4DISPLAYOTHERCLASSES

#include "UnoR4DisplayCommonCfg.h"
#include "OptaBluePrintCfg.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdint.h>
#include <memory>

#define MAX_FUNCTIONS_PER_CHANNEL 2

namespace r4display {

/* ########################################################################## */
/*                   CLASSES FOR UNIT OF MEASURE                              */ 
/* ########################################################################## */  

/* ___________________________________________________________BASE Unit Class */
class ChUnit {
public:
  ChUnit() {}
  ~ChUnit() {}
  virtual void display(Adafruit_SSD1306 &d) {
    
    (void)d;
  }
};
/* _____________________________________________________________________Volts */
class ChUnitVolt : public ChUnit {
  public:
  void display(Adafruit_SSD1306 &d) override {  d.print("V");}
};
/* ________________________________________________________________milliVolts */
class ChUnitMilliVolt : public ChUnit {
  public:
  void display(Adafruit_SSD1306 &d) override {  d.print("mV");}
};
/* ____________________________________________________________________Ampere */
class ChUnitAmpere : public ChUnit {
  public:
  void display(Adafruit_SSD1306 &d) override {  d.print("A");}
};
/* _______________________________________________________________milliAmpere */
class ChUnitMilliAmpere : public ChUnit {
  public:
  void display(Adafruit_SSD1306 &d) override {  d.print("mA");}
};
/* _______________________________________________________________________ohm */
class ChUnitOhm : public ChUnit {
  public:
  void display(Adafruit_SSD1306 &d) override {  d.print("ohm");}
};
/* _____________________________________________________________________Hertz */
class ChUnitHetz : public ChUnit {
  public:
  void display(Adafruit_SSD1306 &d) override {  d.print("Hz");}
};
/* _________________________________________________________________KiloHertz */
class ChUnitKiloHetz : public ChUnit {
  public:
  void display(Adafruit_SSD1306 &d) override { d.print("KHz");}
};
/* _________________________________________________________________MegaHertz */
class ChUnitMegaHetz : public ChUnit {
  public:
  void display(Adafruit_SSD1306 &d) override { d.print("MHz");}
};
/* ________________________________________________________________milliHertz */
class ChUnitMilliHetz : public ChUnit{
  public:
  void display(Adafruit_SSD1306 &d) override { d.print("mHz");}
};
/* ___________________________________________________________________percent */
class ChUnitPerc : public ChUnit {
  public:
  void display(Adafruit_SSD1306 &d) override { d.print("%");}
};



/* ########################################################################## */
/*                            CLASSEs EXPANSIONs                              */ 
/* ########################################################################## */ 

class DisplayExpansion {
private:
  uint8_t type;
public:
   DisplayExpansion() {}
   virtual ~DisplayExpansion() {}
   virtual void display(Adafruit_SSD1306 &d) { (void)d; }
   virtual void display(Adafruit_SSD1306 &d, uint8_t i) { (void)d; (void)i;}
   virtual bool isConfigurable(uint8_t ch) {return false;}
   virtual uint8_t getConfigurationNumPerChannel() {return 0;}
   virtual float getMinDigitalValue() { return 0.0; }
   virtual float getMaxDigitalValue() { return 1.0; }
   virtual float getMinVoltageAdcValue() { return 0.0; }
   virtual float getMaxVoltageAdcValue() { return 10.0; } 
   virtual float getMinCurrentAdcValue() { return 0.0; }
   virtual float getMaxCurrentAdcValue() { return 25.0; } 
   virtual float getMinVoltageDacValue() { return 0.0; }
   virtual float getMaxVoltageDacValue() { return 11.0; } 
   virtual float getMinCurrentDacValue() { return 0.0; }
   virtual float getMaxCurrentDacValue() { return 25.0; } 

   virtual void displayConfiguration(Adafruit_SSD1306 &d, uint8_t cfg, uint8_t ch) { 
    (void)d; 
    (void)cfg;
    (void)ch;
   }
   uint8_t getType() {return type;}
   void setType(uint8_t t) {type = t;}
};

class DisplayExpansionAnalog : public DisplayExpansion {
public:
    DisplayExpansionAnalog() {}
    virtual ~DisplayExpansionAnalog() {}
    void display(Adafruit_SSD1306 &d) override {
      d.print("Analog");
    }
    void display(Adafruit_SSD1306 &d,uint8_t i) override {
      d.print("Analog ");
      d.print(i);
    }

    void displayConfiguration(Adafruit_SSD1306 &d, uint8_t cfg, uint8_t ch) override {
      (void)ch; 
      if(cfg < CH_CONFIG_NUM) {
        switch(cfg) {
        case CH_CONFIG_DAC_VOLTAGE:
          d.print("DAC Voltage");
        break;
        case CH_CONFIG_DAC_CURRENT:
          d.print("DAC Current");
        break;
        case CH_CONFIG_ADC_VOLTAGE:
          d.print("ADC Voltage");
        break;
        case CH_CONFIG_ADC_CURRENT:
          d.print("ADC Current");
        break;
        case CH_CONFIG_RTD_2_WIRES:
          d.print("RTD 2 wires");
        break;
        case CH_CONFIG_RTD_3_WIRES:
          d.print("RTD 3 wires");
        break;
        case CH_CONFIG_DIGTAL_INP:
          d.print("Digital INP");
        break;
        case CH_CONFIG_RTD_HIGH_IM:
          d.print("HIGH IMPEDE");
        break;
        default:

        break;
      
        }
      }
    }
    bool isConfigurable(uint8_t ch) override {
      if(ch < 8) {
        return true;
      }
      return false;
    }
};

class DisplayExpansionDigital : public DisplayExpansion {
public:
   DisplayExpansionDigital() {}
   virtual ~DisplayExpansionDigital() {}
   void display(Adafruit_SSD1306 &d) override {
      d.print("Digital");
   }
   void display(Adafruit_SSD1306 &d,uint8_t i) override {
      d.print("Digital ");
      d.print(i);
   }
   float getMinVoltageAdcValue() override { return 0.0; }
   float getMaxVoltageAdcValue() override { return 24.0; } 
   
   
};

class DisplayExpansionDigitalMec : public DisplayExpansion {
public:
   DisplayExpansionDigitalMec() {}
   virtual ~DisplayExpansionDigitalMec() {}
   void display(Adafruit_SSD1306 &d) override {
      d.print("Dig MEC");
   }
   void display(Adafruit_SSD1306 &d,uint8_t i) override {
      d.print("Dig MEC ");
      d.print(i);
   }
   float getMinVoltageAdcValue() override { return 0.0; }
   float getMaxVoltageAdcValue() override { return 24.0; } 
};

class DisplayExpansionDigitalSts : public DisplayExpansion {
public:
   DisplayExpansionDigitalSts() {}
   virtual ~DisplayExpansionDigitalSts() {}
   void display(Adafruit_SSD1306 &d) override {
      d.print("Dig STS");
   }
   void display(Adafruit_SSD1306 &d,uint8_t i) override {
      d.print("Dig STS ");
      d.print(i);
   }
   float getMinVoltageAdcValue() override { return 0.0; }
   float getMaxVoltageAdcValue() override { return 24.0; } 
};

class DisplayR4Display : public DisplayExpansion {
public:
   DisplayR4Display() {}
   virtual ~DisplayR4Display() {}
   void display(Adafruit_SSD1306 &d) override {
      d.print("R4 Displ");
   }
   void display(Adafruit_SSD1306 &d,uint8_t i) override {
      d.print("R4 Displ ");
      d.print(i);
   }
};


DisplayExpansion *factoryExpansion(uint8_t type);


/* ########################################################################## */
/*                     CLASSES CHANNELS FUNCTIONS                             */ 
/* ########################################################################## */  

/* ______________________________________________ BASE Channel Function class */
class ChFunction { 
protected:
  float value;
  std::shared_ptr<ChUnit> unit;
  
public:
  ChFunction(chUnit_t u) : value(0.0) {
    switch(u) {
      case CH_UNIT_NO_UNIT: unit = std::make_shared<ChUnit>(); break;
      case CH_UNIT_VOLT: unit = std::make_shared<ChUnitVolt>(); break;
      case CH_UNIT_mVOLT: unit = std::make_shared<ChUnitMilliVolt>(); break;
      case CH_UNIT_AMPERE: unit = std::make_shared<ChUnitAmpere>(); break;
      case CH_UNIT_mAMPERE: unit = std::make_shared<ChUnitMilliAmpere>(); break;
      case CH_UNIT_OHM: unit = std::make_shared<ChUnitOhm>(); break;
      case CH_UNIT_DIGIT: unit = std::make_shared<ChUnit>(); break;
      case CH_UNIT_HERTZ: unit = std::make_shared<ChUnitHetz>(); break;
      case CH_UNIT_mHERTZ: unit = std::make_shared<ChUnitMilliHetz>(); break;
      case CH_UNIT_KHERTZ: unit = std::make_shared<ChUnitKiloHetz>(); break;
      case CH_UNIT_MHERTZ: unit = std::make_shared<ChUnitMegaHetz>(); break;
      case CH_UNIT_PERC: unit = std::make_shared<ChUnitPerc>(); break;
      default: unit = std::make_shared<ChUnit>(); break;
    }
  }
  virtual ~ChFunction() { /* shared pointer shoul distruct itself */}
  ChFunction(const ChFunction &other) {
    
    value = other.value;
    unit = other.unit;
  }


  virtual float getMinValue(DisplayExpansion *p)      {(void)p; return 0.0;}
  virtual float getMaxValue(DisplayExpansion *p)      {(void)p; return 1.0;}
  virtual float getStepValue()     {return 1.0;}
  virtual float getLongStepValue() {return 10.0;}
  virtual uint8_t getDisplayPrecision() {return 1;}
  virtual void incrementValue(DisplayExpansion *p) {value += getStepValue();  clampValue(p); }
  virtual void decrementValue(DisplayExpansion *p) {value -= getStepValue();  clampValue(p); }
  virtual void bigIncrementValue(DisplayExpansion *p) {value += getLongStepValue(); clampValue(p);}
  virtual void bigDecrementValue(DisplayExpansion *p) {value -= getLongStepValue(); clampValue(p); }
  virtual bool isChangeable() {return false;}
  virtual void clampValue(DisplayExpansion *p) {
    ;
    value = (value < getMinValue(p)) ? getMinValue(p) : value;
    value = (value > getMaxValue(p)) ? getMaxValue(p) : value;
    
  }
  virtual void setValue(float v)  {value = v;}
  virtual float getValue() {return value; }
  virtual void displayShortDecription(Adafruit_SSD1306 &d) {}
  virtual void displayDescription(Adafruit_SSD1306 &d) {}
  virtual void displayUnit(Adafruit_SSD1306 &d) {
    if(unit != nullptr) {
      unit->display(d);
    }
  }
  virtual void  displayValue(Adafruit_SSD1306 &d) {
    d.print(value,getDisplayPrecision());
    d.print(" ");
    displayUnit(d);
  }
  

};

/* _________________________________________________ Function: HIGH IMPEDENCE */
class ChFunHighImpedence : public ChFunction {
public:
  ChFunHighImpedence(chUnit_t u) : ChFunction(u) {}
  ~ChFunHighImpedence(){}
  void displayDescription(Adafruit_SSD1306 &d) override {
    d.println("High impedence");
  }
  void displayShortDecription(Adafruit_SSD1306 &d) override {
    d.print("HIMP ");
  }
  void  displayValue(Adafruit_SSD1306 &d) override { (void)d; }
  float getMinValue(DisplayExpansion *p) override     {(void)p; return 0.0;}
  float getMaxValue(DisplayExpansion *p) override     {(void)p; return 0.0;}
};
/* __________________________________________________ Function: DIGITAL INPUT */
class ChFunDigitalInput : public ChFunction {
public:
  ChFunDigitalInput(chUnit_t u): ChFunction(u){}
  ~ChFunDigitalInput(){}
  uint8_t getDisplayPrecision() override {return 0;}
  void displayDescription(Adafruit_SSD1306 &d) override {
    d.println("Digital Input");
  }
  void  displayValue(Adafruit_SSD1306 &d) override {
    if(value == 0.0) { d.print("Off");}
    else { d.print("On");}
  }
  void displayShortDecription(Adafruit_SSD1306 &d) override {
    d.print("DINP ");
  }
  float getMinValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMinDigitalValue();
      return 0.0;
   }
  float getMaxValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMaxDigitalValue();
      return 1.0;
   }
};
/* __________________________________________________Function: DIGITAL OUTPUT */
class ChFunDigitalOutput : public ChFunction {
public:
  ChFunDigitalOutput(chUnit_t u): ChFunction(u){}
  ~ChFunDigitalOutput(){}
  uint8_t getDisplayPrecision() override {return 0;}
  void displayDescription(Adafruit_SSD1306 &d) override {
    d.println("Digital Output");
  }
  void  displayValue(Adafruit_SSD1306 &d) override {
    
    if(value == 0.0) {d.print("Off");}
    else {d.print("On");}
  }
  void displayShortDecription(Adafruit_SSD1306 &d) override {
    d.print("DOUT ");
  }
  bool isChangeable() override {return true;}
  float getMinValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMinDigitalValue();
      return 0.0;
   }
  float getMaxValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMaxDigitalValue();
      return 1.0;
   }
};
/* _____________________________________________________Function: DIGITAL LED */
class ChFunDigitalLED : public ChFunction {
public:
  ChFunDigitalLED(chUnit_t u): ChFunction(u){}
  ~ChFunDigitalLED(){}
  void  displayValue(Adafruit_SSD1306 &d) override {
    if(value == 0.0) {d.print("Off");}
    else { d.print("On");}
  }
  uint8_t getDisplayPrecision() override {return 0;}
  void displayDescription(Adafruit_SSD1306 &d) override {
    d.println("Status LED");
  }
  void displayShortDecription(Adafruit_SSD1306 &d) override {
    d.print("LED  ");
  }
  bool isChangeable() override {return true;}
  float getMinValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMinDigitalValue();
      return 0.0;
   }
  float getMaxValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMaxDigitalValue();
      return 1.0;
   }
};
/* _____________________________________________________Function: VOLTAGE ADC */
class ChFunADCVoltage : public ChFunction {
public:
  ChFunADCVoltage(chUnit_t u): ChFunction(u){}
  ~ChFunADCVoltage(){}
  void displayDescription(Adafruit_SSD1306 &d) override {
    d.println("Voltage ADC");
  }
  void displayShortDecription(Adafruit_SSD1306 &d) override {
    d.print("VADC ");
  }
  float getMinValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMinVoltageAdcValue();
      return 0.0;
   }
  float getMaxValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMaxVoltageAdcValue();
      return 1.0;
   }
};
/* _____________________________________________________Function: CURRENT ADC */
class ChFunADCCurrent : public ChFunction {
public:
  ChFunADCCurrent(chUnit_t u): ChFunction(u){}
  ~ChFunADCCurrent(){}
  void displayDescription(Adafruit_SSD1306 &d) override {
    d.println("Current ADC");
  }
  void displayShortDecription(Adafruit_SSD1306 &d) override {
    d.print("CADC ");
  }
  float getMinValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMinCurrentAdcValue();
      return 0.0;
   }
  float getMaxValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMaxCurrentAdcValue();
      return 1.0;
   }
};
/* _____________________________________________________Function: VOLTAGE DAC */
class ChFunDACVoltage : public ChFunction {
public:
  ChFunDACVoltage(chUnit_t u): ChFunction(u){}
  ~ChFunDACVoltage(){}
  void displayDescription(Adafruit_SSD1306 &d) override {
    d.println("Voltage DAC");
  }
  void displayShortDecription(Adafruit_SSD1306 &d) override {
    d.print("VDAC ");
  }
  bool isChangeable() override {return true;}
  float getMinValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMinVoltageDacValue();
      return 0.0;
   }
  float getMaxValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMaxVoltageDacValue();
      return 1.0;
   }
};
/* _____________________________________________________Function: CURRENT DAC */
class ChFunDACCurrent : public ChFunction {
public:
  ChFunDACCurrent(chUnit_t u): ChFunction(u){}
  ~ChFunDACCurrent(){}
  void displayDescription(Adafruit_SSD1306 &d) override {
    d.println("Current DAC");
  }
  
  void displayShortDecription(Adafruit_SSD1306 &d) override {
    d.print("CDAC ");
  }
  bool isChangeable() override {return true;}
  float getMinValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMinCurrentDacValue();
      return 0.0;
   }
  float getMaxValue(DisplayExpansion *p)   override   {
      if(p != nullptr)
          return p->getMaxCurrentDacValue();
      return 1.0;
   }
};
/* _____________________________________________________Function: RTD 2 WIRES */
class ChFunRTD2Wires : public ChFunction {
public:
  ChFunRTD2Wires(chUnit_t u): ChFunction(u){}
  ~ChFunRTD2Wires(){}
  void displayDescription(Adafruit_SSD1306 &d) override {
    d.println("RTD 2 wires");
  }
  void displayShortDecription(Adafruit_SSD1306 &d) override {
    d.print("RTD2 ");
  }
};
/* _____________________________________________________Function: RTD 3 WIRES */
class ChFunRTD3Wires : public ChFunction {
public:
  ChFunRTD3Wires(chUnit_t u): ChFunction(u){}
  ~ChFunRTD3Wires(){}
  void displayDescription(Adafruit_SSD1306 &d) override {
    d.println("RTD 3 wires");
  }
  void displayShortDecription(Adafruit_SSD1306 &d) override {
    d.print("RTD3 ");
  }
};
/* ___________________________________________________Function: PWM FREQUENCY */
class ChFunPWMFreq : public ChFunction {
public:
  ChFunPWMFreq(chUnit_t u): ChFunction(u){}
  ~ChFunPWMFreq(){}
  void displayDescription(Adafruit_SSD1306 &d) override {
     d.println("PWM");
  }
  void displayShortDecription(Adafruit_SSD1306 &d) override {
    d.print("PWM  ");
  }
  bool isChangeable() override {return true;}
  float getMinValue(DisplayExpansion *p)   override   {
      (void)p;
      return 1.0;
   }
  float getMaxValue(DisplayExpansion *p)   override   {
      (void)p;
      return 50000;
   }
};
/* __________________________________________________Function: PWM DUTY CICLE */
class ChFunPWMDuty : public ChFunction {
public:
  ChFunPWMDuty(chUnit_t u): ChFunction(u){}
  ~ChFunPWMDuty(){}
  void displayDescription(Adafruit_SSD1306 &d) override {
    d.println("PWM");
  }
  void displayShortDecription(Adafruit_SSD1306 &d) override {
    d.print("PWM  ");
  }
  bool isChangeable() override {return true;}
  float getMinValue(DisplayExpansion *p)   override   {
      (void)p;
      return 1.0;
   }
  float getMaxValue(DisplayExpansion *p)   override   {
      (void)p;
      return 99.0;
   }
};



/* ########################################################################## */
/*                                CLASSE CHANNEL                              */ 
/* ########################################################################## */  

class Channel {
private:
   bool is_pwm;
   bool is_configurable;
   chFun_t funcs[MAX_FUNCTIONS_PER_CHANNEL];
   chType_t types[MAX_FUNCTIONS_PER_CHANNEL];
   ChFunction  *functions[MAX_FUNCTIONS_PER_CHANNEL];
   
   void clear(uint8_t i) {
      if(i < MAX_FUNCTIONS_PER_CHANNEL) {
         if(functions[i] != nullptr) {
            delete functions[i];
            functions[i] = nullptr;
         }
     }
   }
public:
   Channel() : is_pwm(false), is_configurable(false) {
      for(int i = 0; i < MAX_FUNCTIONS_PER_CHANNEL;i++) {
        functions[i] = nullptr;
      }
   }
   ~Channel(){
      for(int i = 0; i < MAX_FUNCTIONS_PER_CHANNEL;i++) {
          clear(i);
      }
   }

   bool isConfigurable() {return is_configurable; }
   void setConfigurable(bool r) {is_configurable = r;}
   bool isPwm() {return is_pwm;}

   

   /* _____________________________FACTORY METHOD TO CREATE function channels */
   void makeFunction(uint8_t i, chFun_t f, chType_t t, chUnit_t u, DisplayExpansion *p, uint8_t ch) {
      if(p != nullptr) {
        setConfigurable(p->isConfigurable(ch));
      }
      if(i < MAX_FUNCTIONS_PER_CHANNEL) {
         if(f == funcs[i] && t == types[i] && functions[i] != nullptr) {
          return;
         }
         else {
           funcs[i] = f;
           types[i] = t;
           delete functions[i];
           functions[i] = nullptr;
         }
         /*____________________________________________________HIGH IMPEDENCE */
         if(f == CH_FUNCTION_HIGH_IMPEDENCE) {
           functions[i] = new ChFunHighImpedence(u);
         }
         /*___________________________________________________________DIGITAL */
         else if(f == CH_FUNCTION_DIGITAL) {
            if(t == CH_TYPE_INPUT) {
              /*                                                      - Input */
              functions[i] = new ChFunDigitalInput(u);
            }
            else if(t == CH_TYPE_OUTPUT) {
              /*                                                     - Output */
              functions[i] = new ChFunDigitalOutput(u);
            }
            else if(t == CH_TYPE_LED) {
              /*                                                        - LED */
              functions[i] = new ChFunDigitalLED(u);
            }
         }
         /*_______________________________________________________________DAC */
         else if(f == CH_FUNCTION_DAC) {
            if(t == CH_TYPE_VOLTAGE) {
              /*                                                    - Voltage */
              functions[i] = new ChFunDACVoltage(u);
            }
            else if(t == CH_TYPE_CURRENT) {
              /*                                                    - Current */
              functions[i] = new ChFunDACCurrent(u);
            }
         }
         /*_______________________________________________________________ADC */
         else if(f == CH_FUNCTION_ADC) {
            if(t == CH_TYPE_VOLTAGE) {
              /*                                                    - Voltage */
              functions[i] =new ChFunADCVoltage(u);
            }
            else if(t == CH_TYPE_CURRENT) {
              /*                                                    - Current */
              functions[i] =new ChFunADCCurrent(u);
            }
         }
         /*_______________________________________________________________PWM */
         else if(f == CH_FUNCTION_PWM) {
            is_pwm = true;
            if(t == CH_TYPE_FREQ) {
              /*                                                  - Frequency */
              functions[i] =new ChFunPWMFreq(u);
            }
            else if(t == CH_TYPE_DUTY) {
              /*                                                       - Duty */
              functions[i] = new ChFunPWMDuty(u);
            }
         }
         /*_______________________________________________________________RTD */
         else if(f == CH_FUNCTION_RTD) {
            if(t == CH_TYPE_2_WIRES) {
              /*                                                    - 2 Wires */
              functions[i] =new ChFunRTD2Wires(u);
            }
            else if(t == CH_TYPE_3_WIRES) {
              /*                                                    - 3 Wires */
              functions[i] =new ChFunRTD3Wires(u);
            }
         }      
      }
   }

   uint8_t getNumOfFunctions() {
      uint8_t rv = 0;
      for(int i = 0; i < MAX_FUNCTIONS_PER_CHANNEL; i++) {
         if(functions[i] != nullptr) {
            rv++;
         }
      }
      return rv;
   }

   void setValue(uint8_t i, float v) {
      if(i >= MAX_FUNCTIONS_PER_CHANNEL) return;
      
      if(functions[i] != nullptr) {
         functions[i]->setValue(v);
      }
   }

   float getValue(uint8_t i) {
      if(i >= MAX_FUNCTIONS_PER_CHANNEL) return 0.0;
      if(functions[i] != nullptr) {
         return functions[i]->getValue();
      }
      return 0.0;
   }

   void incrementValue(uint8_t i, DisplayExpansion *p) {
      if(i >= MAX_FUNCTIONS_PER_CHANNEL) return;
      if(functions[i] != nullptr) {
         functions[i]->incrementValue(p);
      }
   }

   void incrementBigValue(uint8_t i,DisplayExpansion *p) {
      if(i >= MAX_FUNCTIONS_PER_CHANNEL) return;
      if(functions[i] != nullptr) {
         functions[i]->bigIncrementValue(p);
      }
   }

   void decrementValue(uint8_t i,DisplayExpansion *p) {
      if(i >= MAX_FUNCTIONS_PER_CHANNEL) return;
      if(functions[i] != nullptr) {
         functions[i]->decrementValue(p);
      }
   }

   void decrementBigValue(uint8_t i,DisplayExpansion *p) {
      if(i >= MAX_FUNCTIONS_PER_CHANNEL) return;
      if(functions[i] != nullptr) {
        functions[i]->bigDecrementValue(p);
      }
   }


   void displayShortDecription(Adafruit_SSD1306 &d) {
      if(functions[0] != nullptr) {
         return functions[0]->displayShortDecription(d);
      }
   }

   void displayDescription(Adafruit_SSD1306 &d) {
      if(functions[0] != nullptr) {
         return functions[0]->displayDescription(d);
      }
   }

   void displayValue(uint8_t i, Adafruit_SSD1306 &d) {
      if(i >= MAX_FUNCTIONS_PER_CHANNEL) return;
      if(functions[i] != nullptr) {
         return functions[i]->displayValue(d);
      }
   }

   void displayChannel(Adafruit_SSD1306 &d) {      
      displayShortDecription(d);
      for(int i = 0; i < MAX_FUNCTIONS_PER_CHANNEL; i++) {
        if(funcs[i] != CH_FUNCTION_UNAVAILABLE) {
           if(i > 0) {
              d.print("         ");
           }
           displayValue(i, d);
           d.println();
        }
        else if(i == 0) {
           d.println("waiting info...");
        }
      }
   }

   void displayChannelInfo(Adafruit_SSD1306 &d) {
      for(int i = 0; i < MAX_FUNCTIONS_PER_CHANNEL; i++) {
        if(funcs[i] != CH_FUNCTION_UNAVAILABLE) { 
          if(functions[i] != nullptr) {
             functions[i]->displayDescription(d);
          }
          if(funcs[i] != CH_FUNCTION_HIGH_IMPEDENCE)
            d.print(" -> ");
          displayValue(i,d);
          d.println();
        }
      }
   }

    bool isChangeable() {
      if(functions[0] != nullptr) {
        return functions[0]->isChangeable();
      }
      return false;
    }


    
};




} //namespace r4display


#endif


