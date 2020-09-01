#include <Relay.h>
#include <debug.h>


Relay::Relay(int pin, int off_state) : pin(pin), off_state(off_state) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, this->off_state);
}

void Relay::on() {
  FUNC_IN
  this->is_on = true;
  digitalWrite(this->pin, this->off_state == LOW ? HIGH : LOW);
  FUNC_OUT
}

void Relay::off() {
  FUNC_IN
  this->is_on = false;
  digitalWrite(this->pin, this->off_state == LOW ? LOW : HIGH);
  FUNC_OUT
}

bool Relay::isOn() const {
  return this->is_on;
}
