/***************************************************
 DFRobot Gravity: Analog TDS Sensor/Meter
 <https://www.dfrobot.com/wiki/index.php/Gravity:_Analog_TDS_Sensor_/_Meter_For_Arduino_SKU:_SEN0244>
 
 ***************************************************
 This sample code shows how to read the tds value and calibrate it with the standard buffer solution.
 707ppm(1413us/cm)@25^c standard buffer solution is recommended.
 
 Created 2018-1-3
 By Jason <jason.ling@dfrobot.com@dfrobot.com>
 
 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution.
 ****************************************************/

#include <EEPROM.h>
#include "GravityTDS.h"


//float EEkValue = 1.0;

#define EEPROM_write(address, p) {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) EEPROM.write(address+i, pp[i]);}
#define EEPROM_read(address, p)  {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) pp[i]=EEPROM.read(address+i);}
//#define EEPROM_write(address, p) {EEkValue = p;}
//#define EEPROM_read(address, p)  {p = EEkValue;}

GravityTDS::GravityTDS() {
  this->pin = A1;
  this->temperature = 25.0;
  this->aref = 5.0;
  this->adcRange = 1024.0;
  this->kValueAddress = 0x06;
  this->kValue = 1.0;
}

GravityTDS::GravityTDS(int pin) {
  this->pin = pin;
  this->temperature = 25.0;
  this->aref = 5.0;
  this->adcRange = 1024.0;
  this->kValueAddress = 0x06;
  this->kValue = 1.0;
}

GravityTDS::~GravityTDS() = default;

void GravityTDS::setPin(int pin) {
  this->pin = pin;
}

void GravityTDS::setTemperature(float temp) {
  this->temperature = temp;
}

void GravityTDS::setAref(float value) {
  this->aref = value;
}

void GravityTDS::setAdcRange(float range) {
  this->adcRange = range;
}

void GravityTDS::setKvalueAddress(int address) {
  this->kValueAddress = address;
}

void GravityTDS::begin() {
  pinMode(this->pin, INPUT);
  readKValues();
}

float GravityTDS::getKvalue() const {
  return this->kValue;
}

void GravityTDS::update() {
  this->analogValue = analogRead(this->pin);
  this->voltage = this->analogValue / this->adcRange * this->aref;
  float v = this->voltage * this->voltage;
  this->ecValue = (133.42 * this->voltage * v - 255.86 * v + 857.39 * this->voltage) * this->kValue;
  this->ecValue25 = this->ecValue / (1.0 + 0.02 * (this->temperature - 25.0));  //temperature compensation
  this->tdsValue = this->ecValue25 * TdsFactor;
  if (cmdSerialDataAvailable() > 0) {
    ecCalibration(cmdParse());  // if received serial cmd from the serial monitor, enter into the calibration mode
  }
}

float GravityTDS::getTdsValue() const {
  return tdsValue;
}

float GravityTDS::getEcValue() const {
  return ecValue25;
}

void GravityTDS::readKValues() {
  EEPROM_read(this->kValueAddress, this->kValue);
  if (*(int64_t*)(&this->kValue) == 0xFFFFFFFF) {
    this->kValue = 1.0;   // default value: K = 1.0
    EEPROM_write(this->kValueAddress, this->kValue);
  }
}

boolean GravityTDS::cmdSerialDataAvailable() {
  char cmdReceivedChar;
  static unsigned long cmdReceivedTimeOut = millis();
  while (Serial.available() > 0) {
    if (millis() - cmdReceivedTimeOut > 500U) {
      cmdReceivedBufferIndex = 0;
      memset(cmdReceivedBuffer, 0, (ReceivedBufferLength + 1));
    }
    cmdReceivedTimeOut = millis();
    cmdReceivedChar = Serial.read();
    if (cmdReceivedChar == '\n' || cmdReceivedBufferIndex == ReceivedBufferLength) {
      cmdReceivedBufferIndex = 0;
      strupr(cmdReceivedBuffer);
      return true;
    } else {
      cmdReceivedBuffer[cmdReceivedBufferIndex] = cmdReceivedChar;
      cmdReceivedBufferIndex++;
    }
  }
  return false;
}

byte GravityTDS::cmdParse() {
  byte modeIndex = 0;
  if (strstr(cmdReceivedBuffer, "ENTER") != nullptr)
    modeIndex = 1;
  else if (strstr(cmdReceivedBuffer, "EXIT") != nullptr)
    modeIndex = 3;
  else if (strstr(cmdReceivedBuffer, "CAL:") != nullptr)
    modeIndex = 2;
  return modeIndex;
}

void GravityTDS::ecCalibration(byte mode) {
  char *cmdReceivedBufferPtr;
  static boolean ecCalibrationFinish = false;
  static boolean enterCalibrationFlag = false;
  float KValueTemp, rawECsolution;
  switch (mode) {
    case 0:
      if (enterCalibrationFlag)
        Serial.println(F("Command Error"));
      break;

    case 1:
      enterCalibrationFlag = true;
      ecCalibrationFinish = false;
      Serial.println();
      Serial.println(F(">>>Enter Calibration Mode<<<"));
      Serial.println(F(">>>Please put the probe into the standard buffer solution<<<"));
      Serial.println();
      break;

    case 2:
      cmdReceivedBufferPtr = strstr(cmdReceivedBuffer, "CAL:");
      cmdReceivedBufferPtr += strlen("CAL:");
      rawECsolution = strtod(cmdReceivedBufferPtr, nullptr) / (float) (TdsFactor);
      rawECsolution = rawECsolution * (1.0 + 0.02 * (temperature - 25.0));
      if (enterCalibrationFlag) {
        float v = this->voltage * this->voltage;
        KValueTemp = rawECsolution / (133.42 * voltage * v - 255.86 * v + 857.39 * voltage);  //calibrate in the  buffer solution, such as 707ppm(1413us/cm)@25^c
        if ((rawECsolution > 0) && (rawECsolution < 2000) && (KValueTemp > 0.25) && (KValueTemp < 4.0)) {
          Serial.println();
          Serial.print(F(">>>Confrim Successful,K:"));
          Serial.print(KValueTemp);
          Serial.println(F(", Send EXIT to Save and Exit<<<"));
          kValue = KValueTemp;
          ecCalibrationFinish = 1;
        } else {
          Serial.println();
          Serial.println(F(">>>Confirm Failed,Try Again<<<"));
          Serial.println();
          ecCalibrationFinish = 0;
        }
      }
      break;

    case 3:
      if (enterCalibrationFlag) {
        Serial.println();
        if (ecCalibrationFinish) {
          EEPROM_write(kValueAddress, kValue);
          Serial.print(F(">>>Calibration Successful,K Value Saved"));
        } else
          Serial.print(F(">>>Calibration Failed"));
        Serial.println(F(",Exit Calibration Mode<<<"));
        Serial.println();
        ecCalibrationFinish = 0;
        enterCalibrationFlag = 0;
      }
      break;
  }
}
