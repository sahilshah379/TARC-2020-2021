#include <Arduino.h>
#include <Altimeter.h>
#include <Decoupler.h>

const int DECOUPLER_PIN = 10;
const double SEA_LEVEL_PRESSURE = 1027; // https://weather.us/observations/pressure-qnh.html
const double FLIGHT_TIME = 41; // https://rocketcontest.org/wp-content/uploads/2020-Illustrated-TARC-Handbook.pdf (40-43 seconds)
const double K = 0.40222533776;
const double MASS = 574.83 / 1000; // g to kg
const double GRAVITY = 9.81; // m/s^2
const int OPEN_POS = 40;
const int CLOSE_POS = 90;
const int MAX_DEGREES = 145;
const double OPEN_TIME = 1;

static unsigned long current_time;
static unsigned long start_time;

static Altimeter altimeter(SEA_LEVEL_PRESSURE);
static Decoupler decoupler(0, MAX_DEGREES);

//#define USE_SERIAL
#define LAUNCH
//#define DECOUPLER_TEST

void setup() {
#ifdef USE_SERIAL
    Serial.begin(9600);
    while (!Serial);
#endif

    // wait for altimeter to init
    while (!altimeter.init());
    // populate buffer
    for (int i = 0; i < 10; i++) {
        altimeter.update();
    }

#ifdef DECOUPLER_TEST
    decoupler.link(DECOUPLER_PIN, OPEN_POS, CLOSE_POS);
    decoupler.open();
#endif

#ifdef LAUNCH
    decoupler.link(DECOUPLER_PIN, OPEN_POS, CLOSE_POS);
    decoupler.open();
    current_time = 0;
    // wait for launch
    while (altimeter.velocity() < 10);
#endif
}

void loop() {
#ifdef DECOUPLER_TEST
    decoupler.open();
    delay(1000);
    decoupler.close();
    delay(1000);
#endif

    if (!altimeter.update()) {
        return;
    }

#ifdef USE_SERIAL
    Serial.println(altimeter.velocity());
#endif

#ifdef LAUNCH
    current_time += (millis() - start_time);
    double altitude = altimeter.altitude();
    double velocity = altimeter.velocity();

    if (current_time <= (long) (1000 * (FLIGHT_TIME - OPEN_TIME -
                                        (altitude - velocity * OPEN_TIME - 0.5 * GRAVITY * pow(OPEN_TIME, 2)) /
                                        sqrt(MASS * GRAVITY / K)))) {
        decoupler.close();
    }
#endif

    delay(20);
}