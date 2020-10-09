#include "Arduino.h"
#include "Altimeter.h"

Altimeter::Altimeter(double p0) {
  _p0 = p0;
}

bool Altimeter::init() {
  bool success = false;
  if (_bmp.begin()) {
    _bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_2X);
    _bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X);
    _bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_DISABLE);
    _bmp.setOutputDataRate(BMP3_ODR_100_HZ);
    success = true;
  }
  return success;
}
bool Altimeter::update() {
  if (!_bmp.performReading()) {
    return false;
  }
  _temperature = _bmp.temperature; // temperature (celsius)
  _pressure = _bmp.pressure/100.0; // pressure (hPa)
  _altitude = ((pow((_p0/_pressure),(1.0/5.257))-1.0)*(_temperature+273.15))/0.0065; // altitude (m)
  
  updateReadings();
  return true;
}

void Altimeter::updateReadings() {
  memcpy(_rAltitude, &_rAltitude[1], sizeof(_rAltitude)-sizeof(double));
  memcpy(_rTime, &_rTime[1], sizeof(_rTime)-sizeof(unsigned long));
  _rAltitude[4] = _altitude;
  _rTime[4] = millis();
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
  return 1000*(_rAltitude[4]-_rAltitude[0])/(_rTime[4]-_rTime[0]);
}

void Altimeter::zero(double p0) {
  _p0 = p0;
}
