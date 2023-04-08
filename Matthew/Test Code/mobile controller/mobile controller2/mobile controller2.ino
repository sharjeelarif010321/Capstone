#include <AccelStepper.h>

const int dirPin = 2;  // Direction
const int stepPin = 3; // Step
const int NUMBER_OF_ROTATIONS = 100;
const int STEPS_PER_REV = 3200;

const int sampleWindow = 50;                              // Sample window width in mS (50 mS = 20Hz)

const int SENSOR_PIN = A0;
unsigned int sample;

AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

unsigned long motorStartTime = 0;
bool motorRunning = false;
bool soundThresholdCrossed = false;

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(9600);
  digitalWrite(dirPin, HIGH);
  stepper.setMaxSpeed(2000);
  stepper.setAcceleration(2000);
  stepper.setSpeed(0);
}

void loop() {
  unsigned long startMillis = millis();
  float peakToPeak = 0;
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  for (millis() - startMillis < sampleWindow) {
    sample = analogRead(SENSOR_PIN);

    if (sample < 1024) {
      if (sample > signalMax) {
        signalMax = sample;
      } else if (sample < signalMin) {
        signalMin = sample;
      }
    }

    peakToPeak = signalMax - signalMin;
    int db = map(peakToPeak, 20, 900, 49.5, 90);
    Serial.println(db);
    if (db >= 55) {
      soundThresholdCrossed = true;
      if (!motorRunning) {
        motorStartTime = millis();
        motorRunning = true;
      }
    } else if (soundThresholdCrossed) {
      if ((millis() - motorStartTime) < 10000) {
        // Do nothing, let the motor continue running
      } else {
        soundThresholdCrossed = false;
        motorRunning = false;
      }
    }

    if (soundThresholdCrossed || motorRunning) {
      if (db >= 55 && db <= 64) {
        stepper.setSpeed(1000);
      } else if (db >= 65 && db <= 74) {
        stepper.setSpeed(1500);
      } else if (db >= 75) {
        stepper.setSpeed(2000);
      }

      stepper.runSpeed();
      if (stepper.distanceToGo() == 0) {
        stepper.setCurrentPosition(0);
      }
      digitalWrite(dirPin, HIGH);
      motorRunning = true;
    } else {
      stepper.stop();
      stepper.setSpeed(0);
      digitalWrite(dirPin, LOW);
      motorRunning = false;
    }
  }
}
