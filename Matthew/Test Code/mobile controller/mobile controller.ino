// Connections to A4982
const int dirPin = 2;  // Direction
const int stepPin = 3; // Step
const int NUMBER_OF_ROTATIONS = 15; //How long the motor rotates for

// Motor steps per rotation
const int STEPS_PER_REV = 3200;
 
// Sample window width in mS (50 mS = 20Hz)
const int sampleWindow = 50;                             
unsigned int sample;
unsigned long previousMillis = 0;
const long interval = 10000; // 10 seconds
 
// setting up the motor variables
int motorSpeed = 1;
unsigned long motorStartTime = 0;
unsigned long motorDuration = 0; 

#define SENSOR_PIN A0
 
void setup() {
  pinMode (stepPin, OUTPUT); // Setup the pins as Outputs D3
  pinMode (dirPin, OUTPUT); // Setup the pins as Outputs D2
  pinMode (SENSOR_PIN, INPUT); // Set the signal pin as input A0  
  Serial.begin(9600); //Baud Rate
}

void loop() {
  unsigned long currentMillis = millis(); // current time
  static int currentRotation = 0; //the initial number of rotations of the motor
  float peakToPeak = 0;                                  // peak-to-peak level
   
  if (currentMillis - previousMillis >= interval) {
    Serial.println("this is waiting for 10 seconds");
    previousMillis = currentMillis;   // reset previous time  
    motorDuration = currentMillis - motorStartTime; // calculate motor duration
    currentRotation = 0; // reset current rotation count
    motorDuration = 0;
   unsigned long startMillis= millis();                   // Start of sample window                                 
 
   unsigned int signalMax = 0;                            //minimum value
   unsigned int signalMin = 1024;                         //maximum value
 
                                                          // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(SENSOR_PIN);                    //get reading from microphone
      if (sample < 1024)                                  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;                           // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;                           // save just the min levels
         }
      }
   }
 
  peakToPeak = signalMax - signalMin;                    // max - min = peak-peak amplitude
  int db = map(peakToPeak,20,900,49.5,90);             //calibrate for deciBels
  Serial.print("Loudness: ");
  Serial.print(db);
  Serial.println(" dB");

  
  digitalWrite(dirPin,HIGH); 

  if (currentRotation <= NUMBER_OF_ROTATIONS ) {
  digitalWrite(stepPin,HIGH); 
  
    if (db >= 55 && db <= 64){
      delayMicroseconds(200); // Slow speed for first 5 rotations
      Serial.println ("low speed running");
      previousMillis = currentMillis;  
    } else if(db >= 65 && db <= 74){
      delayMicroseconds(150); // Slow speed for first 5 rotations
      Serial.println ("medium speed running");
      previousMillis = currentMillis;  
    } else if (db >= 75) {
      delayMicroseconds(100);
      Serial.println ("high speed running");
      previousMillis = currentMillis;  
    } else {
      digitalWrite(stepPin,LOW);
    }
    

    digitalWrite(stepPin, LOW);
    if (db >= 55 && db <= 64){
     delayMicroseconds(200); // Slow speed for first 5 rotations
     previousMillis = currentMillis;  
    } else if(db >= 65 && db <= 74){
      delayMicroseconds(150); // Slow speed for first 5 rotations
      previousMillis = currentMillis;  
    } else if (db >= 75) {
      delayMicroseconds(100);
      previousMillis = currentMillis;  
    } else {
    digitalWrite(stepPin, LOW);
    }
     currentRotation++;
    motorDuration = currentMillis - motorStartTime; // update motor duration
  }
  
  if (currentRotation > NUMBER_OF_ROTATIONS || motorDuration > interval) { // add motor duration check
    digitalWrite(stepPin,LOW); // stop motor
  }
}
}
