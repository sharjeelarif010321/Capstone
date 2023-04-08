/**
* gpio.c
*
* DESCRIPTION:
* This file contains a helper function to quickly re-configure a single GPIO pin
* on the STM32F103RB board.
*
* AUTHOR: Ayden Mack 200405410
*/

#include "stm32f1xx_hal.h"
#include "gpio.h"

void GPIO_config(GPIO_TypeDef * port, uint32_t pin, uint32_t mode, uint32_t pull)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = mode;
	GPIO_InitStruct.Pull = pull;
	if(mode == GPIO_MODE_OUTPUT_PP)
	{
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	}
	HAL_GPIO_Init(port, &GPIO_InitStruct);
}
