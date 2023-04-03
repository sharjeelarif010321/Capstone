/**
* nRF24L01.c
*
* DESCRIPTION:
* This file contains helper functions for the use of the nRF24L01 wireless
* communication modules with the STM32 board. Since there is no library
* available for use of nRF24L01 modules on STM32, it is based on code and a
* tutorial found here: https://github.com/controllerstech/NRF24L01.
* The tutorial above is based on the datasheet of the nRF24L01 module.
*
* AUTHOR: Ayden Mack 200405410
*/

#include "stm32f1xx_hal.h"
#include "nRF24L01.h"

extern  SPI_HandleTypeDef hspi1;
#define NRF24_SPI &hspi1

#define NRF24_CE_PORT GPIOA
#define NRF24_CE_PIN GPIO_PIN_3

#define NRF24_CSN_PORT GPIOA
#define NRF24_CSN_PIN GPIO_PIN_4

void CS_Select(void)
{
	HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_RESET);
}

void CS_Deselect(void)
{
	HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_SET);
}

void CE_Enable(void)
{
	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_SET);
}

void CE_Disable(void)
{
	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_RESET);
}

void nrf24_WriteReg(uint8_t reg, uint8_t Data)
{
	uint8_t buf[2];
	buf[0] = reg|(1<<5);								//5th bit must be 1 according to datasheet
														//bits 0-4 contain register address
	buf[1] = Data;
	CS_Select();
	HAL_SPI_Transmit(NRF24_SPI, buf, 2, 1000);			//Write the data to the register
	CS_Deselect();
}

void nrf24_WriteRegMulti(uint8_t reg, uint8_t* data, int size)
{
	uint8_t buf[2];
	buf[0] = reg|(1<<5);								//5th bit must be 1 according to datasheet
														//bits 0-4 contain register address
	CS_Select();
	HAL_SPI_Transmit(NRF24_SPI, buf, 2, 1000);			//Specify address
	HAL_SPI_Transmit(NRF24_SPI, data, size, 1000);		//Write data
	CS_Deselect();
}

uint8_t nrf24_ReadReg(uint8_t reg)
{
	uint8_t data = 0;
	CS_Select();
	HAL_SPI_Transmit(NRF24_SPI, &reg, 2, 1000);			//Specify register to read from
	HAL_SPI_Receive(NRF24_SPI, &data, 1, 1000);			//Read one byte of data from register
	CS_Deselect();
	return data;
}

void nrf24_ReadRegMulti(uint8_t reg, uint8_t* data, int size)
{
	CS_Select();
	HAL_SPI_Transmit(NRF24_SPI, &reg, 2, 1000);			//Specify register to read from
	HAL_SPI_Receive(NRF24_SPI, data, size, 1000);		//Read "size" bytes of data
	CS_Deselect();
}

void nrfSendCmd(uint8_t cmd)
{
	CS_Select();
	HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 1000);			//Send command to nRF24L01
	CS_Deselect();
}

void nrf24_Init(void)
{
	CE_Disable();
	nrf24_WriteReg(CONFIG, 0);							//Write 0s to config register
	nrf24_WriteReg(EN_AA, 0);							//Disable Auto ACK
	nrf24_WriteReg(EN_RXADDR, 0);						//Don't configure data pipe yet
	nrf24_WriteReg(SETUP_AW, 0x03);						//Use 5-byte address format
	nrf24_WriteReg(SETUP_RETR, 0);						//Don't retransmit data
	nrf24_WriteReg(RF_CH, 0);							//Don't configure channel yet
	nrf24_WriteReg(RF_SETUP, 0x26); 					//Power = 0dBm (max), datarate = 250kbps
	CE_Enable();
}

void nrf24_TxMode(uint8_t* address, uint8_t channel)
{
	CE_Disable();
	nrf24_WriteReg(RF_CH, channel);						//Configure channel number
	nrf24_WriteRegMulti(TX_ADDR, address, 5);			//Configure 5-byte Tx address
	uint8_t config = nrf24_ReadReg(CONFIG);
	config = config | (1<<1);							//Power up nRF24L01
	nrf24_WriteReg(CONFIG, config);
	CE_Enable();
}

uint8_t nrf24_Transmit(uint8_t* data)
{
	uint8_t cmdtosend = 0;
	CS_Select();
	cmdtosend = W_TX_PAYLOAD;
	HAL_SPI_Transmit(NRF24_SPI, &cmdtosend, 1, 100);	//Send "write Tx payload" command
	HAL_SPI_Transmit(NRF24_SPI, data, 32, 1000);		//Send Tx payload itself
	CS_Deselect();
	HAL_Delay(1);
	uint8_t fifostatus = nrf24_ReadReg(FIFO_STATUS);

	if((fifostatus & (1<<4)) && !(fifostatus & (1<<3)))	//Check FIFO_STATUS register to
														//make sure transmission was
														//successful
	{
		cmdtosend = FLUSH_TX;
		nrfSendCmd(cmdtosend);
		return 1;
	}
	return 0;
}

void nrf24_RxMode(uint8_t* address, uint8_t channel)
{
	CE_Disable();
	nrf24_reset(STATUS);
	nrf24_WriteReg(RF_CH, channel);						//Configure channel number
	uint8_t en_rxaddr = nrf24_ReadReg(EN_RXADDR);
	en_rxaddr |= (1<<1);
	nrf24_WriteReg(EN_RXADDR, en_rxaddr);				//Enable data pipe 1
	nrf24_WriteRegMulti(RX_ADDR_P1, address, 5);		//Configure 5-byte Rx address
	nrf24_WriteReg(RX_PW_P1, 32);						//Configure payload size
	uint8_t config = nrf24_ReadReg(CONFIG);
	config = config | (1<<1) | (1<<0);					//Power up device and set Rx mode
	nrf24_WriteReg(CONFIG, config);
	CE_Enable();
}

uint8_t isDataAvailable(int pipenum)
{
	uint8_t status = nrf24_ReadReg(STATUS);
	if((status & (1<<6)) && (status & (pipenum<<1)))	//Check if data is waiting in pipe 1
	{
		nrf24_WriteReg(STATUS, (1<<6));					//Clear "Data Ready" bit
		return 1;
	}
	return 0;
}

void nrf24_Receive(uint8_t* data)
{
	uint8_t cmdtosend = 0;
	CS_Select();
	cmdtosend = R_RX_PAYLOAD;
	HAL_SPI_Transmit(NRF24_SPI, &cmdtosend, 1, 100);	//Send "read Rx payload" command
	HAL_SPI_Receive(NRF24_SPI, data, 32, 1000);			//Read Rx payload itself
	CS_Deselect();
	HAL_Delay(1);
	cmdtosend = FLUSH_RX;
	nrfSendCmd(cmdtosend);
}

void nrf24_reset(uint8_t reg)
{
	if (reg == STATUS)									//Reset STATUS Register
	{
		nrf24_WriteReg(STATUS, 0x00);
	}
	else if (reg == FIFO_STATUS)						//Reset FIFO_STATUS Register
	{
		nrf24_WriteReg(FIFO_STATUS, 0x11);
	}
	else												//Reset all registers
	{
		nrf24_WriteReg(CONFIG, 0x08);
		nrf24_WriteReg(EN_AA, 0x3F);
		nrf24_WriteReg(EN_RXADDR, 0x03);
		nrf24_WriteReg(SETUP_AW, 0x03);
		nrf24_WriteReg(SETUP_RETR, 0x03);
		nrf24_WriteReg(RF_CH, 0x02);
		nrf24_WriteReg(RF_SETUP, 0x0E);
		nrf24_WriteReg(STATUS, 0x00);
		nrf24_WriteReg(OBSERVE_TX, 0x00);
		nrf24_WriteReg(CD, 0x00);
		uint8_t rx_addr_p0_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
		nrf24_WriteRegMulti(RX_ADDR_P0, rx_addr_p0_def, 5);
		uint8_t rx_addr_p1_def[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
		nrf24_WriteRegMulti(RX_ADDR_P1, rx_addr_p1_def, 5);
		nrf24_WriteReg(RX_ADDR_P2, 0xC3);
		nrf24_WriteReg(RX_ADDR_P3, 0xC4);
		nrf24_WriteReg(RX_ADDR_P4, 0xC5);
		nrf24_WriteReg(RX_ADDR_P5, 0xC6);
		uint8_t tx_addr_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
		nrf24_WriteRegMulti(TX_ADDR, tx_addr_def, 5);
		nrf24_WriteReg(RX_PW_P0, 0);
		nrf24_WriteReg(RX_PW_P1, 0);
		nrf24_WriteReg(RX_PW_P2, 0);
		nrf24_WriteReg(RX_PW_P3, 0);
		nrf24_WriteReg(RX_PW_P4, 0);
		nrf24_WriteReg(RX_PW_P5, 0);
		nrf24_WriteReg(FIFO_STATUS, 0x11);
		nrf24_WriteReg(DYNPD, 0);
		nrf24_WriteReg(FEATURE, 0);
	}
}
