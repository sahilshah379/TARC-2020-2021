#include "Altimeter.h"

Altimeter::Altimeter(double p0) : _p0(p0) {}

bool Altimeter::init() {
    return _bmp.begin() && _bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X) &&
           _bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X) && _bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3) &&
           _bmp.setOutputDataRate(BMP3_ODR_100_HZ);
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

    _altitudeAvg = 0;
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        _altitudeAvg += _rAltitude[i];
    }
    _altitudeAvg /= SAMPLE_SIZE;

    return true;
}

double Altimeter::temperature() const {
    return _temperature;
}

double Altimeter::pressure() const {
    return _pressure;
}

double Altimeter::altitude() const {
    return _altitudeAvg;
}

double Altimeter::velocity() {
    return 1000.0 * (_rAltitude[SAMPLE_SIZE - 1] - _rAltitude[0]) / (_rTime[SAMPLE_SIZE - 1] - _rTime[0]);
}

void Altimeter::zero(double p0) {
    _p0 = p0;
}
