#include <button_actions.h>
#include <dossing.h>


Pump *button_pump = nullptr;
bool thing_on = false;

Debouncer button_debouncer(BUTTON_PIN, MODE_OPEN_ON_PUSH, buttonPressed, buttonReleased);

Relay *relays[] = {&submersible_pump, &plant_lights};

void buttonPressed() {
#if defined(BUTTON_HOLD_ON_RELAYS)
  for (auto & relay : relays) {
    relay->on();
  }
#elif defined(BUTTON_HOLD_ON_PUMPS)
  if (button_pump != nullptr) {
    if (!button_pump->isOn()) {
      button_pump->on();
    }
  }
#elif defined(BUTTON_TOGGLE_ON_BUTTON_PUMP)
  if (button_pump->isOn()) {
    button_pump->off();
  } else {
    button_pump->on();
  }
#elif defined(BUTTON_TOGGLE_ON)
  thing_on = !thing_on;
#elif defined(BUTTON_TOGGLE_CALIBRATOR)
  if (!is_dosing) {
    is_dosing = true;

    const double WaterVolumeInMl = 15160.0;
    const double DosePerGallon = 7.5;
    double dose_amount, dose_time;

    // This should yield almost exactly 30mL
    dose_amount = WaterVolumeInMl * DosePerGallon / MillilitersPerUSGallon;
    dose_time = dose_amount / MilliliterToMilliseconds;

    Serial.print(dose_amount, 0);
    Serial.println(" mL");
    Serial.println(dose_time, 8);

    button_pump->on((unsigned long) (round(dose_time)));
  }
#elif defined(BUTTON_START_DOSE)
  if (!is_dosing) {
    is_dosing = true;

    th_start_mix_nutrients.startDelayed();
  }
#endif
}

void buttonReleased(unsigned long pressTimespan) {
#if defined(BUTTON_HOLD_ON_RELAYS)
  for (auto & relay : relays) {
    relay->off();
  }
#elif defined(BUTTON_HOLD_ON_PUMPS)
  if (button_pump != nullptr) {
    if (button_pump->isOn()) {
      button_pump->off();
    }
  }
#elif defined(BUTTON_TOGGLE_ON_BUTTON_PUMP)
  // no action
#elif defined(BUTTON_TOGGLE_ON)
  // no action
#elif defined(BUTTON_TOGGLE_CALIBRATOR)
  // no action
#elif defined(BUTTON_START_DOSE)
  // no action
#endif
}