/*
  Decoupler.h
  Created by Sahil Shah, April 16, 2020.
*/

#ifndef Decoupler_h
#define Decoupler_h

#include <Arduino.h>
#include "mbed.h"

class Decoupler {
  private:
    int _r;
    int _pl;
    int _pu;
    int _min;
    int _max;

    int _duration;
    int _open;
    int _close;

    mbed::DigitalOut *_pin;
    mbed::Timeout _timeout;
    mbed::Ticker _ticker;
    
  public:
    Decoupler(int refresh_rate, int pl, int pu, int min, int max);
    
    void link(int pin, int open, int close);
    void open();
    void close();
    void turn(int angle);

    void start();
    void call();
    void toggle();
};

#endif
