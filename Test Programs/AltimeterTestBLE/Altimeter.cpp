#include "Arduino.h"
#include "Altimeter.h"

Altimeter::Altimeter(double p0) {
  _p0 = p0;
  _altitude = 0;
  _time = 0;
}

bool Altimeter::init() {
  if (BARO.begin() && HTS.begin()) {
    for (int i = 0; i < 5; i++) {
      _readings[i] = 0;
      _altitude = altitude();
    }
    _lastAltitude = _altitude;
    return true;
  }
  return false;
}

double Altimeter::altitude() {
  float temperature = HTS.readTemperature(); // temperature (celsius)
  float pressure = BARO.readPressure() * 10.0; // pressure (hPa)
  double rawAltitude = ((pow((_p0/pressure),(1.0/5.257))-1.0)*(temperature+273.15))/0.0065;
  _lastAltitude = _altitude;
  _lastTime = _time;
  _altitude = updateReadings(rawAltitude);
  _time = millis();
  return _altitude;
}

double Altimeter::updateReadings(double r) {
  _total -= _readings[_index];
  _readings[_index] = r;
  _total += _readings[_index];
  _index = (_index >= 4) ? 0 : _index+1;
  return _total/5.0;
}

double Altimeter::velocity() {
  double velocity = 1000*(_altitude-_lastAltitude)/(_time-_lastTime);
  return velocity;
}

double Altimeter::lastAltitude() {
  return _lastAltitude;
}

void Altimeter::zero(double p0) {
  _p0 = p0;
}
