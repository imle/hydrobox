#ifndef HPCC_NUTRIENTDOSSER_H
#define HPCC_NUTRIENTDOSSER_H

#include "Relay.h"
#include "Pump.h"
#include "FeedChart.h"


const unsigned long MixTimeMS = 7000;
const unsigned long SettleTimeMS = 2000;
const double MilliliterToMilliseconds = 90.0 / (60.0 * 1000.0);

#define DEBUG_NUTRIENT_DOSSER

enum PHDirection {
  UP = 0,
  DOWN = 1,
};

class NutrientDosser {
 public:
  NutrientDosser(
      const FeedChart *feed_chart,
      Pump &flora_micro,
      Pump &flora_gro,
      Pump &flora_bloom,
      Pump &ph_changer,
      PHDirection ph_direction,
      Relay **mixers,
      int mixer_count
  );

 protected:
  const FeedChart *const feed_chart;
  Pump &pf_micro;
  Pump &pf_gro;
  Pump &pf_bloom;
  Pump &pump_ph_changer;
  PHDirection ph_direction;
  Relay **mixers;
  int mixer_count;

  void mix(unsigned long ms);

  void dose(double water_volume_in_ml, int regimen);

 public:
  void doseRegimen(double water_volume_in_ml, int regimen);
};

#endif //HPCC_NUTRIENTDOSSER_H
