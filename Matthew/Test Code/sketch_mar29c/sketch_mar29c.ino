const int dirPin = 2;  // Direction
const int stepPin = 3; // Step
const int STEPS_PER_REV = 3200; // Motor steps per rotation
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
volatile bool soundSampleNeeded = false;
int db = 0;

#define SENSOR_PIN A0

void setup() {
  pinMode(stepPin, OUTPUT); // Setup the pins as Outputs
  pinMode(dirPin, OUTPUT); // Setup the pins as Outputs
  pinMode(SENSOR_PIN, INPUT); // Set the signal pin as input
  Serial.begin(9600); // Baud Rate
  
  // Configure Timer1 to trigger an interrupt every 5 seconds
  cli(); // Disable interrupts while configuring the timer
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 31249; // Set the compare match register (5-second interval)
  TCCR1B |= (1 << WGM12); // Configure timer for CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // Set prescaler to 1024
  TIMSK1 |= (1 << OCIE1A); // Enable the compare match interrupt
  sei(); // Re-enable interrupts
}

void loop() {
  static bool motorRunning = false;

  if (soundSampleNeeded) {
    soundSampleNeeded = false;
    
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
    
    if (motorRunning && db <= 55) {
      motorRunning = false; // Set flag to indicate motor is not running
    }
    
    if (motorRunning) {
      // Rotate the motor
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(200);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(200);
    }
  }

  // Check if sound level is above threshold and motor is not already running
  if (db > 55 && !motorRunning) { // Threshold check
motorRunning = true; // Set flag to indicate motor is running
digitalWrite(dirPin, HIGH); // Set direction
}
}

// Timer1 Compare Match Interrupt Service Routine
ISR(TIMER1_COMPA_vect) {
soundSampleNeeded = true; // Set flag to trigger sound level sampling
}