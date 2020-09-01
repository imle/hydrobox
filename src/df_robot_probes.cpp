#include <df_robot_probes.h>
#include <OneWire.h>
#include <pin_assignment.h>
#include <debug.h>


OneWire ds(PIN_DS18S20);

// temperature in DEG Celsius
float getReservoirTemp() {
  FUNC_IN
  byte data[12];
  byte addr[8];

  if (!ds.search(addr)) {
    //no more sensors on chain, reset search
    ds.reset_search();
    FUNC_OUT
    return -1000;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("DS18S20 CRC is not valid!");
    FUNC_OUT
    return -1000;
  }

  if (addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("DS18S20 is not recognized");
    FUNC_OUT
    return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion, with parasite power on at the end

  ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = MSB << 8 | LSB; // using two's compliment
  float TemperatureSum = tempRead / 16;

  FUNC_OUT
  return TemperatureSum;
}
