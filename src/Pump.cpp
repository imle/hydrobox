#include "Pump.h"
#include <EEPROM.h>


Pump::Pump(pin_size_t pin) : pin(pin), k_address_eeprom(KAddressEEPROMDefault) {
  pinMode(this->pin, OUTPUT);

  uint8_t val;
  EEPROM.get(this->k_address_eeprom, val);
  this->k = val;
}

Pump::Pump(pin_size_t pin, int k_address_eeprom) : pin(pin), k_address_eeprom(k_address_eeprom) {
  pinMode(this->pin, OUTPUT);

  uint8_t val;
  EEPROM.get(this->k_address_eeprom, val);
  this->k = val;
}

Pump::Pump(Adafruit_PWMServoDriver *driver, pin_size_t pin)
    : driver(driver), pin(pin), k_address_eeprom(KAddressEEPROMDefault) {
  this->driver->setPin(this->pin, 0);

  EEPROM.get(this->k_address_eeprom, this->k);
}

Pump::Pump(Adafruit_PWMServoDriver *driver, pin_size_t pin, int k_address_eeprom)
    : driver(driver), pin(pin), k_address_eeprom(k_address_eeprom) {
  this->driver->setPin(this->pin, 0);

  EEPROM.get(this->k_address_eeprom, this->k);
}

Pump::State Pump::on() {
  State state = this->state();
  if (state != State::OFF) {
    return state;
  }

  Serial.println(this->k);

  this->is_on = true;
  if (this->driver != nullptr) {
    Serial.println("driver");
    this->driver->setPin(this->pin, this->k);
  } else {
    analogWrite(this->pin, (uint8_t) this->k);
  }

  return State::TOGGLED_ON;
}

Pump::State Pump::on(unsigned long ms) {
  State state = this->state();
  if (state != State::OFF) {
    return state;
  }

  this->onForMS = millis() + ms;
  return this->on();
}

void Pump::off(bool emergency) {
  if (emergency) {
    this->locked_off = true;
  }

  this->is_on = false;
  if (this->driver != nullptr) {
    this->driver->setPin(this->pin, 0);
  } else {
    analogWrite(this->pin, 0);
  }
}

bool Pump::onBlocking(unsigned long ms) {
  State state = this->state();
  if (state != State::OFF) {
    return false;
  }

  this->onForMS = millis() + ms;
  this->on();

  while (this->state()) {
    this->checkShouldOff();
    delay(10);
  }

  return true;
}

bool Pump::checkShouldOff() {
  if (this->state() != State::ON) {
    return false;
  }

  if (this->onForMS > millis()) {
    return false;
  }

  this->off();

  return true;
}

Pump::State Pump::state() const {
  if (this->locked_off) {
    return State::LOCKED_OFF;
  } else if (this->delayOn > millis()) {
    return State::DELAYED_OFF;
  } else if (this->is_on) {
    return State::ON;
  } else {
    return State::OFF;
  }
}

bool Pump::isOn() const {
  return this->state() == State::ON;
}

void Pump::delayNextOn(unsigned long ms) {
  this->delayOn = millis() + ms;
}

void Pump::cancelDelay() {
  this->delayOn = 0;
}

void Pump::beginCalibration() {
  if (this->calibrating || this->locked_off) {
    return;
  }

  this->calibrating = true;

  this->on();
  delay(1000);
  this->off();
}

void Pump::setCalibrationKValue(uint16_t k) {
  this->k = k;
  if (this->driver != nullptr) {
    EEPROM.put(this->k_address_eeprom, this->k);
  } else {
    EEPROM.put(this->k_address_eeprom, (uint8_t)this->k);
  }
}

void Pump::endCalibration() {
  this->calibrating = false;
}
