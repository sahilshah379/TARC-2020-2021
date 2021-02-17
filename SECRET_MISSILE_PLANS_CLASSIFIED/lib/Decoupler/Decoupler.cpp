#include "Decoupler.h"

Decoupler::Decoupler(int min, int max) : _min(min), _max(max) {}

void Decoupler::link(int pin, int open, int close) {
    _open = open;
    _close = close;

    pinMode(pin, OUTPUT);
    _servo.attach(pin);
}

void Decoupler::open() {
    for (int pos = _min; pos <= _open; ++pos) {
        _servo.write(pos);
    }
}

void Decoupler::close() {
    for (int pos = _max; pos >= _close; --pos) {
        _servo.write(pos);
    }
}
