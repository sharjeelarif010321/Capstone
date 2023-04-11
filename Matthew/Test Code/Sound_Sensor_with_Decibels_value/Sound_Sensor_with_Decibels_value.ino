const int sampleWindow = 50;                              // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

unsigned long previousMillis = 0;
const long interval = 10000; // 10 seconds
 
#define SENSOR_PIN A0
 
void setup ()  
{   
  pinMode (SENSOR_PIN, INPUT); // Set the signal pin as input  
  
  Serial.begin(9600); // Baud Rate
}  
 
   
void loop ()  
{ 
  unsigned long currentMillis = millis(); // current time
  if (currentMillis - previousMillis >= interval) {
    Serial.println("this is waiting for 10 seconds");
    previousMillis = currentMillis;   // reset previous time  
  
  unsigned long currentMillis = millis(); // current time
  unsigned long startMillis= millis();                   // Start of sample window
  float peakToPeak = 0;                                  // peak-to-peak level
 
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
  
  if(db >= 55 && db <= 65) {
    Serial.println("Low threshold, waiting 10 seconds...");
    previousMillis = currentMillis;  
  } else if(db >= 66 && db <= 75) {
    Serial.println("medium threshold , waiting 10 seconds...");
    previousMillis = currentMillis;  
  } else if(db >= 76) {
    Serial.println("loud threshold, waiting 10 seconds...");
    previousMillis = currentMillis;  
  } else {
    Serial.println("No threshold reached");
  }
   
}
}
