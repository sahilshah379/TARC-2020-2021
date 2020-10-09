#include "Altimeter.h"

Altimeter altimeter(1015);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  if (!altimeter.init()) {
    Serial.println("Failed to initialize the altimeter");
    while(1);
  }
}

void loop() {
  if (!altimeter.update()) {
    Serial.println("Failed to read altimeter data");
    return;
  }
  Serial.println(altimeter.altitude());
  delay(20);
}
