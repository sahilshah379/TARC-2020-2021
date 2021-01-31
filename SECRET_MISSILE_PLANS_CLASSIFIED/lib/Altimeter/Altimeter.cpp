#include "Altimeter.h"

Altimeter::Altimeter(double p0) : _p0(p0) {}

bool Altimeter::init() {
    if (_bmp.begin()) {
        _bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_2X);
        _bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X);
        _bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_DISABLE);
        _bmp.setOutputDataRate(BMP3_ODR_100_HZ);
        return true;
    }
    return false;
}

bool Altimeter::update() {
    if (!_bmp.performReading()) {
        return false;
    }

    _temperature = _bmp.temperature; // temperature (celsius)
    _pressure = _bmp.pressure / 100.0; // pressure (hPa)
    _altitude = ((pow((_p0 / _pressure), (1.0 / 5.257)) - 1.0) * (_temperature + 273.15)) / 0.0065; // altitude (m)

    _rAltitude.push_back(_altitude);
    _rTime.push_back(millis());
    return true;
}

double Altimeter::temperature() {
    return _temperature;
}

double Altimeter::pressure() {
    return _pressure;
}

double Altimeter::altitude() {
    return _altitude;
}

double Altimeter::velocity() {
    double altitudeAvg = 0, timeAvg = 0;
    for (int i = 0; i < 5; i++) {
        altitudeAvg += _rAltitude[i];
        timeAvg += _rAltitude[i];
    }
    altitudeAvg /= 5;
    timeAvg /= 5;
    return 1000 * altitudeAvg / timeAvg;
}

void Altimeter::zero(double p0) {
    _p0 = p0;
}
