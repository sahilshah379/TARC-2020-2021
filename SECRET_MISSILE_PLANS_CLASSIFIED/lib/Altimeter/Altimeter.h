#ifndef Altimeter_h
#define Altimeter_h

#include <Arduino.h>
#include <Adafruit_BMP3XX.h>
#include <CircularBuffer.h>

class Altimeter {
private:
    Adafruit_BMP3XX _bmp{};
    double _p0{};
    double _temperature{};
    double _pressure{};
    double _altitude{};
    static constexpr int SAMPLE_SIZE = 5;
    CircularBuffer<double, SAMPLE_SIZE> _rAltitude{};
    CircularBuffer<double, SAMPLE_SIZE> _rAltitudeAvg{};
    CircularBuffer<unsigned long, SAMPLE_SIZE> _rTime{};

public:
    Altimeter() = delete;

    explicit Altimeter(double p0);

    inline bool init() {
        return _bmp.begin() && _bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X) &&
        _bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X) && _bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3) &&
        _bmp.setOutputDataRate(BMP3_ODR_100_HZ);
    }

    bool update();

    inline double temperature() const {
        return _temperature;
    }

    inline double pressure() const {
        return _pressure;
    }

    inline double altitude() {
        return _rAltitudeAvg[SAMPLE_SIZE - 1];
    }

    inline double velocity() {
        return 1000.0 * (_rAltitudeAvg[SAMPLE_SIZE - 1] - _rAltitudeAvg[0]) / (_rTime[SAMPLE_SIZE - 1] - _rTime[0]);
    }

    inline void zero(double p0) {
        _p0 = p0;
    }
};

#endif
