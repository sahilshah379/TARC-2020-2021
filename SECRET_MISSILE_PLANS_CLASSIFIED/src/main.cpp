#include <Arduino.h>
#include <Altimeter.h>
#include <Decoupler.h>
#include <SD.h>

#define DECOUPLER_PIN 10
#define SEA_LEVEL_PRESSURE 1016
#define FLIGHT_TIME 41
#define K 0.40222533776 // kg/m
#define MASS (615.0 / 1000) // g to kg
#define GRAVITY 9.81 // m/s^2
#define OPEN_POS 7
#define CLOSE_POS 58
#define MAX_DEGREES 145
#define OPEN_TIME 1.0
#define UPDATE_TIME 20

#define ZERO_VEL_THRESH 2
#define FALLBACK_RELEASE_TIME 13597L

static unsigned long elapsed_time;
static unsigned long start_time;
static bool fallback = false;
static bool apogee = false;

static CircularBuffer<double, 5000 / UPDATE_TIME> avgVel{};

static Altimeter altimeter(SEA_LEVEL_PRESSURE);
static Decoupler decoupler(0, MAX_DEGREES);

//#define USE_SERIAL
//#define LAUNCH
#define DECOUPLER_TEST
//#define USE_SD

#ifdef USE_SD
static File logFile;
static bool descent = false;
#endif

#ifdef LAUNCH
#define PRE_LAUNCH_TIMEOUT 120
#endif

void setup() {
    // configure led & turn it off
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

#ifdef USE_SERIAL
    Serial.begin(9600);
    while (!Serial);
#endif

#ifdef USE_SD
    if (!SD.begin(4)) {
        // SD card could not be loaded
        digitalWrite(LED_BUILTIN, HIGH);
        for (;;);
    }

    unsigned int log_ctr = 1;
    static String filename = "log" + String(log_ctr) + ".csv";
    while (SD.exists(filename)) {
        filename = "log" + String(++log_ctr) + ".csv";
    }
    logFile = SD.open(filename, FILE_WRITE);
#endif

#ifdef LAUNCH
    // pre-init timeout to allow for rocket assembly
    for (int i = 0; i < PRE_LAUNCH_TIMEOUT; ++i) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }
#endif

    // wait for altimeter to init
    while (!altimeter.init());
    // HACK: erratic readings for first few samples, we ignore first 1000ms here
    for (int i = 0; i < 50; ++i) {
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
#endif
    start_time = millis();
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
        digitalWrite(LED_BUILTIN, HIGH);

#ifdef LAUNCH
        // when we're a bit after the apogee
        if (elapsed_time >= FALLBACK_RELEASE_TIME) {
            decoupler.close();
#ifdef USE_SD
            if (logFile) {
                logFile.println(String(elapsed_time) + ", " + "Decoupled");
            }
#endif
        }
#endif

#ifdef USE_SD
        if (logFile) {
            logFile.println(String(elapsed_time) + ", on fallback");

            // stop logging after 3 minutes
            if (elapsed_time > 180000) {
                logFile.close();
            }
        }
#endif
    } else {

#ifdef USE_SERIAL
        Serial.println(altimeter.velocity());
#endif

#ifdef USE_SD
        if (logFile) {
            logFile.println(
                    String(elapsed_time) + ", " + String(altimeter.altitude()) + ", " + String(altimeter.velocity()));
        }
#endif

        avgVel.push_back(altimeter.velocity());

#ifdef LAUNCH
        if (elapsed_time > 5000) {
            double avg2sec = 0;
            // add from 5 sec to 3 sec
            for (int i = 5000 / UPDATE_TIME - 1; i > (3000 / UPDATE_TIME); --i) {
                avg2sec += avgVel[i];
            }
            avg2sec /= (2000.0 / UPDATE_TIME);

            if (avg2sec < ZERO_VEL_THRESH) {
                apogee = true;
#ifdef USE_SD
                if (logFile) {
                    logFile.println(String(elapsed_time) + ", " + "Apogee Reached");
                }
#endif
            }
        }

        if (apogee && elapsed_time <= (unsigned long) (1000 * (FLIGHT_TIME - OPEN_TIME -
                                                               (altimeter.altitude() -
                                                                altimeter.velocity() * OPEN_TIME -
                                                                0.5 * GRAVITY * pow(OPEN_TIME, 2)) /
                                                               sqrt(MASS * GRAVITY / K)))) {
            decoupler.close();
#ifdef USE_SD
            if (logFile) {
                logFile.println(String(elapsed_time) + ", " + "Decoupled");
            }
#endif
        }
#endif

#ifdef USE_SD
        if (logFile && elapsed_time > 5000) {
            double avg5sec = 0;
            for (int i = 0; i < (5000 / UPDATE_TIME); ++i) {
                avg5sec += avgVel[i];
            }
            avg5sec /= (5000.0 / UPDATE_TIME);
            if (avg5sec < -ZERO_VEL_THRESH) {
                descent = true;
            }
            if (descent && elapsed_time > 20000 && avg5sec < ZERO_VEL_THRESH) {
                logFile.close();
            }
        }
#endif
    }

    delay(UPDATE_TIME);
}

/*
 * https://whatismyelevation.com/
 * https://weather.us/observations/pressure-qnh.html
 * https://weather.us/model-charts/euro/usa/sea-level-pressure/20210217-0500z.html <-- this one is better use NRRR or NAM-C
 * https://rocketcontest.org/wp-content/uploads/2020-Illustrated-TARC-Handbook.pdf (40-43 seconds)
 * https://keisan.casio.com/has10/SpecExec.cgi?path=06000000.Science%252F02100100.Earth%2520science%252F12000300.Altitude%2520from%2520atmospheric%2520pressure%252Fdefault.xml&charset=utf-8
 * http://www.ambrsoft.com/Physics/FreeFall/FreeFallWairResistance.htm
 */