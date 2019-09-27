//SysTick.h - Interface to use the system timer
//Justin Turcotte
//February 7, 2019

#ifndef SYSTICK_H
#define SYSTICK_H

#include "stm32f303xe.h"

#define SYSTICK_TICKS 72	//number of ticks indicate frequency of interrupt -> ticks = clock_freq_hz * wanted_delay_s

//PROTOTYPES
void SysTick_Init(uint32_t ticks);		//Will initialize the system timer for interrupts
void SysTick_Handler(void);						//Code in this function will run on interrupt
void Delay_us(uint32_t nTime);				//delay by nTime cycles of ticks
void Delay_ms(uint32_t nTime);				//delays by nTime * 1000 cycles of ticks
void Delay_s(uint32_t nTime);					//delays by nTime * 1 000 000 cycles of ticks

#endif
