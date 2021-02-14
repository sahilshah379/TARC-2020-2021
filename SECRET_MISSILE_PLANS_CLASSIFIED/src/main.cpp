#include <Arduino.h>
#include <Altimeter.h>
#include <Decoupler.h>

#define DECOUPLER_PIN 10
#define SEA_LEVEL_PRESSURE 1027 // https://weather.us/observations/pressure-qnh.html
#define FLIGHT_TIME 41 // https://rocketcontest.org/wp-content/uploads/2020-Illustrated-TARC-Handbook.pdf (40-43 seconds)
#define K 0.40222533776
#define MASS (596.0 / 1000) // g to kg
#define GRAVITY 9.81 // m/s^2
#define OPEN_POS 40
#define CLOSE_POS 90
#define MAX_DEGREES 145
#define OPEN_TIME 1.0

#define FALLBACK_RELEASE_TIME 12000L

static unsigned long elapsed_time;
static unsigned long start_time;
static bool fallback = false;

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
    // HACK: erratic readings for first few samples, we ignore first 1000ms here
    for (int i = 0; i < 50; i++) {
        altimeter.update();
    }

#ifdef DECOUPLER_TEST
    decoupler.link(DECOUPLER_PIN, OPEN_POS, CLOSE_POS);
    decoupler.open();
#endif

#ifdef LAUNCH
    decoupler.link(DECOUPLER_PIN, OPEN_POS, CLOSE_POS);
    decoupler.open();
    // wait for launch
    while (altimeter.velocity() < 10);
    start_time = millis();
#endif
}

void loop() {
#ifdef DECOUPLER_TEST
    decoupler.open();
    delay(1000);
    decoupler.close();
    delay(1000);
#endif

    elapsed_time = millis() - start_time;

    if (fallback || !altimeter.update()) {
        // something has gone horribly wrong, switching to time-based fallback
        fallback = true;
        // when we're a bit after the apogee
        if (elapsed_time >= FALLBACK_RELEASE_TIME) {
            decoupler.close();
        }
    }

#ifdef USE_SERIAL
    Serial.println(altimeter.velocity());
#endif

#ifdef LAUNCH
    if (elapsed_time <= (unsigned long) (1000 * (FLIGHT_TIME - OPEN_TIME -
                                                 (altimeter.altitude() - altimeter.velocity() * OPEN_TIME -
                                                  0.5 * GRAVITY * pow(OPEN_TIME, 2)) /
                                                 sqrt(MASS * GRAVITY / K)))) {
        decoupler.close();
    }
#endif

    delay(20);
}