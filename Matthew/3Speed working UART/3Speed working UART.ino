// Connections to A4982
const int dirPin = 2;  // Direction
const int stepPin = 3; // Step
const int NUMBER_OF_ROTATIONS = 15; // How long the motor rotates for
char noise = '0';

// Sample window width in mS (50 mS = 20Hz)
const int sampleInterval = 5000; // 5 seconds
const int sampleWindow = 50;  //How long the microphone samples for                            
unsigned int sample; // value of the sample 
unsigned long previousSampleMillis = 0;
unsigned long previousTransmitMillis = 0;
const unsigned long TRANSMIT_INTERVAL = 2500; // 2.5 seconds

// setting up the motor variables
int motorSpeed = 1;
int setSpeed;

#define SENSOR_PIN A0 //Analog out pin on the ky038

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
      
      long totalSteps = NUMBER_OF_ROTATIONS; // resets how long the motor runs for after every loop 
      Serial.println(totalSteps);
      if(db >= 55 && db <= 64){
        setSpeed = 200; // low speed
        noise = '1';
      } else if (db >= 65 && db <= 74){
        setSpeed = 150; //medium speed 
        noise = '2';
      } else if(db >= 75){
        setSpeed = 100; // fasteste spee 
        noise = '3';
      }
      
      // Transmit noise character over UART if it's been more than 2.5 seconds since last transmission
      if (currentMillis - previousTransmitMillis >= TRANSMIT_INTERVAL) {
        Serial.println(noise);
        previousTransmitMillis = currentMillis;
      }
        // code to run the motor based on the values above
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
        }
      }

      digitalWrite(dirPin, LOW); // Reset motor direction
      setSpeed = 0; // resests motor speed to zero
      noise = '0';  // resets noise to zero 
      Serial.println(noise);      
      Serial.println("Motor stopped.");
    }
  }
}



