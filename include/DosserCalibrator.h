#ifndef HPCC_NUTRIENTCALIBRATOR_H
#define HPCC_NUTRIENTCALIBRATOR_H

#include "Relay.h"
#include "Pump.h"
#include "NutrientDosser.h"

class DosserCalibrator {
 public:
  void dose(Pump &pump);
};

#endif //HPCC_NUTRIENTCALIBRATOR_H
