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
	buf[0] = reg|(1<<5);
	buf[1] = Data;
	CS_Select();
	HAL_SPI_Transmit(NRF24_SPI, buf, 2, 1000);
	CS_Deselect();
}

void nrf24_WriteRegMulti(uint8_t reg, uint8_t* data, int size)
{
	uint8_t buf[2];
	buf[0] = reg|(1<<5);
	CS_Select();
	HAL_SPI_Transmit(NRF24_SPI, buf, 2, 1000);
	HAL_SPI_Transmit(NRF24_SPI, data, size, 1000);
	CS_Deselect();
}

uint8_t nrf24_ReadReg(uint8_t reg)
{
	uint8_t data = 0;
	CS_Select();
	HAL_SPI_Transmit(NRF24_SPI, &reg, 2, 1000);
	HAL_SPI_Receive(NRF24_SPI, &data, 1, 1000);
	CS_Deselect();
	return data;
}

void nrf24_ReadRegMulti(uint8_t reg, uint8_t* data, int size)
{
	CS_Select();
	HAL_SPI_Transmit(NRF24_SPI, &reg, 2, 1000);
	HAL_SPI_Receive(NRF24_SPI, data, size, 1000);
	CS_Deselect();
}

void nrfSendCmd(uint8_t cmd)
{
	CS_Select();
	HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 1000);
	CS_Deselect();
}

void nrf24_Init(void)
{
	CE_Disable();
	nrf24_WriteReg(CONFIG, 0);
	nrf24_WriteReg(EN_AA, 0);
	nrf24_WriteReg(EN_RXADDR, 0);
	nrf24_WriteReg(SETUP_AW, 0x03);
	nrf24_WriteReg(SETUP_RETR, 0);
	nrf24_WriteReg(RF_CH, 0);
	nrf24_WriteReg(RF_SETUP, 0x26); //Power= 0dB, data rate = 250kbps
	CE_Enable();
}

void nrf24_TxMode(uint8_t* address, uint8_t channel)
{
	CE_Disable();
	nrf24_WriteReg(RF_CH, channel);
	nrf24_WriteRegMulti(TX_ADDR, address, 5);
	uint8_t config = nrf24_ReadReg(CONFIG);
	config = config | (1<<1);
	nrf24_WriteReg(CONFIG, config);
	CE_Enable();
}

uint8_t nrf24_Transmit(uint8_t* data)
{
	uint8_t cmdtosend = 0;
	CS_Select();
	cmdtosend = W_TX_PAYLOAD;
	HAL_SPI_Transmit(NRF24_SPI, &cmdtosend, 1, 100);
	HAL_SPI_Transmit(NRF24_SPI, data, 32, 1000);
	CS_Deselect();
	HAL_Delay(1);
	uint8_t fifostatus = nrf24_ReadReg(FIFO_STATUS);

	if((fifostatus & (1<<4)) && !(fifostatus & (1<<3)))
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
	nrf24_WriteReg(RF_CH, channel);
	uint8_t en_rxaddr = nrf24_ReadReg(EN_RXADDR);
	en_rxaddr |= (1<<1);
	nrf24_WriteReg(EN_RXADDR, en_rxaddr);
	nrf24_WriteRegMulti(RX_ADDR_P1, address, 5);
	nrf24_WriteReg(RX_PW_P1, 32);
	uint8_t config = nrf24_ReadReg(CONFIG);
	config = config | (1<<1) | (1<<0);
	nrf24_WriteReg(CONFIG, config);
	CE_Enable();
}

uint8_t isDataAvailable(int pipenum)
{
	uint8_t status = nrf24_ReadReg(STATUS);
	if((status & (1<<6)) && (status & (pipenum<<1)))
	{
		nrf24_WriteReg(STATUS, (1<<6));
		return 1;
	}
	return 0;
}

void nrf24_Receive(uint8_t* data)
{
	uint8_t cmdtosend = 0;
	CS_Select();
	cmdtosend = R_RX_PAYLOAD;
	HAL_SPI_Transmit(NRF24_SPI, &cmdtosend, 1, 100);
	HAL_SPI_Receive(NRF24_SPI, data, 32, 1000);
	CS_Deselect();
	HAL_Delay(1);
	cmdtosend = FLUSH_RX;
	nrfSendCmd(cmdtosend);
}

void nrf24_reset(uint8_t reg)
{
	if (reg == STATUS)
	{
		nrf24_WriteReg(STATUS, 0x00);
	}
	else if (reg == FIFO_STATUS)
	{
		nrf24_WriteReg(FIFO_STATUS, 0x11);
	}
	else
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
