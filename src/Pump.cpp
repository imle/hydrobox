#include <Pump.h>


Pump::Pump(int pin, int k) : pin(pin), k(k) {
  pinMode(this->pin, OUTPUT);
}

Pump::Pump(PCA9685 *driver, int pin, int k): driver(driver), pin(pin), k(k) {}

Pump::State Pump::on() {
  State state = this->state();
  if (state != State::OFF) {
    return state;
  }

  this->is_on = true;
  if (this->driver != nullptr) {
    this->driver->getPin(this->pin).setValueAndWrite(this->k);
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

  this->on_for_ms = millis() + ms;
  this->on_async = true;
  return this->on();
}

void Pump::off(bool emergency) {
  if (emergency) {
    this->locked_off = true;
  }

  this->on_async = false;
  this->is_on = false;
  if (this->driver != nullptr) {
    this->driver->getPin(this->pin).fullOffAndWrite();
  } else {
    analogWrite(this->pin, 0);
  }
}

bool Pump::checkShouldOff() {
  if (this->state() != State::ON) {
    return false;
  }

  if (!this->on_async) {
    return false;
  }

  if (this->on_for_ms > millis()) {
    return false;
  }

  this->off();

  return true;
}

Pump::State Pump::state() const {
  if (this->locked_off) {
    return State::LOCKED_OFF;
  } else if (this->delay_on > millis()) {
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
  this->delay_on = millis() + ms;
}

void Pump::cancelDelay() {
  this->delay_on = 0;
}
