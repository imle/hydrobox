#include "../include/NutrientDosser.h"


NutrientDosser::NutrientDosser(
    const FeedChart *feed_chart,
    Pump *flora_micro,
    Pump *flora_gro,
    Pump *flora_bloom,
    Pump *ph_changer,
    PHDirection ph_direction,
    Relay **mixers,
    int mixer_count
) : feed_chart(feed_chart),
    pump_flora_micro(flora_micro),
    pump_flora_gro(flora_gro),
    pump_flora_bloom(flora_bloom),
    pump_ph_changer(ph_changer),
    ph_direction(ph_direction),
    mixers(mixers),
    mixer_count(mixer_count) {}

void NutrientDosser::doseRegimen(double water_volume_in_ml, int regimen) {
  this->mix(MixTimeMS);

  delay(SettleTimeMS); // Let it settle a small amount

  this->dose(water_volume_in_ml, regimen);
}

void NutrientDosser::mix(unsigned long ms) {
  bool *done_mixing = new bool[this->mixer_count];

  for (int i = 0; i < this->mixer_count; ++i) {
    this->mixers[i]->on(ms);
    done_mixing[i] = false;
  }

  bool all_done;

  do {
    all_done = true;

    for (int i = 0; i < this->mixer_count; ++i) {
      if (this->mixers[i]->isOn()) {
        done_mixing[i] = this->mixers[i]->checkShouldOff();
      }

      all_done = all_done && done_mixing[i];
    }
    delay(10);
  } while (!all_done);

  delete[] done_mixing;
}

void NutrientDosser::dose(double water_volume_in_ml, int regimen) {
  double dose_amount, dose_time;

  dose_amount = water_volume_in_ml * this->feed_chart->regimens[regimen].flora_micro;
  dose_time = dose_amount / MilliliterToMilliseconds;
#ifdef DEBUG_NUTRIENT_DOSSER
  Serial.println(dose_amount, 8);
  Serial.println(dose_time, 8);
#endif
  this->pump_flora_micro->on((unsigned long)(round(dose_time)));

  dose_amount = water_volume_in_ml * this->feed_chart->regimens[regimen].flora_gro;
  dose_time = dose_amount / MilliliterToMilliseconds;
#ifdef DEBUG_NUTRIENT_DOSSER
  Serial.println(dose_amount, 8);
  Serial.println(dose_time, 8);
#endif
  this->pump_flora_gro->on((unsigned long)(round(dose_time)));

  dose_amount = water_volume_in_ml * this->feed_chart->regimens[regimen].flora_bloom;
  dose_time = dose_amount / MilliliterToMilliseconds;
#ifdef DEBUG_NUTRIENT_DOSSER
  Serial.println(dose_amount, 8);
  Serial.println(dose_time, 8);
#endif
  this->pump_flora_bloom->on((unsigned long)(round(dose_time)));

  while (this->pump_flora_micro->isOn() || this->pump_flora_gro->isOn() || this->pump_flora_bloom->isOn()) {
    this->pump_flora_micro->checkShouldOff();
    this->pump_flora_gro->checkShouldOff();
    this->pump_flora_bloom->checkShouldOff();
  }
}
