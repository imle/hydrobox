#include <Pump.h>
#include <debug.h>


Pump::Pump(int pin, int k) : pin(pin), k(k) {
  pinMode(this->pin, OUTPUT);
}

Pump::Pump(PCA9685 *driver, int pin, int k): driver(driver), pin(pin), k(k) {}

void Pump::on() {
  FUNC_IN
  this->is_on = true;
  if (this->driver != nullptr) {
    this->driver->getPin(this->pin).setValueAndWrite(this->k);
  } else {
    analogWrite(this->pin, (uint8_t) this->k);
  }
  FUNC_OUT
}

void Pump::off() {
  FUNC_IN
  this->is_on = false;
  if (this->driver != nullptr) {
    this->driver->getPin(this->pin).fullOffAndWrite();
  } else {
    analogWrite(this->pin, 0);
  }
  FUNC_OUT
}

bool Pump::isOn() const {
  return this->is_on;
}
