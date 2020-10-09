#include "Altimeter.h"

Altimeter altimeter(1019);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  if (!altimeter.init()) {
    Serial.println("Failed initializing the altimeter");
    while(1);
  }
}

void loop() {
  double altitude = altimeter.altitude();
  Serial.println(altitude);
  delay(10);
}
