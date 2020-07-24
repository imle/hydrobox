#include <NutrientDosser.h>


NutrientDosser::NutrientDosser(
    const FeedChart *feed_chart,
    Pump &flora_micro,
    Pump &flora_gro,
    Pump &flora_bloom,
    Pump &ph_changer,
    PHDirection ph_direction,
    Relay **mixers,
    int mixer_count
) : feed_chart(feed_chart),
    pf_micro(flora_micro),
    pf_gro(flora_gro),
    pf_bloom(flora_bloom),
    pump_ph_changer(ph_changer),
    ph_direction(ph_direction),
    mixers(mixers),
    mixer_count(mixer_count) {}

void NutrientDosser::doseRegimen(double water_volume_in_ml, int regimen) {
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
  this->pf_micro.on((unsigned long) (round(dose_time)));

  dose_amount = water_volume_in_ml * this->feed_chart->regimens[regimen].flora_gro;
  dose_time = dose_amount / MilliliterToMilliseconds;
#ifdef DEBUG_NUTRIENT_DOSSER
  Serial.println(dose_amount, 8);
  Serial.println(dose_time, 8);
#endif
  this->pf_gro.on((unsigned long) (round(dose_time)));

  dose_amount = water_volume_in_ml * this->feed_chart->regimens[regimen].flora_bloom;
  dose_time = dose_amount / MilliliterToMilliseconds;
#ifdef DEBUG_NUTRIENT_DOSSER
  Serial.println(dose_amount, 8);
  Serial.println(dose_time, 8);
#endif
  this->pf_bloom.on((unsigned long) (round(dose_time)));

  while (this->pf_micro.isOn() || this->pf_gro.isOn() || this->pf_bloom.isOn()) {
    this->pf_micro.checkShouldOff();
    this->pf_gro.checkShouldOff();
    this->pf_bloom.checkShouldOff();
  }
}
