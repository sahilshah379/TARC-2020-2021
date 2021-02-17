#ifndef SECRET_MISSILE_PLANS_CLASSIFIED_CIRCULARBUFFER_H
#define SECRET_MISSILE_PLANS_CLASSIFIED_CIRCULARBUFFER_H

#include <cstring>
#include <Arduino.h>

template<class T, int size>
class CircularBuffer {
private:
    T _buf[size];
    int _head = 0;

public:
    CircularBuffer() = default;

    CircularBuffer(const CircularBuffer<T, size> &buf) {
        for (int i = 0; i < size; ++i) {
            _buf[i] = buf._buf[i];
        }
        _head = buf._head;
    }

    CircularBuffer &operator=(const CircularBuffer<T, size> &buf) {
        if (this != &buf) {
            for (int i = 0; i < size; ++i) {
                _buf[i] = buf._buf[i];
            }
            _head = buf._head;
        }
        return *this;
    }

    T &operator[](int idx) {
        if (idx >= size) {
            pinMode(LED_BUILTIN, OUTPUT);
            digitalWrite(LED_BUILTIN, HIGH);
            for (;;);
        }
        return _buf[(_head + idx) % size];
    }

    void push_back(const T &obj) {
        _buf[_head++ % size] = obj;
    }
};

#endif //SECRET_MISSILE_PLANS_CLASSIFIED_CIRCULARBUFFER_H
