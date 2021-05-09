#include <Arduino.h>
#include <Altimeter.h>
#include <Decoupler.h>
#include <SD.h>

#define DECOUPLER_PIN 10
#define SEA_LEVEL_PRESSURE 1018
#define FLIGHT_TIME 60.0 //fuck you TARC you don't control me
#define K 0.40222533776 // kg/m
#define MASS (615.0 / 1000) // g to kg
#define GRAVITY 9.81 // m/s^2
#define OPEN_POS 143.0
#define CLOSE_POS 93.0
#define MAX_DEGREES 145.0
#define OPEN_TIME 1.5
#define UPDATE_TIME 20
#define MAGIC_NUMBER 5.73
#define PRE_LAUNCH_TIMEOUT 300 // seconds
#define ZERO_VEL_THRESH 2.0
#define FALLBACK_RELEASE_TIME 11735L

static unsigned long elapsed_time;
static unsigned long start_time;
static bool fallback = false;

static bool decouple_math = false;
static bool decouple_fallback_altitude = false;
static bool decouple_fallback_time = false;

static CircularBuffer<double, int(5000 / UPDATE_TIME)> avgVel{};

static Altimeter altimeter(SEA_LEVEL_PRESSURE);
static Decoupler decoupler(0, MAX_DEGREES);

//static File logFile;


void setup() {
    // configure led & turn it off
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);


//    if (!SD.begin(4)) {
//        // SD card could not be loaded
//        digitalWrite(LED_BUILTIN, HIGH);
//        for (;;);
//    }

//    unsigned int log_ctr = 1;
//    static String filename = "log" + String(log_ctr) + ".csv";
//    while (SD.exists(filename)) {
//        filename = "log" + String(++log_ctr) + ".csv";
//    }
//    logFile = SD.open(filename, FILE_WRITE);

    decoupler.link(DECOUPLER_PIN, OPEN_POS, CLOSE_POS);
    decoupler.open();
    // pre-init timeout to allow for rocket assembly
    for (int i = 0; i < PRE_LAUNCH_TIMEOUT; ++i) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }

    // wait for altimeter to init
    while (!altimeter.init());
    // HACK: erratic readings for first few samples, we ignore first 1000ms here
    for (int i = 0; i < 50; ++i) {
        altimeter.update();
    }

    // wait for launch
    while (abs(altimeter.velocity()) < 5);
    start_time = millis();
}

void loop() {
    elapsed_time = millis() - start_time;
    if (decouple_math || decouple_fallback_time || decouple_fallback_altitude) {
        if (decouple_math) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(3000);
            digitalWrite(LED_BUILTIN, LOW);
            delay(1000 - UPDATE_TIME);
        } else if (decouple_fallback_time) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(5000);
            digitalWrite(LED_BUILTIN, LOW);
            delay(1000 - UPDATE_TIME);
        } else if (decouple_fallback_altitude) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(7000);
            digitalWrite(LED_BUILTIN, LOW);
            delay(1000 - UPDATE_TIME);
        }
    } else {
        if (fallback || !altimeter.update()) {
            // something has gone horribly wrong, switching to time-based fallback
            fallback = true;
            digitalWrite(LED_BUILTIN, HIGH);

            // when we're a bit after the apogee
            if (elapsed_time >= FALLBACK_RELEASE_TIME) {
                decoupler.close();
                decouple_fallback_time = true;
                //            if (logFile) {
                //                logFile.println(String(elapsed_time) + ", Decoupled (FALLBACK - TIME)");
                //            }
            }

            //        if (logFile) {
            //            logFile.println(String(elapsed_time) + ", on fallback");
            //            // stop logging after 3 minutes
            //            if (elapsed_time > 180000) {
            //                logFile.close();
            //                exit(0);
            //            }
            //        }
        } else {
            avgVel.push_back(altimeter.velocity());

            if (elapsed_time > 1000) {
                //            if (avgVel.avg(1000 / UPDATE_TIME) < ZERO_VEL_THRESH) {
                //                if (logFile) {
                //                    logFile.println(String(elapsed_time) + ", " + "Apogee Reached");
                //                }
                //            }

                if (avgVel.avg(1000 / UPDATE_TIME) < 0 && altimeter.altitude() < 120) { // altitude fallback
                    decoupler.close();
                    decouple_fallback_altitude = true;
                    //                if (logFile) {
                    //                    logFile.println(String(elapsed_time) + ", " + "Decoupled (FALLBACK - ALTITUDE)");
                    //                }
                }

                if (avgVel.avg(1000 / UPDATE_TIME) < 0 &&
                    elapsed_time <= (unsigned long) (1000 * (FLIGHT_TIME - OPEN_TIME -
                                                             (altimeter.altitude() -
                                                              altimeter.velocity() *
                                                              OPEN_TIME -
                                                              (MAGIC_NUMBER / 2.0) *
                                                              pow(OPEN_TIME, 2)) /
                                                             sqrt(MASS * GRAVITY /
                                                                  K)))) {
                    decoupler.close();
                    decouple_math = true;
                    //                if (logFile) {
                    //                    logFile.println(String(elapsed_time) + ", " + "Decoupled");
                    //                }
                }
            }

            //        if (logFile) {
            //            logFile.println(
            //                    String(elapsed_time) + ", " + String(altimeter.altitude()) + ", " + String(altimeter.velocity()));
            //        }

            //        if (elapsed_time > 180000) {
            //            logFile.close();
            //            exit(0);
            //        }
        }
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