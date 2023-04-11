// Define Constants
 
// Connections to A4982
const int dirPin = 2;  // Direction
const int stepPin = 3; // Step
const int NUMBER_OF_ROTATIONS = 45;
 
// Motor steps per rotation
const int STEPS_PER_REV = 3200;
 
void setup() {
  
  // Setup the pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);

}
void loop() {
  int currentRotation = 0;
  // Set motor direction clockwise
  while (true) {
    digitalWrite(dirPin,HIGH); 
     
      // Spin motor one rotation slowly
    for(int x = 0; x < STEPS_PER_REV; x++) {
      digitalWrite(stepPin,HIGH); 
     
     
      if (currentRotation <= 15) {
        delayMicroseconds(200); // Slow speed for first 5 rotations      
      } else if (currentRotation > 16 && currentRotation <= 30) {
        delayMicroseconds(150); // Faster speed for remaining rotations
      } else {
        delayMicroseconds(100);
      }
      
      digitalWrite(stepPin,LOW); 
       // Set delay time based on current rotation count
      if (currentRotation < 15) {
        delayMicroseconds(200); // Slow speed for first 5 rotations
      } else if (currentRotation > 16 && currentRotation <= 30) {
        delayMicroseconds(150); // Faster speed for remaining rotations
      } else {
        delayMicroseconds(100); // Faster speed for remaining rotations
      }
    } 
  
   currentRotation++;
   
   if(currentRotation >= NUMBER_OF_ROTATIONS) {
      break;     
   }
  }
  }


