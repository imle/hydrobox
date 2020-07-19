
#include "Ezo.h"
#include "Arduino.h"
#include "Wire.h"
#include <stdlib.h>


EzoBoard::EzoBoard(uint8_t address) {
  this->i2c_address = address;
}

EzoBoard::EzoBoard(uint8_t address, const char *name) {
  this->i2c_address = address;
  this->name = name;
}

const char *EzoBoard::getName() const {
  return this->name;
}

uint8_t EzoBoard::getAddress() const {
  return i2c_address;
}

void EzoBoard::sendCmd(const char *command) {
  Wire.beginTransmission(this->i2c_address);
  Wire.write(command);
  Wire.endTransmission();
  this->issued_read = false;
  this->request_pending = true;
  this->response_received = false;
}

void EzoBoard::sendReadCmd() {
  this->sendCmd("r");
  this->issued_read = true;
}

void EzoBoard::sendCmdWithNum(const char *cmd, float num, uint8_t decimal_amount) {
  String temp = String(cmd) + String(num, decimal_amount);
  const char *pointer = temp.c_str();
  this->sendCmd(pointer);
}

void EzoBoard::sendReadWithTempComp(float temperature) {
  this->sendCmdWithNum("rt,", temperature, 3);
  this->issued_read = true;
}

enum EzoBoard::Error EzoBoard::receiveReadCmd() {
  char _sensordata[EzoBoard::buffer_len];
  this->error = this->receiveCmd(_sensordata, buffer_len);

  if (this->error == SUCCESS) {
    if (!this->issued_read) {
      this->error = NOT_READ_CMD;
    } else {
      this->reading = atof(_sensordata);
    }
    this->issued_read = false;
  }
  return this->error;
}

bool EzoBoard::isReadPoll() const {
  return this->issued_read;
}

float EzoBoard::getLastReceivedReading() const {
  return this->reading;
}

enum EzoBoard::Error EzoBoard::getError() const {
  return this->error;
}

enum EzoBoard::Error EzoBoard::receiveCmd(char *sensordata_buffer, uint8_t buffer_len) {
  byte sensor_bytes_received = 0;
  byte code;
  byte in_char;

  memset(sensordata_buffer, 0, buffer_len);        // clear sensordata array;

  Wire.requestFrom(this->i2c_address, (size_t) (buffer_len - 1), true);
  code = Wire.read();

  //Wire.beginTransmission(this->i2c_address);
  while (Wire.available()) {
    in_char = Wire.read();

    if (in_char == 0) {
      //Wire.endTransmission();
      break;
    } else {
      sensordata_buffer[sensor_bytes_received] = in_char;
      sensor_bytes_received++;
    }
  }

  //should last array point be set to 0 to stop string overflows?
  switch (code) {
    case 1:
      this->error = SUCCESS;
      break;

    case 2:
      this->error = FAIL;
      break;

    case 254:
      this->error = NOT_READY;
      break;

    case 255:
      this->error = NO_DATA;
      break;
  }

  this->response_received = true;

  if (this->error != NOT_READY) {
    this->request_pending = false;
    this->response_received = true;
  }

  return this->error;
}

bool EzoBoard::isRequestPending() const {
  return this->request_pending;
}

bool EzoBoard::isResponseReceived() const {
  return this->response_received;
}
