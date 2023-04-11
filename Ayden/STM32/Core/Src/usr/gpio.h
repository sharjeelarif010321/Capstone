/**
* gpio.h
*
* DESCRIPTION:
* This file contains definitions and prototypes for gpio.c
*
* AUTHOR: Ayden Mack 200405410
*/

#ifndef SRC_USR_GPIO_H_
#define SRC_USR_GPIO_H_

/**
Function: GPIO_config
Purpose: A helper function that reconfigures a specified GPIO pin
Input: GPIO_TypeDef * port - The GPIO port (A, B, C) that is being changed
	   uint32_t pin - The GPIO pin number that is being changed
	   uint32_t mode - The mode that the GPIO pin will be changed to
	   uint32_t pull - Specifies whether or not to use internal pull-up resistors
Output: None
*/
void GPIO_config(GPIO_TypeDef * port, uint32_t pin, uint32_t mode, uint32_t pull);

#endif /* SRC_USR_GPIO_H_ */
