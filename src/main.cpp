#include <Arduino.h>
#include <SPI.h>
#include <Pump.h>
#include <Relay.h>
#include <NutrientDosser.h>
#include <FeedChart.h>


Pump pump_flora_micro(11, KAddressEEPROMDefault + 8 * 0);    // Pump top
Pump pump_flora_gro(10, KAddressEEPROMDefault + 8 * 1);      //
Pump pump_flora_bloom(9, KAddressEEPROMDefault + 8 * 2);     //
Pump pump_ph_up_reservoir(6, KAddressEEPROMDefault + 8 * 3); //
Pump pump_ph_up_basin(5, KAddressEEPROMDefault + 8 * 4);     //
Pump pump_ph_down_basin(3, KAddressEEPROMDefault + 8 * 5);   // Pump bottom

Relay r0(1, HIGH);
Relay r1(0, HIGH);
Relay r2(A0, HIGH);
Relay r3(A1, HIGH);
Relay r4(A2, HIGH);
Relay r5(A3, HIGH);
Relay r6(A4, HIGH);
Relay r7(A5, HIGH);

Relay fan0(12, LOW);
Relay fan1(13, LOW);

Relay *mixers[] = {&fan0, &fan1};

NutrientDosser dosser(&DefaultFeedChart, &pump_flora_gro, &pump_flora_bloom, &pump_flora_micro, &pump_ph_up_reservoir, UP, mixers, 2);

volatile bool prime = false;

void primePumps() {
  Serial.println(1);
  prime = true;
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  pinMode(2, INPUT_PULLDOWN);
//  attachInterrupt(digitalPinToInterrupt(2), primePumps, RISING);

  delay(2000);
  dosser.doseRegimen(10000, 0); // TODO: Run test
}

void loop() {
  if (digitalRead(2) == HIGH) {
    pump_flora_micro.on();
  } else {
    pump_flora_micro.off();
  }

  delay(20);

//  Serial.println("r0 on");
//  digitalWrite(1, LOW);
//  delay(1000);
//  digitalWrite(1, HIGH);
//  Serial.println("r0 off");
//  delay(1000);
//
//  Serial.println("r1 on");
//  digitalWrite(0, LOW);
//  delay(1000);
//  digitalWrite(0, HIGH);
//  Serial.println("r1 off");
//  delay(1000);
//
//  Serial.println("r2 on");
//  digitalWrite(A0, LOW);
//  delay(1000);
//  digitalWrite(A0, HIGH);
//  Serial.println("r2 off");
//  delay(1000);
//
//  Serial.println("r3 on");
//  digitalWrite(A1, LOW);
//  delay(1000);
//  digitalWrite(A1, HIGH);
//  Serial.println("r3 off");
//  delay(1000);
//
//  Serial.println("r4 on");
//  digitalWrite(A2, LOW);
//  delay(1000);
//  digitalWrite(A2, HIGH);
//  Serial.println("r4 off");
//  delay(1000);
//
//  Serial.println("r5 on");
//  digitalWrite(A3, LOW);
//  delay(1000);
//  digitalWrite(A3, HIGH);
//  Serial.println("r5 off");
//  delay(1000);
//
//  Serial.println("r6 on");
//  digitalWrite(A4, LOW);
//  delay(1000);
//  digitalWrite(A4, HIGH);
//  Serial.println("r6 off");
//  delay(1000);
//
//  Serial.println("r7 on");
//  digitalWrite(A5, LOW);
//  delay(1000);
//  digitalWrite(A5, HIGH);
//  Serial.println("r7 off");
//  delay(1000);




//  Serial.println("p0 on");
//  p0.on();
//  delay(1000);
//  p0.off();
//  Serial.println("p0 off");
//  delay(100);
//
//  Serial.println("p1 on");
//  p1.on();
//  delay(1000);
//  p1.off();
//  Serial.println("p1 off");
//  delay(100);
//
//  Serial.println("p2 on");
//  p2.on();
//  delay(1000);
//  p2.off();
//  Serial.println("p2 off");
//  delay(100);
//
//  Serial.println("p3 on");
//  p3.on();
//  delay(1000);
//  p3.off();
//  Serial.println("p3 off");
//  delay(100);
//
//  Serial.println("p4 on");
//  p4.on();
//  delay(1000);
//  p4.off();
//  Serial.println("p4 off");
//  delay(100);
//
//  Serial.println("p5 on");
//  p5.on();
//  delay(1000);
//  p5.off();
//  Serial.println("p5 off");
//  delay(100);
}
