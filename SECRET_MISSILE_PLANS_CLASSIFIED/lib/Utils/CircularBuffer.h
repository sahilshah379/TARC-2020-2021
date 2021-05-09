#ifndef ROCKET_KEK_CIRCULARBUFFER_H
#define ROCKET_KEK_CIRCULARBUFFER_H

template<class T, unsigned int SIZE>
class CircularBuffer {
private:
    T _buf[SIZE] = {};
    int _head = 0;

public:
    CircularBuffer() = default;

    CircularBuffer(const CircularBuffer<T, SIZE> &rhs) {
        for (unsigned int i = 0; i < SIZE; ++i) {
            _buf[i] = rhs._buf[i];
        }
        _head = rhs._head;
    }

    CircularBuffer &operator=(const CircularBuffer<T, SIZE> &rhs) {
        if (this != &rhs) {
            for (unsigned int i = 0; i < SIZE; ++i) {
                _buf[i] = rhs._buf[i];
            }
            _head = rhs._head;
        }
        return *this;
    }

    T &operator[](unsigned int idx) {
        if (idx >= SIZE) {
            pinMode(LED_BUILTIN, OUTPUT);
            digitalWrite(LED_BUILTIN, HIGH);
            return *(new T());
        }
        return _buf[(_head + idx) % SIZE];
    }

    void push_back(const T &obj) {
        _buf[_head++ % SIZE] = obj;
    }

    unsigned int size() const {
        return SIZE;
    }

    int head() const {
        return _head;
    }

    T back() const {
        return _buf[(_head - 1 + SIZE) % SIZE];
    }

    T get(unsigned int idx) const {
        return _buf[idx];
    }

    double avg(unsigned int samp) const;
};

// specialized average funcs

template<unsigned int SIZE>
double avg_aux(const CircularBuffer<int, SIZE> &buf, unsigned int samp) {
    double avg = 0;
    int front = (buf.head() - 1 + SIZE) % SIZE;
    for (unsigned  i = front; i != (front - samp - 1 + SIZE) % SIZE; i = (i - 1 + SIZE) % SIZE) {
        avg += buf.get(i);
    }
    return avg / samp;
}

template<unsigned int SIZE>
double avg_aux(const CircularBuffer<double, SIZE> &buf, unsigned int samp) {
    double avg = 0;
    int front = (buf.head() - 1 + SIZE) % SIZE;
    avg += buf.get(front);
    front = (front - 1 + SIZE) % SIZE;
    for (unsigned i = front; i != (front - samp - 1 + SIZE) % SIZE; i = (i - 1 + SIZE) % SIZE) {
        avg += buf.get(i);
    }
    return avg / samp;
}

template<class T, unsigned int SIZE>
double avg_aux(const CircularBuffer<T, SIZE> &buf, unsigned int samp) {
    return 0;
}

// definition of class method

template<class T, unsigned int SIZE>
double CircularBuffer<T, SIZE>::avg(unsigned int samp) const {
    if (samp > SIZE || samp == 0) {
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH);
        return *(new T());
    }
    return avg_aux(*this, samp);
}

#endif //ROCKET_KEK_CIRCULARBUFFER_H
