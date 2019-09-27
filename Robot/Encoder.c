//Encoder.c - Implementation to handle the Encoders
//Justin Turcotte
//April 11, 2019

// INCLUDES //
#include "Encoder.h"
#include <stdarg.h>
#include <stdio.h>
#include "stm32f303xe.h"
#include "Macros.h"
#include "SysTick.h"

// GLOBALS //
volatile uint32_t pulseWidthLeft = 0;
volatile uint32_t pulseWidthRight = 0;
volatile uint32_t lastCapturedLeft = 0;
volatile uint32_t lastCapturedRight = 0;
volatile uint32_t signalPolarityLeft = 0;	//assume input is initially low
volatile uint32_t signalPolarityRight = 0;

// INTERRUPTS //
void TIM2_IRQHandler(void)
{
	uint32_t currentCapturedLeft;
	uint32_t currentCapturedRight;
	
	//check if interrupt flag is set
	if(((TIM2->SR & TIM_SR_CC3IF) | (TIM2->SR & TIM_SR_CC4IF)) != 0)
	{
		//read CCR1 to get current value, which then will clear CC1F flag
		currentCapturedLeft = TIM2->CCR4;
		currentCapturedRight = TIM2->CCR3;
		
		//toggle polarity flag because signal is now high
		signalPolarityLeft = 1 - signalPolarityLeft;
		signalPolarityRight = 1 - signalPolarityRight;
		
		//calculate only when current input is low
		if(signalPolarityLeft == 0)
		{
			pulseWidthLeft = currentCapturedLeft - lastCapturedLeft; //assume up counting
		}
		if(signalPolarityRight == 0)
		{
			pulseWidthRight = currentCapturedRight - lastCapturedRight; //assume up counting
		}
		
		lastCapturedLeft = currentCapturedLeft;
		lastCapturedRight = currentCapturedRight;
	}
	
	//check if overflow has happened
	if((TIM2->SR & TIM_SR_UIF) != 0)
	{
		TIM2->SR &= ~TIM_SR_UIF;	//clear UIF flag to prevent re-entering
	}
}

// PRIVATE FUNCTIONS //

// Initializes port for the ultrasonic sensor
static void Encoder_Port_Init(void)
{
	//Enable ultrasonic port clocks
	GPIO_PORT_ENABLE(ENCODER_PORT);
	
	GPIO_PIN_MODE(ENCODER_PORT, ENCODER_PIN_LEFT, AF);	//enables alternate function so Encoder can be triggered
	GPIO_PIN_MODE(ENCODER_PORT, ENCODER_PIN_RIGHT, AF);	//enables alternate function so Encoder can be triggered
	
	//select alternate function 1 (TIM2_CH3N). See appendix I
	GPIOB->AFR[1] &= ~(0xF << ((ENCODER_PIN_RIGHT - 8) * 4));		//ARF[0] for pin 0-7, ARF[1] for pin 8-15
	GPIOB->AFR[1] |= 1UL << ((ENCODER_PIN_RIGHT - 8) * 4);			//TIM2_CH3N defined as 01
	
	//select alternate function 1 (TIM2_CH4N). See appendix I
	GPIOB->AFR[1] &= ~(0xF << ((ENCODER_PIN_LEFT - 8) * 4));		//ARF[0] for pin 0-7, ARF[1] for pin 8-15
	GPIOB->AFR[1] |= 1UL << ((ENCODER_PIN_LEFT - 8) * 4);			//TIM2_CH3N defined as 01
	
//	GPIO_PIN_DRV_TYPE(ENCODER_PORT, ENCODER_PIN_LEFT, OTYPER_PUSH_PULL);
//	GPIO_PIN_DRV_TYPE(ENCODER_PORT, ENCODER_PIN_RIGHT, OTYPER_PUSH_PULL);
	
//	GPIO_PIN_PUPD(ENCODER_PORT, ENCODER_PIN_LEFT, PUPDR_NO_PUPD);
//	GPIO_PIN_PUPD(ENCODER_PORT, ENCODER_PIN_RIGHT, PUPDR_NO_PUPD);
}

// Initialize the echo timer for the ultrasonic
static void Encoder_TIM_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;							//enable clock of timer 2
	TIM2->PSC = 71;																	//set PSC so that frequency is about 1 MHz
	TIM2->ARR = 0xFFFFFFFFUL;															//set ARR to 0xFFFF because max 32 bit number
	
	//set channel 3 as active input
	TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
	TIM2->CCMR2 |= TIM_CCMR2_CC3S_0;
	
	//set channel 4 as active input
	TIM2->CCMR2 &= ~TIM_CCMR2_CC4S;
	TIM2->CCMR2 |= TIM_CCMR2_CC4S_0;
	
	//disable digital filtering
	TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
	TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
	
	//set active transition as rising and falling edges
	TIM2->CCER &= ~(TIM_CCER_CC3P | TIM_CCER_CC3NP);
	TIM2->CCER &= ~(TIM_CCER_CC4P | TIM_CCER_CC4NP);
	
	//clear prescaler to catch all transitions
	TIM2->CCMR2 &= ~(TIM_CCMR2_IC3PSC);
	TIM2->CCMR2 &= ~(TIM_CCMR2_IC4PSC);
	
	TIM2->CCER |= TIM_CCER_CC3E;
	TIM2->CCER |= TIM_CCER_CC4E;
	
	//enable capture for chanel 3 & 4
	TIM2->DIER |= TIM_DIER_CC3IE;
	TIM2->DIER |= TIM_DIER_CC4IE;
	
	TIM2->EGR |= TIM_EGR_UG;												//neccessary for interrupt enabling - a bit of magic
	
	//enable timer counter
	TIM2->CR1 |= TIM_CR1_CEN;
}

// Enables the interrupt on timer 4
static void Encoder_Interrupt_Init(void)
{
	NVIC_SetPriority(TIM2_IRQn, 0);
	NVIC_EnableIRQ(TIM2_IRQn);
}

// PUBLIC FUNCTIONS //

void Encoder_Init(void)
{
	Encoder_Port_Init();
	Encoder_TIM_Init();
	Encoder_Interrupt_Init();
}

uint32_t Encoder_Read(uint8_t encoder)
{
	switch(encoder)
	{
		case ENCODER_LEFT:
			return pulseWidthLeft/1000;
		case ENCODER_RIGHT:
			return pulseWidthRight/1000;
		default:
			return 0;
	}
}
