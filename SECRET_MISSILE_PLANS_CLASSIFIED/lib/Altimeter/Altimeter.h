#ifndef Altimeter_h
#define Altimeter_h

#include <Arduino.h>
#include <Adafruit_BMP3XX.h>
#include <CircularBuffer.h>

class Altimeter {
private:
    Adafruit_BMP3XX _bmp;
    double _p0;
    double _temperature;
    double _pressure;
    double _altitude;
    double _altitudeAvg;
    static constexpr int SAMPLE_SIZE = 5;
    CircularBuffer<double> _rAltitude{SAMPLE_SIZE};
    CircularBuffer<unsigned long> _rTime{SAMPLE_SIZE};

public:
    Altimeter() = delete;

    explicit Altimeter(double p0);

    bool init();

    bool update();

    double temperature() const;

    double pressure() const;

    double altitude() const;

    double velocity();

    void zero(double p0);
};

#endif
