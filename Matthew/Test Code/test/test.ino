#include <AccelStepper.h>

// Connections to A4982
const int dirPin = 2; // Direction
const int stepPin = 3; // Step
const int NUMBER_OF_ROTATIONS = 15;

// Motor steps per rotation
const int STEPS_PER_REV = 3200;

// Sample window width in mS (50 mS = 20Hz)
const int sampleWindow = 50;
unsigned int sample;

#define SENSOR_PIN A0

// Add new variables to track motor start time and sound threshold
unsigned long motorStartTime = 0;
bool motorRunning = false;
bool soundThresholdCrossed = false;

AccelStepper stepper(1, stepPin, dirPin); // 1 = Driver mode, stepPin, dirPin

void setup() {
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
  pinMode(SENSOR_PIN, INPUT); // Set the signal pin as input A0
  Serial.begin(9600); //Baud Rate
  stepper.setSpeed(1000); // set motor speed
}

void loop() {
  unsigned long startMillis = millis(); // Start of sample window
  float peakToPeak = 0; // peak-to-peak level
  unsigned int signalMax = 0; //minimum value
  unsigned int signalMin = 1024; //maximum value
  int currentRotation = 0; //number of rotations

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(SENSOR_PIN); //get reading from microphone
    if (sample < 1024) // toss out spurious readings
    {
      if (sample > signalMax) {
        signalMax = sample; // save just the max levels
      } else if (sample < signalMin) {
        signalMin = sample; // save just the min levels
      }
    }
  }

  peakToPeak = signalMax - signalMin; // max - min = peak-peak amplitude
  int db = map(peakToPeak, 20, 900, 49.5, 90); //calibrate for deciBels

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
    // Motor should continue running
    if (db >= 55 && db <= 64) {
      stepper.move(STEPS_PER_REV * NUMBER_OF_ROTATIONS);
      stepper.run();
    } else if (db >= 65 && db <= 74) {
      stepper.move(STEPS_PER_REV * NUMBER_OF_ROTATIONS);
      stepper.run();
    } else if (db >= 75) {
      stepper.move(STEPS_PER_REV * NUMBER_OF_ROTATIONS);
      stepper.run();
    }

    // Check if current rotation has reached the NUMBER_OF_ROTATIONS value
    if (stepper.distanceToGo() == 0) {
      motorRunning = false;
      soundThresholdCrossed = false;
      delay(10000); // wait for 10
    }
  }
}
