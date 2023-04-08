// Connections to A4982
const int dirPin = 2;  // Direction
const int stepPin = 3; // Step
const int NUMBER_OF_ROTATIONS = 15; // How long the motor rotates for
const int STEPS_PER_REV = 3200;
char noise;

// Sample window width in mS (50 mS = 20Hz)
const int sampleInterval = 5000; // 5 seconds
const int sampleWindow = 50;    // how long samples are taken for                           
unsigned int sample;
unsigned long previousSampleMillis = 0;

// setting up the motor variables
int motorSpeed = 1;
int setSpeed;

#define SENSOR_PIN A0 // takes output from A0 pin on Ky038 

// Threshold dB value for motor activation
const int THRESHOLD_DB = 55;

// Duration for motor to run after threshold is reached (in milliseconds)
const unsigned long MOTOR_RUN_DURATION = 5000;

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
  if (currentMillis - previousSampleMillis > sampleInterval) {
    Serial.println("Taking a sample...");
    previousSampleMillis = currentMillis;   // reset previous time  

    // Collect data for 50 mS while the motor is running
    unsigned long startMillis = millis(); // start of sample window                                  
    unsigned int signalMax = 0; // minimum value
    unsigned int signalMin = 1024; // maximum value

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
    int db = map(peakToPeak,20,900,49.5, 90); // calibrate for deciBels
    Serial.print("Loudness: ");
    Serial.print(db);
    Serial.println(" dB");

    if (db >= THRESHOLD_DB) {
      // Run motor for 5 seconds
      Serial.println("Running motor...");
      digitalWrite(dirPin, HIGH); // Set motor direction
      unsigned long motorStartTime = millis();
      unsigned long motorCurrentTime;
      
      long totalSteps = NUMBER_OF_ROTATIONS;
      Serial.println(totalSteps);
      if(db >= 55 && db <= 64){
        setSpeed = 200; 
        noise = '1';
        Serial.println(noise);
      } else if (db >= 65 && db <= 74){
        setSpeed = 150; 
        noise = '2';
        Serial.println(noise);
      } else if(db >= 75){
        setSpeed = 100;
        noise = '3';
        Serial.println(noise);
      }
      
      //This code runs the motor
      while (millis() - motorStartTime < MOTOR_RUN_DURATION) {
        motorCurrentTime = millis();
        for (int i = 0; i < totalSteps; i++) {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(setSpeed);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(setSpeed);

          // Break out of the loop if the motor has run for the specified duration
          if (millis() - motorStartTime >= MOTOR_RUN_DURATION) {
            break;
          }
                    if (millis() - motorStartTime >= MOTOR_RUN_DURATION) {
            break;
          }
        }
      }

      digitalWrite(dirPin, LOW); // Reset motor direction
      setSpeed = 0;
      noise = '0'; 
      Serial.println(noise);      
      Serial.println("Motor stopped.");
    }
  }
}

