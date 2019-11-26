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
#include "Robot_Command.h"

#define STUPID_SPEED_ERROR 500
#define MAX_DRIVE_VALUE 35
#define MIN_DRIVE_VALUE 0
#define MAX_PWM_VALUE 100
#define MIN_PWM_VALUE 0
#define P_GAIN 10
#define I_GAIN 10
#define GAIN_DIVISOR 1000

// Globals
static volatile int16_t left_speedError, right_speedError;
volatile int16_t left_setpoint_dutyCycle, right_setpoint_dutyCycle;
static volatile int16_t left_setpoint_Speed, right_setpoint_Speed;
static volatile int16_t left_speedErrorIntegral, right_speedErrorIntegral;
static volatile int16_t left_driveValue, right_driveValue;
volatile int16_t left_dutyCycle = 0, right_dutyCycle = 0;
volatile uint16_t overflow;


void Control_Law_Init(void){
	
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;							//enable clock of timer 2
	
	TIM16->CR1 &= ~TIM_CR1_DIR;
	
	TIM16->PSC = 7999;																	//set PSC so that frequency is about 1 MHz 71
	TIM16->ARR = 0x100;																	//set ARR to 0xFFFF because max 32 bit number
	
	// Number count has to count to in order to fire interrupt
	TIM16->CCR1 = 0x100;
	
	// Set CC1IE if interrupt request is to be generated
	SET_BITS(TIM16->DIER, TIM_DIER_UIE);	// REMOVED TIM_DIER_CC1IE
	
	// Enable TIM16 interrupt
	NVIC_EnableIRQ(TIM16_IRQn);
	
	TIM16->EGR |= TIM_EGR_UG;
	
	TIM16->BDTR |= TIM_BDTR_MOE;
	
	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M;
	
	TIM16->CCMR1 |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1;
	
	TIM16->CCER &= ~TIM_CCER_CC1NP;
	
	TIM16->CCER |= TIM_CCER_CC1NE;
	
	// Enable counter
	TIM16->CR1 |= TIM_CR1_CEN;
	
}

void TIM16_IRQHandler(void){
	
	if((TIM16->SR & TIM_SR_UIF) != 0){
		
		//left_dutyCycle = left_setpoint_dutyCycle;
		//right_dutyCycle = right_setpoint_dutyCycle;
	
		left_setpoint_Speed = left_setpoint_dutyCycle / 3;
		right_setpoint_Speed = right_setpoint_dutyCycle / 3;
		
		left_speedError = left_setpoint_Speed - Wheel_Speed(ENCODER_RIGHT);
		right_speedError = right_setpoint_Speed - Wheel_Speed(ENCODER_RIGHT);
		
		// Check for stupid speed error --> don't run controller if garbage
		if ((left_speedError < STUPID_SPEED_ERROR) && (left_speedError > -STUPID_SPEED_ERROR)){ // REMOVED: && ((left_speedError | right_speedError) > -STUPID_SPEED_ERROR)){
		
			// Update integral term but only if drive is not on the rail																// CHANGE LOG: changed left_setpoint_dutyCycle to left_dutyCycle etc
			if 	(((left_dutyCycle == MAX_PWM_VALUE) && (left_speedError < 0)) ||							// REMOVED: ((left_dutyCycle == MIN_PWM_VALUE) && (left_speedError > 0)) ||
			((left_dutyCycle == MIN_PWM_VALUE) && (left_speedError > 0)) ||	
			((left_dutyCycle < MAX_PWM_VALUE) && (left_dutyCycle > MIN_PWM_VALUE))){
			
						left_speedErrorIntegral += left_speedError;
					}
			
			// Calculate the control law (NB: PI - no derivative term)...This is all the integer math
			left_driveValue = (((left_speedError * P_GAIN) + (left_speedErrorIntegral * I_GAIN)) / GAIN_DIVISOR);
			
			// Limit the controller output to the range of PWM
			if (left_driveValue > MAX_DRIVE_VALUE) left_driveValue = MAX_DRIVE_VALUE;
			else if (left_driveValue < MIN_DRIVE_VALUE) left_driveValue = MIN_DRIVE_VALUE;
			
			// Save the motor drive value for next time. Write PWM to hardware to drive wheel
			left_dutyCycle = left_driveValue * 3;
		}
		
		if ((right_speedError < STUPID_SPEED_ERROR) && (right_speedError > -STUPID_SPEED_ERROR)){
			
			if 	(((right_dutyCycle == MAX_PWM_VALUE) && (right_speedError < 0)) ||										// REMOVED: (right_dutyCycle == MIN_PWM_VALUE) && (right_speedError > 0)) ||
			((right_dutyCycle == MIN_PWM_VALUE) && (right_speedError > 0)) ||	
			((right_dutyCycle < MAX_PWM_VALUE) && (right_dutyCycle > MIN_PWM_VALUE))){
					
					right_speedErrorIntegral += right_speedError;
			}
			
			right_driveValue = (((right_speedError * P_GAIN) + (right_speedErrorIntegral * I_GAIN)) / GAIN_DIVISOR);	

			if (right_driveValue > MAX_DRIVE_VALUE) right_driveValue = MAX_DRIVE_VALUE;
			else if (right_driveValue < MIN_DRIVE_VALUE) right_driveValue = MIN_DRIVE_VALUE;
						
			right_dutyCycle = right_driveValue * 3;
		}

		
		
				
		CLR_BITS(TIM16->SR, TIM_SR_UIF);
	}

	
}

