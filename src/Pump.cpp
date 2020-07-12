#include "Pump.h"
#include <EEPROM.h>


Pump::Pump(pin_size_t pin) : pin(pin), k_address_eeprom(KAddressEEPROMDefault) {
  pinMode(pin, OUTPUT);

  EEPROM.get(this->k_address_eeprom, this->k);
}

Pump::Pump(pin_size_t pin, int k_address_eeprom) : pin(pin), k_address_eeprom(k_address_eeprom) {
  pinMode(pin, OUTPUT);

  EEPROM.get(this->k_address_eeprom, this->k);
}

void Pump::on() {
  this->is_on = true;
  Serial.println(this->k);
  analogWrite(this->pin, this->k);
}

bool Pump::on(unsigned long ms) {
  if (this->delayOn > millis()) {
    return false;
  }

  this->onForMS = millis() + ms;
  this->on();

  return true;
}

void Pump::off() {
  this->is_on = false;
  analogWrite(this->pin, 0);
}

void Pump::onBlocking(unsigned long ms) {
  this->onForMS = millis() + ms;
  this->on();

  while (this->isOn()) {
    this->checkShouldOff();
    delay(10);
  }
}

bool Pump::checkShouldOff() {
  if (!this->isOn()) {
    return false;
  }

  if (this->onForMS > millis()) {
    return false;
  }

  this->off();

  return true;
}

bool Pump::isOn() const {
  return this->is_on;
}

void Pump::delayNextOn(unsigned long ms) {
  this->delayOn = millis() + ms;
}

void Pump::cancelDelay() {
  this->delayOn = 0;
}

void Pump::beginCalibration() {
  if (this->calibrating) {
    return;
  }

  this->calibrating = true;

  this->on();
  delay(1000);
  this->off();
}

void Pump::setCalibrationKValue(byte k) {
  this->k = k;
  EEPROM.put(this->k_address_eeprom, (byte)this->k);
}

void Pump::endCalibration() {
  this->calibrating = false;
}
