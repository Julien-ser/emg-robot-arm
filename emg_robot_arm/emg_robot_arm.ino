#include <Servo.h>

int emgPin = A0;
int servoPin = 2;

Servo servo;  

const int windowSize = 8;   // how many past readings to average (5-10 range)
int readings[windowSize];
int readIndex = 0;
int numReadings = 0;        // tracks how many slots are filled before buffer is full

int flexDrop = 20;         // current must be this much BELOW avg to flex
int relaxRise = 20;         // current must be this much ABOVE avg to unflex

bool servoFlexed = false;

void setup() {
  Serial.begin(9600);
  servo.attach(servoPin);
  servo.write(110); // start relaxed

  for (int i = 0; i < windowSize; i++) {
    readings[i] = analogRead(emgPin);
  }
}

float getAverage() {
  long sum = 0;
  for (int i = 0; i < windowSize; i++) {
    sum += readings[i];
  }
  return (float)sum / windowSize;
}

void loop() {
  int emgValue = analogRead(emgPin);

  // Average of the last N readings (before this new one is added in)
  float avg = getAverage();

  if (!servoFlexed) {
    if (emgValue < (avg - flexDrop)) {
      servoFlexed = true;
      servo.write(165);
    }
  } else {
    if (emgValue > (avg + relaxRise)) {
      servoFlexed = false;
      servo.write(110);
    }
  }

  Serial.print("EMG: ");
  Serial.print(emgValue);
  Serial.print("  Avg: ");
  Serial.print(avg);
  Serial.print("  Servo: ");
  Serial.println(servoFlexed ? "FLEXED" : "RELAXED");

  // Update rolling buffer with the new reading
  readings[readIndex] = emgValue;
  readIndex = (readIndex + 1) % windowSize;

  delay(50);
}