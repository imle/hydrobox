#include "Relay.h"


Relay::Relay(pin_size_t pin, PinStatus off_state) : pin(pin), off_state(off_state) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, this->off_state);
}

void Relay::on() {
  this->is_on = true;
  digitalWrite(this->pin, this->off_state == LOW ? HIGH : LOW);
}

bool Relay::on(unsigned long ms) {
  if (this->delayOn > millis()) {
    return false;
  }

  this->onForMS = millis() + ms;
  this->on();

  return true;
}

void Relay::off() {
  this->is_on = false;
  digitalWrite(this->pin, this->off_state == LOW ? LOW : HIGH);
}

void Relay::onBlocking(unsigned long ms) {
  this->onForMS = millis() + ms;
  this->on();

  while (this->isOn()) {
    this->checkShouldOff();
    delay(10);
  }
}

bool Relay::checkShouldOff() {
  if (!this->isOn()) {
    return false;
  }

  if (this->onForMS > millis()) {
    return false;
  }

  this->off();

  return true;
}

bool Relay::isOn() const {
  return this->is_on;
}

void Relay::delayNextOn(unsigned long ms) {
  this->delayOn = millis() + ms;
}

void Relay::cancelDelay() {
  this->delayOn = 0;
}
