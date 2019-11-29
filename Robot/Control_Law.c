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
#include "DAC.h"

#define STUPID_SPEED_ERROR 70
#define MAX_DRIVE_VALUE 25
#define MIN_DRIVE_VALUE 0
#define MAX_PWM_VALUE 100
#define MIN_PWM_VALUE 0
//#define P_GAIN 100
//#define I_GAIN 1
//#define GAIN_DIVISOR 1000


// Globals
static volatile int32_t left_speedError, right_speedError;
volatile int16_t left_setpoint_dutyCycle, right_setpoint_dutyCycle;
static volatile int16_t left_setpoint_Speed, right_setpoint_Speed;
static volatile int32_t left_speedErrorIntegral, right_speedErrorIntegral;
static volatile int32_t left_driveValue, right_driveValue;
static volatile int32_t left_updated_DV, right_updated_DV;
volatile int32_t left_feedback, right_feedback;
volatile int16_t left_dutyCycle = 0, right_dutyCycle = 0;
volatile uint16_t overflow;
volatile uint16_t left_encoder, right_encoder;
int32_t P_GAIN = 5;
int32_t I_GAIN = 30;
int32_t GAIN_DIVISOR = 100;


void Control_Law_Init(void){
	
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;							//enable clock of timer 2
	
	TIM16->CR1 &= ~TIM_CR1_DIR;
	
	TIM16->PSC = 71;																	//set PSC so that frequency is about 1 MHz 71 CHANGED FROM 799
	TIM16->ARR = 0xFFFF;																	//set ARR to 0xFFFF because max 32 bit number	CHANGED FROM FFF
	
	// Number count has to count to in order to fire interrupt
	//TIM16->CCR1 = 0x100;
	
	// Set CC1IE if interrupt request is to be generated
	SET_BITS(TIM16->DIER, TIM_DIER_UIE);	// REMOVED TIM_DIER_CC1IE
	
	// Enable TIM16 interrupt
	NVIC_EnableIRQ(TIM16_IRQn);
	NVIC_SetPriority(TIM16_IRQn, 0);
	
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
		
//		left_dutyCycle = left_setpoint_dutyCycle;
//		right_dutyCycle = right_setpoint_dutyCycle;
	
		left_setpoint_Speed = left_setpoint_dutyCycle / 3;
		right_setpoint_Speed = right_setpoint_dutyCycle / 3;
		
		left_encoder = Wheel_Speed(ENCODER_LEFT);
		right_encoder = Wheel_Speed(ENCODER_RIGHT);
		
		left_speedError = left_setpoint_Speed - left_encoder;
		right_speedError = right_setpoint_Speed - right_encoder;
		
		// Check for stupid speed error --> don't run controller if garbage
		if ((left_speedError < STUPID_SPEED_ERROR) && (left_speedError > -STUPID_SPEED_ERROR)){ // REMOVED: && ((left_speedError | right_speedError) > -STUPID_SPEED_ERROR)){
		
			// Update integral term but only if drive is not on the rail																// CHANGE LOG: changed left_setpoint_dutyCycle to left_dutyCycle etc
			if 	(((left_driveValue == MAX_DRIVE_VALUE) && (left_speedError < 0)) ||							// REMOVED: ((left_dutyCycle == MIN_PWM_VALUE) && (left_speedError > 0)) ||
			((left_driveValue == MIN_DRIVE_VALUE) && (left_speedError > 0)) ||	
			((left_driveValue < MAX_DRIVE_VALUE) && (left_driveValue > MIN_DRIVE_VALUE))){
			
						left_speedErrorIntegral += left_speedError;
					}
			
			// Calculate the control law (NB: PI - no derivative term)...This is all the integer math
			left_driveValue = ((left_speedError * P_GAIN) + (left_speedErrorIntegral * I_GAIN)) / GAIN_DIVISOR;
			//left_driveValue += left_feedback;
				
			
			// Limit the controller output to the range of PWM
			if (left_driveValue > MAX_DRIVE_VALUE) left_driveValue = MAX_DRIVE_VALUE;
			else if (left_driveValue < MIN_DRIVE_VALUE) left_driveValue = MIN_DRIVE_VALUE;
			
			// Save the motor drive value for next time. Write PWM to hardware to drive wheel
			left_dutyCycle = (left_driveValue * 3);
					
			if (left_dutyCycle > MAX_PWM_VALUE) left_dutyCycle = MAX_PWM_VALUE;
			else if (left_dutyCycle < MIN_PWM_VALUE) left_dutyCycle = MIN_PWM_VALUE;
		}
		
		
		
		// Check for stupid speed error --> don't run controller if garbage
		if ((right_speedError < STUPID_SPEED_ERROR) && (right_speedError > -STUPID_SPEED_ERROR)){ // REMOVED: && ((right_speedError | right_speedError) > -STUPID_SPEED_ERROR)){
		
			// Update integral term but only if drive is not on the rail																// CHANGE LOG: changed right_setpoint_dutyCycle to right_dutyCycle etc
			if 	(((right_driveValue == MAX_DRIVE_VALUE) && (right_speedError < 0)) ||							// REMOVED: ((right_dutyCycle == MIN_PWM_VALUE) && (right_speedError > 0)) ||
			((right_driveValue == MIN_DRIVE_VALUE) && (right_speedError > 0)) ||	
			((right_driveValue < MAX_DRIVE_VALUE) && (right_driveValue > MIN_DRIVE_VALUE))){
			
						right_speedErrorIntegral += right_speedError;
					}
			
			// Calculate the control law (NB: PI - no derivative term)...This is all the integer math
			right_driveValue = ((right_speedError * P_GAIN) + (right_speedErrorIntegral * I_GAIN)) / GAIN_DIVISOR;
			//right_driveValue += right_feedback;
				
			
			// Limit the controller output to the range of PWM
			if (right_driveValue > MAX_DRIVE_VALUE) right_driveValue = MAX_DRIVE_VALUE;
			else if (right_driveValue < MIN_DRIVE_VALUE) right_driveValue = MIN_DRIVE_VALUE;
			
			// Save the motor drive value for next time. Write PWM to hardware to drive wheel
			right_dutyCycle = (right_driveValue * 3);
					
			if (right_dutyCycle > MAX_PWM_VALUE) right_dutyCycle = MAX_PWM_VALUE;
			else if (right_dutyCycle < MIN_PWM_VALUE) right_dutyCycle = MIN_PWM_VALUE;
					
		}
				
		CLR_BITS(TIM16->SR, TIM_SR_UIF);

	}
	
		CLR_BITS(TIM16->CNT, TIM_CNT_CNT); 	// reset timer
	
	DAC_output(left_driveValue);
	

	
}

uint32_t get_left_driveValue(void){
	return left_driveValue;
}

uint32_t get_right_driveValue(void){
	return right_driveValue;
}

