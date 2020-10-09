#include "Decoupler.h"

const int pin = 10;

const int refresh_interval = 3333;
const int pulse_lower = 750;
const int pulse_upper = 2250;
const int max_degrees = 145;

const int openPos = 30;
const int closePos = 120;

Decoupler decoupler(refresh_interval, pulse_lower, pulse_upper, 0, max_degrees);

void setup() {
  decoupler.link(pin, openPos, closePos);
  Serial.println("SetupCheckpoint");
}

void loop() {
  decoupler.open();
  Serial.println("OpenCheckpoint");
  delay(1500);
  decoupler.close();
  Serial.println("CloseCheckpoint");
  delay(1500);
}
