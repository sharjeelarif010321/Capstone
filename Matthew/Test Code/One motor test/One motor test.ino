// Connections to A4982
const int dirPin = 2;  // Direction
const int stepPin = 3; // Step
const int NUMBER_OF_ROTATIONS = 3200; // Steps needed for 1 rotation
const int STEPS_PER_REV = 3200; // Motor steps per rotation
const int sampleInterval = 10000; // 10 seconds
const int sampleWindow = 50;                             
unsigned int sample;
unsigned long previousSampleMillis = 0;

// setting up the motor variables
unsigned long motorStartTime = 0;
bool motorRunning = false;

#define SENSOR_PIN A0
 
void setup() {
  pinMode (stepPin, OUTPUT); // Setup the pins as Outputs D3
  pinMode (dirPin, OUTPUT); // Setup the pins as Outputs D2
  pinMode (SENSOR_PIN, INPUT); // Set the signal pin as input A0  
  Serial.begin(9600); // Baud Rate
}

void loop() {
  unsigned long currentMillis = millis(); // current time
  float peakToPeak = 0; // peak-to-peak level

  // Check if it's time to take a sample
  if (currentMillis - previousSampleMillis >= sampleInterval) {
    Serial.println("Taking a sample...");
    previousSampleMillis = currentMillis;   // reset previous time  
    unsigned long startMillis = millis(); // start of sample window                                  
    unsigned int signalMax = 0; // minimum value
    unsigned int signalMin = 1024; // maximum value

    // Collect data for 50 mS
    while (millis() - startMillis < sampleWindow) {
      sample = analogRead(SENSOR_PIN); // get reading from microphone
      if (sample < 1024) {
        if (sample > signalMax) {
          signalMax = sample; // save just the max levels
        }
        else if (sample < signalMin) {
          signalMin = sample; // save just the min levels
        }
      }
    }

    peakToPeak = signalMax - signalMin; // max - min = peak-peak amplitude
    int db = map(peakToPeak, 20, 900, 49.5, 90); // calibrate for deciBels
    Serial.print("Loudness: ");
    Serial.print(db);
    Serial.println(" dB");

    // Check if decibel level is above threshold
    if (db >= 55) {
      motorStartTime = currentMillis; // start motor
      motorRunning = true;
      Serial.println("entered for loop");
      for (int i = 0; i < STEPS_PER_REV ; i++) {
        digitalWrite(dirPin, HIGH); // set motor direction
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(100); // delay between steps
        digitalWrite(stepPin, LOW);
        delayMicroseconds(100);
        if (i == STEPS_PER_REV-1) {
          Serial.println("last loop");       
        }
      }
        Serial.println("exit loop");
      
    }
    else {
      digitalWrite(stepPin, LOW); // stop motor
      digitalWrite(dirPin, LOW);
      motorRunning = false;
    }
  }

  // Check if it's time to stop the motor
  if (motorRunning && currentMillis - motorStartTime >= sampleInterval) {
    Serial.println("Stopping the motor...");
    digitalWrite(stepPin, LOW); // stop motor
    digitalWrite(dirPin, LOW);
    motorRunning = false;
  }
}