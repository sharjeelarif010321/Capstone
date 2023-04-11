// Define Constants

// Connections to A4988
const int dirPin = 2;  // Direction
const int stepPin = 3; // Step

// Motor steps per rotation
const int STEPS_PER_REV = 3200;

// Microphone input pin
const int MIC_PIN = A0;

// Variables for decibel level and motor speed
int decibelLevel;
int motorSpeed;

// Threshold for stopping the motor
const int THRESHOLD = 55;

void setup() {

  // Setup the pins as Outputs
  pinMode(stepPin, OUTPUT); 
  pinMode(dirPin, OUTPUT);

  // Setup the microphone input as an Input
  pinMode(MIC_PIN, INPUT);

  Serial.begin(9600); // Baud Rate

}

void loop() {
  // Set motor direction clockwise
  digitalWrite(dirPin, HIGH); 

  while (true) {
    // Read the microphone input and calculate the decibel level
    int micValue = analogRead(MIC_PIN);
    decibelLevel = map(micValue, 0, 1023, 0, 100); // Map analog value to decibel level (0-100)
    // Calculate motor speed based on the decibel level
    if (decibelLevel > 55) {
      motorSpeed = 200; // Slow speed for quiet environments
    } else if (decibelLevel >= 65 && decibelLevel < 74) {
      motorSpeed = 150; // Medium speed for moderate noise levels
    } else if (decibelLevel < 75) {
      motorSpeed = 100; // Fast speed for loud environments
    }

    // Set motor speed
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(motorSpeed);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(motorSpeed);

    // Check if the decibel level is less than the threshold
    if (decibelLevel > THRESHOLD) {
      // Continue rotating the motor
      continue;
    } else {
      // Stop the motor
      digitalWrite(stepPin, LOW);
      break;
    }
  }
}
