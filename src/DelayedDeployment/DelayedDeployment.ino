#include <math.h>
#include "Arduino.h"
#include "Altimeter.h"
#include "Decoupler.h"

const int decoupler_pin = 10;
const double sea_level_pressure = 1019; // https://weather.us/observations/pressure-qnh.html
const double flight_time = 41; // https://rocketcontest.org/wp-content/uploads/2020-Illustrated-TARC-Handbook.pdf (40-43 seconds)
const double k = 0.40222533776;
const double mass = 574.83/1000; // g to kg
const double gravity = 9.81; // m/s^2
const int openPos = 30;
const int closePos = 120;
const int max_degrees = 145;
unsigned long current_time;
unsigned long start_time;

Altimeter altimeter(sea_level_pressure);
Decoupler decoupler(0, max_degrees);

void setup() {
  while(!altimeter.init());
  decoupler.link(decoupler_pin, openPos, closePos);
  decoupler.open();
  current_time = 0;
  start_time = millis();
}

void loop() {
  current_time += (millis()-start_time);
  if (!altimeter.update()) {
    return;
  }
  
  double altitude = altimeter.altitude();
  double velocity = altimeter.velocity();
  double delta_time = sqrt(mass/(gravity*k))*(acosh(exp((altitude*k)/mass)-(0.5*log(1-((velocity*velocity*k)/(mass*gravity)))))-(atanh(velocity*sqrt(k/(mass*gravity)))));
  
  if(current_time*1000+delta_time <= flight_time) {
    decoupler.close();
  }
}
