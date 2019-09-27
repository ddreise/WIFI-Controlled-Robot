//RC_Servo.h - Interface to handle the RC_Servo positioning
//Justin Turcotte
//April 5, 2019



#ifndef RC_SERVO_H
#define RC_SERVO_H

#include "stm32f303xe.h"

// RC SERVO PORT INFORMATION //

#define RC_PORT 				A												//port used to manipulate the RC servo

#define RC_SIGNAL_PIN		15UL										//pin used to manipulate the PWN signal to the RC servo
#define RC_SIGNAL_BIT		(1UL << RC_SIGNAL_PIN)	//bit mask used for manipulation of the output register

#define RC_TIMER				(TIM2)																

#define RC_TIMER_OUT		(RC_TIMER->BDTR)											//accesses the output enable register for the timers
#define RC_TIMER_OUT_EN	((RC_TIMER_OUT) |= (TIM_BDTR_MOE))		//enables PWM output for the timer used on the RC servo
#define RC_TIMER_OUT_DD	((RC_TIMER_OUT) &= ~(TIM_BDTR_MOE))		//disables PWM output for the timer used on the RC servo

#define RC_TIMER_STATE	(RC_TIMER->CR1)												//accesses the main register of the timer
#define RC_TIMER_EN			((RC_TIMER_STATE) |= (TIM_CR1_CEN))		//enables the timer for the RC servo
#define RC_TIMER_DD			((RC_TIMER_STATE) &= ~(TIM_CR1_CEN))	//disables the timer for the RC servo

#define RC_TIMER_PSC		719																		//(CLOCK_FREQUENCY)/(1+PSC) = PSC_F -> in this case the board is operating at 72MHz
#define RC_TIMER_AR			1999																	//(AR + 1)(1/PSC_F) = PWM_PERIOD

#define RC_SIGNAL_PERIOD_US	20000															//period of PWM signal servo needs to operate

// PROTOTYPES //
void RC_Init(void);	//sets up required registers/ports for use with the RC servo
void RC_Position(int8_t position_degree);	//change the position of the RC servo

#endif
