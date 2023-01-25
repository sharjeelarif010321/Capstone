/*!
 * @file  gainHeartbeatSPO2.ino
 * @n experiment phenomena: get the heart rate and blood oxygenation, during the update the data obtained does not change
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author      PengKaixing(kaixing.peng@dfrobot.com)
 * @version     V1.0.0
 * @date        2021-06-21
 * @url         https://github.com/DFRobot/DFRobot_BloodOxygen_S
 */
#include "DFRobot_BloodOxygen_S.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"
#include "printf.h"
#include <stdio.h>
#include <LowPower.h>

#define I2C_COMMUNICATION  //use I2C for communication, but use the serial port for communication if the line of codes were masked

#ifdef  I2C_COMMUNICATION
#define I2C_ADDRESS    0x57
  DFRobot_BloodOxygen_S_I2C MAX30102(&Wire ,I2C_ADDRESS);
#else
/* ---------------------------------------------------------------------------------------------------------------
 *    board   |             MCU                | Leonardo/Mega2560/M0 |    UNO    | ESP8266 | ESP32 |  microbit  |
 *     VCC    |            3.3V/5V             |        VCC           |    VCC    |   VCC   |  VCC  |     X      |
 *     GND    |              GND               |        GND           |    GND    |   GND   |  GND  |     X      |
 *     RX     |              TX                |     Serial1 TX1      |     5     |   5/D6  |  D2   |     X      |
 *     TX     |              RX                |     Serial1 RX1      |     4     |   4/D7  |  D3   |     X      |
 * ---------------------------------------------------------------------------------------------------------------*/
#if defined(ARDUINO_AVR_UNO) || defined(ESP8266)
SoftwareSerial mySerial(4, 5);
DFRobot_BloodOxygen_S_SoftWareUart MAX30102(&mySerial, 9600);
#else
DFRobot_BloodOxygen_S_HardWareUart MAX30102(&Serial1, 9600); 
#endif
#endif
char spo2[] = "\0\0\0\0\0\0\0";
char pulse[] = "\0\0\0";
RF24 radio(9,10);
uint8_t address[] = {0xE6,0xE6,0xE6,0xE6,0xE6};
void setup()
{
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
  Serial.begin(115200);
  while (false == MAX30102.begin())
  {
    Serial.println("init fail!");
    delay(1000);
  }
  Serial.println("init success!");
  Serial.println("start measuring...");
  MAX30102.sensorStartCollect();
  radio.openWritingPipe(address);
}

void loop()
{
  MAX30102.getHeartbeatSPO2();
  while((MAX30102._sHeartbeatSPO2.SPO2 == -1) || (MAX30102._sHeartbeatSPO2.Heartbeat == -1))
  {
    MAX30102.getHeartbeatSPO2();
    delay(4000);
  }
  Serial.print("SPO2 is : ");
  Serial.print(MAX30102._sHeartbeatSPO2.SPO2);
  Serial.println("%");
  Serial.print("heart rate is : ");
  Serial.print(MAX30102._sHeartbeatSPO2.Heartbeat);
  Serial.println("Times/min");
  Serial.print("Temperature value of the board is : ");
  Serial.print(MAX30102.getTemperature_C());
  Serial.println(" ℃");
  itoa(MAX30102._sHeartbeatSPO2.SPO2, spo2, 10);
  itoa(MAX30102._sHeartbeatSPO2.Heartbeat, pulse, 10);
  strcat(spo2," ");
  strcat(spo2, pulse);
  printf(spo2);
  printf("\n");
  radio.write(&spo2,sizeof(spo2));
  delay(50);
  for(uint8_t i = 0; i < 7; i++)
  {
    spo2[i] = '\0';
  }
  MAX30102.sensorEndCollect();
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
                 SPI_OFF, USART0_OFF, TWI_OFF);
  delay(50);
  MAX30102.sensorStartCollect();
  delay(4000);
}