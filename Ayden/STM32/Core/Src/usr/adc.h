/**
* adc.h
*
* DESCRIPTION:
* This file contains definitions and prototypes for adc.c
*
* AUTHOR: Ayden Mack 200405410
*/

#ifndef SRC_USR_ADC_H_
#define SRC_USR_ADC_H_

/**
Function: ADC_Select_CH11
Purpose: A helper function that selects ADC1 CH11 (Column 1 of pressure mat)
Input: None
Output: None
*/
void ADC_Select_CH11(void);

/**
Function: ADC_Select_CH12
Purpose: A helper function that selects ADC1 CH12 (Column 2 of pressure mat)
Input: None
Output: None
*/
void ADC_Select_CH12(void);

/**
Function: ADC_Select_CH13
Purpose: A helper function that selects ADC1 CH13 (Column 3 of pressure mat)
Input: None
Output: None
*/
void ADC_Select_CH13(void);

/**
Function: ADC_Select_CH14
Purpose: A helper function that selects ADC1 CH14 (Column 4 of pressure mat)
Input: None
Output: None
*/
void ADC_Select_CH14(void);

#endif /* SRC_USR_ADC_H_ */




