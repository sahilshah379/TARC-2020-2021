#ifndef SECRET_MISSILE_PLANS_CLASSIFIED_CIRCULARBUFFER_H
#define SECRET_MISSILE_PLANS_CLASSIFIED_CIRCULARBUFFER_H

#include <string.h>
#include <Arduino.h>

template<class T>
class CircularBuffer {
private:
    T *_buf = nullptr;
    int _head = 0;

public:
    const int size;

    CircularBuffer() = delete;

    CircularBuffer(int length) : size(length) {
        _buf = static_cast<T *>(malloc((size + 1) * sizeof(T)));
    }

    CircularBuffer(const CircularBuffer<T> &buf) : size(buf.size) {
        _buf = static_cast<T *>(malloc((buf.size + 1) * sizeof(T)));
        memcpy(_buf, buf._buf, sizeof(T) * (buf.size + 1));
        _head = buf._head;
    }

    CircularBuffer &operator=(const CircularBuffer<T> &buf) {
        if (this != &buf) {
            if (_buf != nullptr) {
                free(_buf);
            }
            _buf = static_cast<T *>(malloc((buf.size + 1) * sizeof(T)));
            memcpy(_buf, buf._buf, sizeof(T) * (buf.size + 1));
            size = buf.size;
            _head = buf._head;
        }
        return *this;
    }

    ~CircularBuffer() {
        free(_buf);
    }

    T &operator[](int idx) {
        if (idx >= size) {
            Serial.print("Circular Buffer out of bounds!");
            exit(0);
        }
        return _buf[(_head + idx) % size];
    }

    void push_back(const T &obj) {
        _buf[_head++ % size] = obj;
    }
};

#endif //SECRET_MISSILE_PLANS_CLASSIFIED_CIRCULARBUFFER_H
