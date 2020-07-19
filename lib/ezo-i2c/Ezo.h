/*
MIT License

Copyright (c) 2019 Atlas Scientific

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
*/

#ifndef EZO_I2C_H
#define EZO_I2C_H

#include "Arduino.h"

#include <Wire.h>


class EzoBoard {
 public:

  //Error
  enum Error { SUCCESS, FAIL, NOT_READY, NO_DATA, NOT_READ_CMD };

  //constructors
  EzoBoard(uint8_t address);     //Takes I2C address of the device
  EzoBoard(uint8_t address, const char *name); //Takes I2C address of the device
  //as well a name of your choice

  void sendCmd(const char *command);
  //send any command in a string, see the devices datasheet for available i2c commands

  void sendReadCmd();
  //sends the "R" command to the device and sets issued_read() to true,
  //so we know to parse the data when we receive it with receive_read()

  void sendCmdWithNum(const char *cmd, float num, uint8_t decimal_amount = 3);
  //sends any command with the number appended as a string afterwards.
  //ex. PH.sendCmdWithNum("T,", 25.0); will send "T,25.000"

  void sendReadWithTempComp(float temperature);
  //sends the "RT" command with the temperature converted to a string
  //to the device and sets issued_read() to true,
  //so we know to parse the data when we receive it with receive_read()

  enum Error receiveCmd(char *sensordata_buffer, uint8_t buffer_len);
  //receive the devices response data into a buffer you supply.
  //Buffer should be long enough to hold the longest command
  //you'll receive. We recommend 32 bytes/chars as a default

  enum Error receiveReadCmd();
  //gets the read response from the device, and parses it into the reading variable
  //if send_read() wasn't used to send the "R" command and issued_read() isn't set, the function will
  //return the "NOT_READ_CMD" error

  bool isReadPoll() const;
  //function to see if issued_read() was set.
  //Useful for determining if we should call receive_read() (if isReadPoll() returns true)
  //or receiveCmd() if isReadPoll() returns false)

  float getLastReceivedReading() const;
  //returns the last reading the device received as a float

  const char *getName() const;
  //returns a pointer to the name string

  enum Error getError() const;
  //returns the error status of the last received command,
  //used to check the validity of the data returned by get_reading()

  uint8_t getAddress() const;
  //returns the address of the device

  bool isRequestPending() const;

  bool isResponseReceived() const;

 protected:
  uint8_t i2c_address;
  const char *name = nullptr;
  float reading = 0;
  bool issued_read = false;
  bool request_pending = false;
  bool response_received = false;
  enum Error error{NOT_READY};
  const static uint8_t buffer_len = 32;
};

#endif
