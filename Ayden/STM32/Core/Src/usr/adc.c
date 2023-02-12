/*
 * adc.c
 *
 *  Created on: Feb 12, 2023
 *      Author: Ayden
 */
#include "stm32f1xx_hal.h"
#include "adc.h"

extern ADC_HandleTypeDef hadc1;
extern void Error_Handler(void);

void ADC_Select_CH11(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	/** Configure Regular Channel
	*/
	sConfig.Channel = ADC_CHANNEL_11;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
}

void ADC_Select_CH12(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	/** Configure Regular Channel
	*/
	sConfig.Channel = ADC_CHANNEL_12;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
}

void ADC_Select_CH13(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	/** Configure Regular Channel
	*/
	sConfig.Channel = ADC_CHANNEL_13;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}
}
