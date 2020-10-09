#ifndef Altimeter_h
#define Altimeter_h

#include <Arduino.h>
#include <Adafruit_BMP3XX.h>

class Altimeter {
  private:
    Adafruit_BMP3XX _bmp;
    double _p0;
    double _temperature;
    double _pressure;
    double _altitude;
    double _rAltitude[5];
    unsigned long _rTime[5];

    void updateReadings();
    
  public:
    Altimeter(double p0);
    
    bool init();
    bool update();
    double temperature();
    double pressure();
    double altitude();
    double velocity();
    
    void zero(double p0);
};

#endif
