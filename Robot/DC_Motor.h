//DC_Motor.h - Interface to handle the DC Motor output
//Justin Turcotte
//February 4, 2019

#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include "stm32f303xe.h"

// PIN SETUP //
#define DC_PORT				B

#define DC_PIN_ISEN12	1
#define DC_PIN_ISEN34	2
#define DC_PIN_MODE		3
#define DC_PIN_TRQ		0
#define DC_PIN_NSLEEP	4
#define DC_PIN_NFAULT	5
#define DC_PIN_IN4		9
#define DC_PIN_IN3		8
#define DC_PIN_IN2		7
#define DC_PIN_IN1		6

#define DC_M1	0
#define DC_M2	1

#define DC_M1_IN1_CH	1
#define DC_M1_IN2_CH	2
#define DC_M2_IN3_CH	3
#define DC_M2_IN4_CH	4

#define DC_COAST		0
#define DC_BRAKE		1
#define DC_FORWARD	2
#define DC_BACKWARD	3

#define DC_TIMER				(TIM4)	

#define DC_TIMER_OUT		(DC_TIMER->BDTR)											//accesses the output enable register for the timers
#define DC_TIMER_OUT_EN	((DC_TIMER_OUT) |= (TIM_BDTR_MOE))		//enables PWM output for the timer used on the RC servo
#define DC_TIMER_OUT_DD	((DC_TIMER_OUT) &= ~(TIM_BDTR_MOE))		//disables PWM output for the timer used on the RC servo

#define DC_TIMER_STATE	(DC_TIMER->CR1)												//accesses the main register of the timer
#define DC_TIMER_EN			((DC_TIMER_STATE) |= (TIM_CR1_CEN))		//enables the timer for the RC servo
#define DC_TIMER_DD			((DC_TIMER_STATE) &= ~(TIM_CR1_CEN))	//disables the timer for the RC servo

#define DC_TIMER_PSC		719UL
#define DC_TIMER_AR			1999UL


// PROTOTYPES //

void DC_Init(void);
void Motor(uint8_t motorNum, uint8_t driveDutyCycle_percent, uint8_t motorDirection);

#endif
