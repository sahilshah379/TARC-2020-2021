/*
  Altimeter.h
  Created by Sahil Shah, April 16, 2020.

  https://weather.us/observations/pressure-qnh.html
*/

#ifndef Altimeter_h
#define Altimeter_h

#include <Arduino.h>
#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>

class Altimeter {
  private:
    double _p0;
    double _altitude;
    double _lastAltitude;
    unsigned long _time;
    unsigned long _lastTime;
    double _readings[5];
    int _index = 0;
    double _total = 0;

    double updateReadings(double in);
    
  public:
    Altimeter(double p0);
    
    bool init();
    double altitude();
    double velocity();
    double lastAltitude();
    void zero(double p0);
};

#endif
