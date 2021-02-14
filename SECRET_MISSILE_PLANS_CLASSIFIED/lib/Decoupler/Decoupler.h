#ifndef Decoupler_h
#define Decoupler_h

#include <Servo.h>
#include <Arduino.h>

class Decoupler {
private:
    const int _min;
    const int _max;

    int _open{};
    int _close{};

    Servo _servo{};

public:
    Decoupler() = delete;

    Decoupler(int min, int max);

    void link(int pin, int open, int close);

    void open();

    void close();
};

#endif
