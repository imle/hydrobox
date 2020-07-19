# Ezo_I2c_lib
Library for using Atlas Scientific EZO devices in I2C mode

## Instructions
To use the library with Arduino, follow [this link](https://www.arduino.cc/en/Guide/Libraries).

For instructions on how to set up the devices in I2C mode, see [this instructable](https://www.instructables.com/id/UART-AND-I2C-MODE-SWITCHING-FOR-ATLAS-SCIENTIFIC-E/)

See our [instructables page](https://www.instructables.com/member/AtlasScientific/) to learn how to assemble the hardware for some of the examples

## Documentation
```C++
//Error
enum Error {SUCCESS, FAIL, NOT_READY, NO_DATA, NOT_READ_CMD};

//constructors
EzoBoard(uint8_t address);	 //Takes I2C address of the device
EzoBoard(uint8_t address, const char* name); 
//Takes I2C address of the device
//as well a name of your choice

void sendCmd(const char* command);	
//send any command in a string, see the devices datasheet for available i2c commands

void sendReadCmd();	
//sends the "R" command to the device and sets issued_read() to true, 
//so we know to parse the data when we receive it with receive_read()

void sendCmdWithNum(const char* cmd, float num, uint8_t decimal_amount = 3);
//sends any command with the number appended as a string afterwards.
//ex. PH.sendCmdWithNum("T,", 25.0); will send "T,25.000"

void sendReadWithTempComp(float temperature);
//sends the "RT" command with the temperature converted to a string
//to the device and sets issued_read() to true, 
//so we know to parse the data when we receive it with receive_read()

enum Error receiveCmd(char* sensordata_buffer, uint8_t buffer_len); 
//receive the devices response data into a buffer you supply.
//Buffer should be long enough to hold the longest command 
//you'll receive. We recommand 32 bytes/chars as a default

enum Error receiveReadCmd(); 
//gets the read response from the device, and parses it into the reading variable
//if send_read() wasn't used to send the "R" command and issued_read() isnt set, the function will 
//return the "NOT_READ_CMD" error

bool isReadPoll();		
//function to see if issued_read() was set. 
//Useful for determining if we should call receive_read() (if isReadPoll() returns true) 
//or recieve_cmd() if isReadPoll() returns false) 

float getLastReceivedReading();		
//returns the last reading the device received as a float

const char* getName();		
//returns a pointer to the name string

uint8_t getAddress();
//returns the address of the device

enum Error getError();	
//returns the error status of the last received command, 
//used to check the validity of the data returned by get_reading()
```
