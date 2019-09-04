//LED.h - interface to flash an LED
//Justin Turcotte
//January 10, 2018

#ifndef LED_H
#define LED_H

#include "stm32f303xe.h"

#define DELAY_COUNT 1000000UL

#define LED_PORT	A
#define LED_PIN		5

void LED_Init(GPIO_TypeDef* PORT, uint32_t PIN);										//Initializes GPIOB or GPIOE to use the LED 
																																		//(will probably work on other ports with a LED)
																																		
void Toggle_PIN(GPIO_TypeDef* PORT, uint32_t PIN, uint32_t DELAY);	//Toggles a PIN on a PORT high and low with a DELAY

void Change_PIN(GPIO_TypeDef* PORT, uint32_t PIN, uint8_t STATE); 	//Change the PORTs PIN to a new STATE (high or low, 1 or 0) 
																																		//NOTE: if you use > 1, you'll be changing multiple pins

#endif
