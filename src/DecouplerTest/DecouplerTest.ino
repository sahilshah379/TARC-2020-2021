#include "Decoupler.h"
#include <Servo.h>

const int pin = 10;

const int refresh_interval = 3333;
const int pulse_lower = 750;
const int pulse_upper = 2250;
const int max_degrees = 145;

const int openPos = 30;
const int closePos = 120;

Servo myservo;

Decoupler decoupler(refresh_interval, pulse_lower, pulse_upper, 0, max_degrees);

void setup() {
  decoupler.link(pin, openPos, closePos);
  myservo.attach(pin);
}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) {
    myservo.write(pos);
    delay(15);
  }
  for (pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);
  }
}
