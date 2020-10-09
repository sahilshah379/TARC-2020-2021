#include <PID_v1.h>

#define EN_A A5
#define EN_B A4
#define PH 10
#define EN 9

int ticks = 0;
double input, output, target;
double kp=0.2, ki=0, kd=0;

PID controller(&input, &output, &target, kp, ki, kd, DIRECT);

void setup() {
  pinMode(EN_A, INPUT);
  pinMode(EN_B, INPUT);
  pinMode(PH, OUTPUT);
  pinMode(EN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(EN_A), isr, CHANGE);

  input = ticks;
  target = 0;

  controller.SetMode(AUTOMATIC);
  controller.SetOutputLimits(-255,255);
  
  Serial.begin(9600);
  while(!Serial);
}

void loop() {
  if (Serial.available() > 0) {
    target = (Serial.readStringUntil('\n')).toDouble();
  }
  input = ticks;
  controller.Compute();
  int power = map(abs(output),0,255,25,255)*constrain(output,-1,1);
  
  if (abs(output) < 5) {
    digitalWrite(PH,LOW);
    analogWrite(EN,0);
  } else if (power > 0) {
    digitalWrite(PH,1);
    analogWrite(EN,abs(power));
  } else if (power < 0) {
    digitalWrite(PH,0);
    analogWrite(EN,abs(power));
  }

  if (power < 10) Serial.print(' ');
  if (power < 100) Serial.print(' ');
  Serial.print(power);
  Serial.print('\t');
  Serial.println(ticks);
  delay(50);
}

void isr() {
  if (digitalRead(EN_A) == HIGH) {
    if (digitalRead(EN_B) == LOW) {
      ticks++;
    } else {
      ticks--;
    }
  } else {
    if (digitalRead(EN_B) == LOW) {
      ticks--;
    } else {
      ticks++;
    }
  }
}
