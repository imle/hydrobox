#include <Relay.h>


Relay::Relay(int pin, int off_state) : pin(pin), off_state(off_state) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, this->off_state);
}

void Relay::on() {
  this->is_on = true;
  digitalWrite(this->pin, this->off_state == LOW ? HIGH : LOW);
}

bool Relay::on(unsigned long ms) {
  if (this->delay_on > millis()) {
    return false;
  }

  this->on_for_ms = millis() + ms;
  this->on_async = false;
  this->on();

  return true;
}

void Relay::off() {
  this->on_async = false;
  this->is_on = false;
  digitalWrite(this->pin, this->off_state == LOW ? LOW : HIGH);
}

bool Relay::checkShouldOff() {
  if (!this->isOn()) {
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

bool Relay::isOn() const {
  return this->is_on;
}

void Relay::delayNextOn(unsigned long ms) {
  this->delay_on = millis() + ms;
}

void Relay::cancelDelay() {
  this->delay_on = 0;
}
