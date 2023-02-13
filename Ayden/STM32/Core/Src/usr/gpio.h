/*
 * gpio.h
 *
 *  Created on: Feb 12, 2023
 *      Author: Ayden
 */

#ifndef SRC_USR_GPIO_H_
#define SRC_USR_GPIO_H_

void GPIO_config(GPIO_TypeDef * port, uint32_t pin, uint32_t mode, uint32_t pull);

#endif /* SRC_USR_GPIO_H_ */
