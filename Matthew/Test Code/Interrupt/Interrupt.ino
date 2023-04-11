const int dirPin = 2;  // Direction
const int stepPin = 3; // Step
const int STEPS_PER_REV = 3200; // Motor steps per rotation
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned long previousMotorMillis = 0;

#define SENSOR_PIN A0

void setup() {
  pinMode(stepPin, OUTPUT); // Setup the pins as Outputs
  pinMode(dirPin, OUTPUT); // Setup the pins as Outputs
  pinMode(SENSOR_PIN, INPUT); // Set the signal pin as input
  Serial.begin(9600); // Baud Rate
}

void loop() {
  unsigned long currentMillis = millis(); // Current time
  static bool motorRunning = false;
  
  // Check if it's time to take a sample
  if (currentMillis - previousMotorMillis >= 5000) {
    previousMotorMillis = currentMillis;
    
    // Check if sound level is above threshold and motor is not already running
    float peakToPeak = 0; // Peak-to-peak level
    unsigned int signalMax = 0; // Minimum value
    unsigned int signalMin = 1024; // Maximum value
    unsigned long startMillis = millis(); // Start of sample window
    while (millis() - startMillis < sampleWindow) {
      unsigned int sample = analogRead(SENSOR_PIN); // Get reading from microphone
      if (sample < 1024) {
        if (sample > signalMax) {
          signalMax = sample; // Save just the max levels
        }
        else if (sample < signalMin) {
          signalMin = sample; // Save just the min levels
        }
      }
    }
    peakToPeak = signalMax - signalMin; // Max - min = peak-peak amplitude
    int db = map(peakToPeak, 20, 900, 49.5, 90); // Calibrate for deciBels
    if (db > 55 && !motorRunning) { // Threshold check
      motorRunning = true; // Set flag to indicate motor is running
      digitalWrite(dirPin, HIGH); // Set direction
      unsigned long motorDuration = 0;
      while (motorDuration < 5000) { // Limit motor runtime to 5 seconds
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(200);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(200);
        
        // Check if sound level has dropped below threshold
        signalMax = 0; // Minimum value
        signalMin = 1024; // Maximum value
        startMillis = millis(); // Start of sample window
        while (millis() - startMillis < sampleWindow) {
          unsigned int sample = analogRead(SENSOR_PIN); // Get reading from microphone
          if (sample < 1024) {
            if (sample > signalMax) {
              signalMax = sample; // Save just the max levels
            }
            else if (sample < signalMin) {
              signalMin = sample; // Save just the min levels
            }
          }
        }
        peakToPeak = signalMax - signalMin; // Max - min = peak-peak amplitude
        db = map(peakToPeak, 20, 900, 49.5, 90); // Calibrate for deciBels
        if (db <= 55) {
          break; // Exit the motor while loop if sound level drops below threshold
        }
      }
    }
  }
}      
       
