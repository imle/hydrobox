#include "../include/DosserCalibrator.h"


void DosserCalibrator::dose(Pump &pump) {
  const double WaterVolumeInMl = 15160.0;
  const double DosePerGallon = 7.5;
  double dose_amount, dose_time;

  // This should yield almost exactly 30mL
  dose_amount = WaterVolumeInMl * DosePerGallon / MillilitersPerUSGallon;
  dose_time = dose_amount / MilliliterToMilliseconds;

  Serial.print(dose_amount, 0);
  Serial.println(" mL");
  Serial.println(dose_time, 8);

  pump.on((unsigned long) (round(dose_time)));

  while (pump.isOn()) {
    pump.checkShouldOff();
  }
}
