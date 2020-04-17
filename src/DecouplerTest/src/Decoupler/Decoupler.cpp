#include "Arduino.h"
#include "Decoupler.h"

Decoupler::Decoupler(int refresh_rate, int pl, int pu, int min, int max) {
  _r = refresh_rate;
  _pl = pl;
  _pu = pu;
  _min = min;
  _max = max;
  _duration = -1;
}

void Decoupler::link(int pin, int open, int close) {
  _open = open;
  _close = close;

  pinMode(pin,OUTPUT);
  _pin = new mbed::DigitalOut(digitalPinToPinName(pin));
}

void Decoupler::open() {
  turn(_open);
}

void Decoupler::close() {
  turn(_close);
}

void Decoupler::turn(int angle) {
  int pulse = map(angle, _min, _max, _pl, _pu);
  
  if (_duration == -1) {
    start();
    _duration = pulse;
  } else {
    _duration = pulse;
  }
}
void Decoupler::start() {
  _ticker.attach(mbed::callback(this, &Decoupler::call),_r/1e6);
}

void Decoupler::call() {
  _timeout.attach(mbed::callback(this, &Decoupler::toggle),_duration/1e6);
}

void Decoupler::toggle() {
  *_pin = !*_pin;
}