// Connections to A4982
const int dirPin = 2;  // Direction
const int stepPin = 3; // Step
const int NUMBER_OF_ROTATIONS = 15; // How long the motor rotates for

// Motor steps per rotation
const int STEPS_PER_REV = 3200;

// Sample window width in mS (50 mS = 20Hz)
const int sampleInterval = 10000; // 10 seconds
const int sampleWindow = 50;                             
unsigned int sample;
unsigned long previousSampleMillis = 0;

// setting up the motor variables
int motorSpeed = 1;
unsigned long motorStartTime = 0;
unsigned long motorDuration = 0;

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
  int currentRotation = 0;

  // Check if it's time to take a sample
  if (currentMillis - previousSampleMillis >= sampleInterval) {
    
    Serial.println("Taking a sample...");
    previousSampleMillis = currentMillis;   // reset previous time  
    motorStartTime = currentMillis; // start motor
    digitalWrite(dirPin,HIGH); 
    for (currentRotation < 15){
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(200);
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(200);
    }
    currentRotation++;
   
   if(currentRotation >= NUMBER_OF_ROTATIONS) {
      break;  
  }

  // Check if it's time to stop the motor
  if (currentMillis - motorStartTime >= sampleInterval) {
    Serial.println("Stopping the motor...");
    digitalWrite(stepPin, LOW);
    digitalWrite(dirPin, LOW);
  }

  // Collect data for 50 mS while the motor is running
  if (digitalRead(stepPin) == HIGH) {
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
    int db = map(peakToPeak,20,900,49.5,90); // calibrate for deciBels
    Serial.print("Loudness: ");
    Serial.print(db);
    Serial.println(" dB");
    
  }
}
