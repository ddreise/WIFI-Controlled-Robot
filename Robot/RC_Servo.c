//RC_Servo.c - Implementation to handle the RC_Servo positioning
//Justin Turcotte
//April 5, 2019

// INCLUDES //
#include "RC_Servo.h"
#include <stdarg.h>
#include <stdio.h>
#include "stm32f303xe.h"
#include "Macros.h"
#include "SysTick.h"
#include "LCD.h"

#define MAX_UP_TILT 90
#define	MAX_DOWN_TILT -10

// STATIC VALUES //
static uint16_t pulse_width = 150;	//pulse width of where servo should be, initialized to starting position 

// STATIC FUNCTIONS //
// Enables PWM output for the RC servo
static void RC_Timer_EN(void)
{
	RC_TIMER_OUT_EN;
	RC_TIMER_EN;
}

// Disables PWM output for the RC servo
static void RC_Timer_DD(void)
{
	RC_TIMER_OUT_DD;
	RC_TIMER_DD;
}

// Change the pulse width of the PWM signal controlling the RC servo
static void RC_Pulse_Width(int8_t position_degree)
{
	uint16_t pulseWidth = (position_degree * 10) + 1500;
	float ARpercent = (float)pulseWidth / RC_SIGNAL_PERIOD_US;
	float CCRvalue = (ARpercent*(RC_TIMER_AR+1));
	pulse_width = (uint16_t)CCRvalue;
}

// PUBLIC FUNCTIONS //
// Sets up required registers/ports for use with the RC servo
void RC_Init(void)
{
	RC_Timer_DD();
	
	//Configure RC Servo pin
	//Enable servo port clocks NOTE - SERVO NEEDS ITS PIN ON A PWM COMPATIBLE PIN
	GPIO_PORT_ENABLE(RC_PORT);
	
	GPIO_PIN_MODE(RC_PORT, RC_SIGNAL_PIN, AF);	//enables alternate function so servo can receive PWM input
	
	//select alternate function 1 (TIM1_CH1N). See appendix I
	GPIOA->AFR[1] &= ~(0xFUL << ((RC_SIGNAL_PIN - 8) * 4));		//ARF[0] for pin 0-7, ARF[1] for pin 8-15
	GPIOA->AFR[1] |= 1UL << ((RC_SIGNAL_PIN - 8) * 4);			//TIM2_CH1N defined as 01
	
	//GPIO_PIN_SPEED(RC_PORT, RC_SIGNAL_PIN, OSPEEDR_LOW);	//set I/O output speed value as low
	
	GPIO_PIN_PUPD(RC_PORT, RC_SIGNAL_PIN, PUPDR_NO_PUPD); //set beeper pin as no pull-up/pull-down
	
	//Configure Timer 2
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;	//Enable servo timer clocks
	
	//counting direction: 0 = up, 1 = down
	TIM2->CR1 &= ~TIM_CR1_DIR;	//select up-counting
	
	//prescaler - slow down input clock by factor of (1 + prescaler)
	TIM2->PSC = RC_TIMER_PSC;	//80 MHz / (1 + 799) = 100KHz
	
	//auto-reload
	TIM2->ARR = RC_TIMER_AR;	//determines the period of the PWM signal
	
	//clear output compare mode bits for channel 1
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;
	
	//select PWM mode 1 output on channel 1 (OC1M = 110)
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	
	//output 1 preload enable
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE;
	
	TIM2->CR1 |= TIM_CR1_ARPE;
	
	TIM2->EGR |= TIM_EGR_UG;
	
	//select output polarity: 0 = active high, 1 = active low
	TIM2->CCER &= ~TIM_CCER_CC1P;	//OC1N = OCREF + CC1NP
	
	//enable complementary output of channel 1 (CH1N)
	TIM2->CCER |= TIM_CCER_CC1E;
	
	//output compare register for channel 1
	TIM2->CCR1 = pulse_width;	//determines how much of PWM period is high (width between pulses determines servo position)
	
	RC_Timer_EN();
}

// Update the position of the RC servo
void RC_Position(int8_t position_degree)
{
	if(position_degree > MAX_UP_TILT) 
	{
		position_degree = MAX_UP_TILT;
		//LCD_Printf(SECOND_LINE, "CANT EXCEED +45DEG");					// Error message
		LCD_Printf(FIRST_LINE, "RC SERVO %d", position_degree);
	}
	else if(position_degree < MAX_DOWN_TILT) 
	{
		position_degree = MAX_DOWN_TILT;
		//LCD_Printf(SECOND_LINE, "CANT EXCEED -5DEG");						// Error message
		LCD_Printf(FIRST_LINE, "RC SERVO %d", position_degree);
	}
	else
	{
		LCDclear();
		LCD_Printf(FIRST_LINE, "RC SERVO %d", position_degree);
		
	}
	
	RC_Pulse_Width(position_degree);
	
	TIM2->CCR1 = pulse_width;
}
