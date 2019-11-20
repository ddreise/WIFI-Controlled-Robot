// Control_Law.c
//
// Functions and implementation of the Control Law loop to compensate for error using DC Motors and Encoders
//
// November 13, 2019
// By: Daniel Dreise

#include "Control_Law.h"
#include "Encoder.h"
#include <stdarg.h>
#include <stdio.h>
#include "stm32f303xe.h"
#include "Macros.h"
#include "SysTick.h"

#define STUPID_SPEED_ERROR 50

// Globals
int left_speedError, right_speedError;
int left_speed, right_speed;
int left_setpoint, right_setpoint;


void Control_Law_Init(void){
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;							//enable clock of timer 2
	TIM2->PSC = 71;																	//set PSC so that frequency is about 1 MHz
	TIM2->ARR = 0xFFFFUL;														//set ARR to 0xFFFF because max 32 bit number
	
	// For preloading
	//FORCE_BITS(TIM2->CCR1, TIM_CCR2_CCR2, value goes here);
	
	//disable digital filtering
//	TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
//	TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
	
//	//set active transition as rising and falling edges
//	TIM2->CCER &= ~(TIM_CCER_CC3P | TIM_CCER_CC3NP);
//	TIM2->CCER &= ~(TIM_CCER_CC4P | TIM_CCER_CC4NP);
	
	//clear prescaler to catch all transitions
//	TIM2->CCMR2 &= ~(TIM_CCMR2_IC3PSC);
//	TIM2->CCMR2 &= ~(TIM_CCMR2_IC4PSC);
	
//	TIM2->CCER |= TIM_CCER_CC3E;
//	TIM2->CCER |= TIM_CCER_CC4E;
	
	//enable capture for chanel 2
	SET_BITS(TIM2->DIER, TIM_DIER_CC2IE | TIM_DIER_CC2DE);
	
	//set channel 2 as output
	CLR_BITS(TIM2->CCMR1, TIM_CCMR1_CC2S);
	
	//TIM2->EGR |= TIM_EGR_UG;												//neccessary for interrupt enabling - a bit of magic
	
	//enable timer counter
	TIM2->CR1 |= TIM_CR1_CEN;
	
	NVIC_SetPriority(TIM2_IRQn, 0);
	NVIC_EnableIRQ(TIM2_IRQn);
	
}

void TIM2_IRQHandler(void){
	
	// calculate error term
	/* y = mx + b
	
	(setpoint from 0 to 100) = m(converted to cm/s) + b		--> typical max value around 30 cm/s
	100 = m(30) + 0
	y = 3.3x
	
	*/
	
	left_setpoint = left_speed / 3.3;
	right_setpoint = right_speed / 3.3;
	
	left_speedError = left_setpoint - Wheel_Speed(ENCODER_LEFT);
	right_speedError = right_setpoint - Wheel_Speed(ENCODER_RIGHT);
	
	// Check for stupid speed error --> don't run controller if garbage
	if (((left_speedError | right_speedError) < STUPID_SPEED_ERROR) && ((left_speedError | right_speedError) > -STUPID_SPEED_ERROR)){
	
			// Update integral term but only if drive is not on the rail
		if (
	}
}

