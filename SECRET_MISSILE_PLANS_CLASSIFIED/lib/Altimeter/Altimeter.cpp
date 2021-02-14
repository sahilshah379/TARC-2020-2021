#include "Altimeter.h"

Altimeter::Altimeter(double p0) : _p0(p0) {}

bool Altimeter::update() {
    if (!_bmp.performReading()) {
        return false;
    }

    _temperature = _bmp.temperature; // temperature (celsius)
    _pressure = _bmp.pressure / 100.0; // pressure (hPa)
    _altitude = ((pow((_p0 / _pressure), (1.0 / 5.257)) - 1.0) * (_temperature + 273.15)) / 0.0065; // altitude (m)

    _rAltitude.push_back(_altitude);
    _rTime.push_back(millis());

    double altitudeAvg = 0;
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        altitudeAvg += _rAltitude[i];
    }
    altitudeAvg /= SAMPLE_SIZE;
    _rAltitudeAvg.push_back(altitudeAvg);

    return true;
}