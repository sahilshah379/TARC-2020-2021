/*
  Decoupler.h
  Created by   Sahil Shah, April 16, 2020.
*/

#ifndef Decoupler_h
#define Decoupler_h
#include <Servo.h>
#include <Arduino.h>

class Decoupler {
  private:
    int _min;
    int _max;

    int _open;
    int _close;

    Servo _servo;

  public:
    Decoupler(int min, int max);
    
    void link(int pin, int open, int close);
    void open();
    void close();

};

#endif
