/**
* nRF24L01.h
*
* DESCRIPTION:
* This file contains definitions and prototypes for nRF24L01.c. Since there is
* no library available for use of nRF24L01 modules on STM32, it is based on code
* and a tutorial found here: https://github.com/controllerstech/NRF24L01.
* The tutorial above is based on the datasheet of the nRF24L01 module.
*
* AUTHOR: Ayden Mack 200405410
*/

#ifndef SRC_USR_NRF24L01_H_
#define SRC_USR_NRF24L01_H_

/* Memory Map */
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD	    0x1C
#define FEATURE	    0x1D

/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF

/**
Function: nrf24_Init
Purpose: This function initializes the nRF24L01 with default values, and
		 specifies config options such as disabling Auto ACK, setting a 5-byte
		 address format, disabling data retransmission, and setting transmission
		 power and datarate
Input: None
Output: None
*/
void nrf24_Init(void);

/**
Function: nrf24_TxMode
Purpose: This function configures the nRF24L01 in Tx Mode with the specified
		 address and channel
Input: uint8_t *address - a pointer to the array containing the Tx address
	   uint8_t channel - the frequency channel the nRF24L01 will transmit on
Output: None
*/
void nrf24_TxMode(uint8_t *address, uint8_t channel);

/**
Function: nrf24_Transmit
Purpose: This function transmits the specified data
Input: uint8_t *data - a pointer to the array containing the data to be
					   transmitted
Output: uint8_t - a 1 if the transmission was successful, and a 0 if it failed
*/
uint8_t nrf24_Transmit(uint8_t *data);

/**
Function: nrf24_RxMode
Purpose: This function configures the nRF24L01 in Rx Mode with the specified
		 data pipe, address, and channel
Input: uint8_t *address - a pointer to the array containing the Rx address
	   uint8_t channel - the frequency channel the nRF24L01 will receive on
Output: None
*/
void nrf24_RxMode(uint8_t* address, uint8_t channel);

/**
Function: isDataAvailable
Purpose: A helper function that reconfigures a specified GPIO pin
Input: int pipenum - the data pipe to be checked for incoming data
Output: uint8_t - a 1 if data is available, and a 0 if not
*/
uint8_t isDataAvailable(int pipenum);

/**
Function: nrf24_Receive
Purpose: This function receives data and stores it in the data buffer
Input: uint8_t* data - the data buffer that the received data will be stored in
Output: None
*/
void nrf24_Receive(uint8_t* data);

/**
Function: nrf24_reset
Purpose: A helper function to reset a specific register, or all registers of the
		 nRF24L01
Input: uint8_t reg - the register to be reset
Output: None
*/
void nrf24_reset(uint8_t reg);

#endif /* SRC_USR_NRF24L01_H_ */
