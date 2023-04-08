/**
* wearable.ino
*
* DESCRIPTION:
* This program is responsible for taking heart rate and blood oxygen
* measurements from the DFRobot SEN0344, as well as transmitting the data
* wirelessly to the STM32F103RB using an nRF24L01 module. This file contains
* some example code from:
* https://github.com/DFRobot/DFRobot_BloodOxygen_S
*
* AUTHOR: Ayden Mack 200405410
*/
#include "DFRobot_BloodOxygen_S.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"
#include "printf.h"
#include <stdio.h>
#include <SoftwareSerial.h>

#define ARDUINO_AVR_UNO                                   //This line necessary
                                                          //to force use of SW
                                                          //UART in the DFRobot
                                                          //library.
#ifdef  I2C_COMMUNICATION
#define I2C_ADDRESS    0x57
  DFRobot_BloodOxygen_S_I2C MAX30102(&Wire ,I2C_ADDRESS);
#else
/* HARDWARE CONNECTION TABLE
* ---------------------------------------------------------
*   SEN0344  |              MCU               |   NANO    |
*     VCC    |              3.3V              |    VCC    |
*     GND    |              GND               |    GND    |
*     RX     |              TX                |     3     |
*     TX     |              RX                |     2     |
* ---------------------------------------------------------
*/

//Use serial communcation between Arduino Nano and SEN0344
#if defined(ARDUINO_AVR_UNO) || defined(ESP8266)
SoftwareSerial mySerial(2, 3);
DFRobot_BloodOxygen_S_SoftWareUart MAX30102(&mySerial, 9600);
#else
DFRobot_BloodOxygen_S_HardWareUart MAX30102(&Serial, 9600); 
#endif
#endif

//Initialize data buffers
char spo2[] = "\0\0\0\0\0\0\0";
char pulse[] = "\0\0\0";
char invalid_data[] = "000 000";

//Specify pins used for nRF24L01
RF24 radio(9,10);

//Specify Tx address used for nRF24L01
uint8_t address[] = {0xE6,0xE6,0xE6,0xE6,0xE6};

//Various flags and values
uint8_t clk_divider = 1;
uint8_t spo2_flag = 0;
uint8_t pulse_flag = 0;
uint8_t count = 0;
void setup()
{
  //Initialize TPL5111 control pin
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

  //Initialize nRF24L01 module with same parameters as programmed in
  //STM32F103RB's code
  radio.begin();
  printf_begin();
  radio.setAddressWidth(5);
  radio.setChannel(10);
  radio.setPayloadSize(32);
  radio.disableAckPayload();
  radio.disableDynamicPayloads();
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.disableCRC();
  radio.stopListening();

  //Initialize serial comms for debug when connected to PC
  Serial.begin(115200*clk_divider);

  //Attempt to initilalize the SEN0344 sensor forever
  while (false == MAX30102.begin())
  {
    Serial.println("init fail!");
    delay(1000/clk_divider);
  }
  Serial.println("init success!");
  Serial.println("start measuring...");

  //Begin taking heart rate/SpO2 measurement
  MAX30102.sensorStartCollect();

  //Open wireless comms on the specified address
  radio.openWritingPipe(address);
}

void loop()
{
  //Retrieve heart rate and SpO2 measurement from SEN0344
  MAX30102.getHeartbeatSPO2();

  //If either value is invalid, keep attempting to take a measurement
  while((MAX30102._sHeartbeatSPO2.SPO2 == -1) || (MAX30102._sHeartbeatSPO2.Heartbeat == -1))
  {
    //If SpO2 value is valid, save it
    if((MAX30102._sHeartbeatSPO2.SPO2 != -1) && (spo2_flag == 0))
    {
      itoa(MAX30102._sHeartbeatSPO2.SPO2, spo2, 10);
      spo2_flag = 1;
    }
    //If heart rate value is valid, save it
    if((MAX30102._sHeartbeatSPO2.Heartbeat != -1) && (pulse_flag == 0))
    {
      itoa(MAX30102._sHeartbeatSPO2.Heartbeat, pulse, 10);
      pulse_flag = 1;
    }

    //Wait 4 seconds, then attempt to get measurements again
    delay(4000/clk_divider);
    MAX30102.getHeartbeatSPO2();

    count++;

    //If 6 attempts have been made, or valid values for heart rate and SpO2
    //have been saved, break out of the loop
    if((count == 6) || ((spo2_flag == 1) && (pulse_flag == 1)))
    {
      if (((spo2_flag == 1) && (pulse_flag == 1)))
      {
        count--;
      }
      break;
    }
  }

  //If 6 attempts were made and the values were still not valid, transmit an
  //"invalid data" string
  if(count == 6)
  {
    count = 0;
    // Serial.println("FAIL");
    radio.powerUp();
    delay(5);
    radio.write(&invalid_data,sizeof(invalid_data));
  }
  
  //If valid data is saved, organize it for transmission
  else
  {
    count = 0;
    if((spo2_flag == 0) || (pulse_flag == 0))
    {
      itoa(MAX30102._sHeartbeatSPO2.SPO2, spo2, 10);
      itoa(MAX30102._sHeartbeatSPO2.Heartbeat, pulse, 10);
    }

    //Add leading 0s if SpO2 value is less than 3 digits
    while (strlen(spo2) < 3)
    {
      char padded[7] = "0";
      strcat(padded, spo2);
      strcpy(spo2, padded);
    }

    //Add leading 0s if pulse value is less than 3 digits
    while (strlen(pulse) < 3)
    {
      char padded[3] = "0";
      strcat(padded, pulse);
      strcpy(pulse, padded);
    }
    
    //Add data to a single string
    strcat(spo2," ");
    strcat(spo2, pulse);

    //Transmit data
    radio.powerUp();
    delay(5);
    radio.write(&spo2,sizeof(spo2));
  }
  delay(5);
  radio.powerDown();
  
  //Clear all flags and data buffers
  spo2_flag = 0;
  pulse_flag = 0;
  for(uint8_t i = 0; i < 7; i++)
  {
    spo2[i] = '\0';
  }
  for(uint8_t i = 0; i < 3; i++)
  {
    pulse[i] = '\0';
  }
  MAX30102.sensorEndCollect();

  //Signal the TPL5111 to power down
  digitalWrite(7, HIGH);
}
