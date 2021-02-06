#include <Arduino.h>
//#include <cmath>
#include <Altimeter.h>
#include <Decoupler.h>

const int DECOUPLER_PIN = 10;
const double SEA_LEVEL_PRESSURE = 1019; // https://weather.us/observations/pressure-qnh.html
const double FLIGHT_TIME = 41; // https://rocketcontest.org/wp-content/uploads/2020-Illustrated-TARC-Handbook.pdf (40-43 seconds)
const double K = 0.40222533776;
const double MASS = 574.83 / 1000; // g to kg
const double GRAVITY = 9.81; // m/s^2
const int OPEN_POS = 30;
const int CLOSE_POS = 120;
const int MAX_DEGREES = 145;
const double OPEN_TIME = 1;

static unsigned long current_time;
static unsigned long start_time;

Altimeter altimeter(SEA_LEVEL_PRESSURE);
Decoupler decoupler(0, MAX_DEGREES);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.begin(9600);
    while (!Serial);
    // wait for altimeter to init
    while (!altimeter.init());
//    decoupler.link(DECOUPLER_PIN, OPEN_POS, CLOSE_POS);
//    decoupler.open();
//    current_time = 0;
    // wait for launch
//    while (altimeter.velocity() < 10);
    // ascent
}

void loop() {
//    decoupler.open();
//    delay(1000);
//    decoupler.close();
//    delay(1000);

    if (!altimeter.update()) {
        Serial.println("Failed to read altimeter data");
        return;
    }
    Serial.println(altimeter.velocity());
    delay(20);

//    current_time += (millis() - start_time);
//    if (!altimeter.update()) {
//        return;
//    }
//
//    double altitude = altimeter.altitude();
//    double velocity = altimeter.velocity();
//
//    if (current_time / 1000 >= FLIGHT_TIME - OPEN_TIME -
//                               (velocity * OPEN_TIME + 0.5 * GRAVITY * pow(OPEN_TIME, 2) + altitude) /
//                               sqrt(MASS * GRAVITY / K)) {
//        decoupler.close();
//    }
}

//void setup() {
//    pinMode(LED_BUILTIN, OUTPUT);
//}
//
//void loop() {
//    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//    delay(1000);                       // wait for a second
//    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//    delay(1000);                       // wait for a second
//}