//DC_Motor.c - Implementation to handle the DC Motor output
//Justin Turcotte
//February 4, 2019

#include "DC_Motor.h"
#include "stm32f303xe.h"
#include "Macros.h"
#include "SysTick.h"
#include "Control_Law.h"
#include "Encoder.h"




// STATIC FUNCTIONS //

// Enables PWM output for the RC servo
static void DC_Timer_EN(void)
{
	DC_TIMER_OUT_EN;
	DC_TIMER_EN;
}

// Disables PWM output for the RC servo
static void DC_Timer_DD(void)
{
	DC_TIMER_OUT_DD;
	DC_TIMER_DD;
}

static void DC_Port_Init(void)
{
	GPIO_PORT_ENABLE(DC_PORT);
	
	// PWM Inputs
	GPIO_PIN_MODE(DC_PORT, DC_PIN_IN1, AF);
	GPIO_PIN_MODE(DC_PORT, DC_PIN_IN2, AF);
	GPIO_PIN_MODE(DC_PORT, DC_PIN_IN3, AF);
	GPIO_PIN_MODE(DC_PORT, DC_PIN_IN4, AF);
	
	GPIOB->AFR[0] &= ~(0xFUL << ((DC_PIN_IN1) * 4));		//ARF[0] for pin 0-7, ARF[1] for pin 8-15
	GPIOB->AFR[0] |= 2UL << ((DC_PIN_IN1) * 4);					//TIM4_CH1N
	
	GPIOB->AFR[0] &= ~(0xFUL << ((DC_PIN_IN2) * 4));		//ARF[0] for pin 0-7, ARF[1] for pin 8-15
	GPIOB->AFR[0] |= 2UL << ((DC_PIN_IN2) * 4);					//TIM4_CH2N
	
	GPIOB->AFR[1] &= ~(0xFUL << ((DC_PIN_IN3 - 8) * 4));		//ARF[0] for pin 0-7, ARF[1] for pin 8-15
	GPIOB->AFR[1] |= 2UL << ((DC_PIN_IN3 - 8) * 4);					//TIM4_CH3N
	
	GPIOB->AFR[1] &= ~(0xFUL << ((DC_PIN_IN4 - 8) * 4));		//ARF[0] for pin 0-7, ARF[1] for pin 8-15
	GPIOB->AFR[1] |= 2UL << ((DC_PIN_IN4 - 8) * 4);					//TIM4_CH4N
	
	GPIO_PIN_PUPD(DC_PORT, DC_PIN_IN1, PUPDR_NO_PUPD);
	GPIO_PIN_PUPD(DC_PORT, DC_PIN_IN2, PUPDR_NO_PUPD);
	GPIO_PIN_PUPD(DC_PORT, DC_PIN_IN3, PUPDR_NO_PUPD);
	GPIO_PIN_PUPD(DC_PORT, DC_PIN_IN4, PUPDR_NO_PUPD);
}

static void DC_TIM_Init(void)
{
	//Configure Timer 4
	DC_Timer_DD();
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;	//Enable servo timer clocks
	
	//counting direction: 0 = up, 1 = down
	TIM4->CR1 &= ~TIM_CR1_DIR;	//select up-counting
	
	//prescaler - slow down input clock by factor of (1 + prescaler)
	TIM4->PSC = DC_TIMER_PSC;	//72 MHz / (1 + 799) = 100KHz
	
	//auto-reload
	TIM4->ARR = DC_TIMER_AR;	//determines the period of the PWM signal
	
	//clear output compare mode bits for channels 1 2 3 4
	TIM4->CCMR1 &= ~TIM_CCMR1_OC1M;
	TIM4->CCMR1 &= ~TIM_CCMR1_OC2M;
	TIM4->CCMR2 &= ~TIM_CCMR2_OC3M;
	TIM4->CCMR2 &= ~TIM_CCMR2_OC4M;
	
	//select PWM mode 1 output on channel 1 (OC1M = 110)
	TIM4->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	TIM4->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
	TIM4->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
	TIM4->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
	
	//output 1 preload enable
	TIM4->CCMR1 |= TIM_CCMR1_OC1PE;
	TIM4->CCMR1 |= TIM_CCMR1_OC2PE;
	TIM4->CCMR2 |= TIM_CCMR2_OC3PE;
	TIM4->CCMR2 |= TIM_CCMR2_OC4PE;
	
	TIM4->CR1 |= TIM_CR1_ARPE;
	
	TIM4->EGR |= TIM_EGR_UG;
	
	//select output polarity: 0 = active high, 1 = active low for channels 1 2 3 4
	TIM4->CCER &= ~TIM_CCER_CC1P;	//OC1N = OCREF + CC1NP
	TIM4->CCER &= ~TIM_CCER_CC2P;	//OC1N = OCREF + CC1NP
	TIM4->CCER &= ~TIM_CCER_CC3P;	//OC1N = OCREF + CC1NP
	TIM4->CCER &= ~TIM_CCER_CC4P;	//OC1N = OCREF + CC1NP
	
	//enable output of channel 1 2 3 4
	TIM4->CCER |= TIM_CCER_CC1E;
	TIM4->CCER |= TIM_CCER_CC2E;
	TIM4->CCER |= TIM_CCER_CC3E;
	TIM4->CCER |= TIM_CCER_CC4E;
	
	//output compare register for channel 1
	TIM4->CCR1 = 0;	//determines how much of PWM period is high (width between pulses determines servo position)
	TIM4->CCR2 = 0;
	TIM4->CCR3 = 0;
	TIM4->CCR4 = 0;
	
	DC_Timer_EN();
}





static void Set_CCRx(uint8_t channel, uint8_t driveDutyCycle_percent)
{
	uint16_t driveDutyCycle;
	
	DC_Timer_DD();
	
	//1 - convert duty cycle to CCRx value
	//2 - adjust appropriate motor inputs
	
	driveDutyCycle = (uint16_t)driveDutyCycle_percent * ((DC_TIMER_AR + 1) / 100);
	
	switch(channel)
	{
		case DC_M1_IN1_CH:
			TIM4->CCR1 = driveDutyCycle;
			break;
		case DC_M1_IN2_CH:
			TIM4->CCR2 = driveDutyCycle;
			break;
		case DC_M2_IN3_CH:
			TIM4->CCR3 = driveDutyCycle;
			break;
		case DC_M2_IN4_CH:
			TIM4->CCR4 = driveDutyCycle;
			break;
		default:
			break;
	}
	
	DC_Timer_EN();
}

// FUNCTIONS //

void DC_Init(void)
{
	DC_Port_Init();
	DC_TIM_Init();
}

void Motor(uint8_t motorNum, uint8_t driveDutyCycle_percent, uint8_t motorDirection)
{
	//1 - determine which motor is being used
	//2 - determine the direction
	//3 - output appropriate mode
	
	switch(motorNum)
	{
		case DC_M1:
			switch(motorDirection)
			{
				case DC_BRAKE:
					Set_CCRx(DC_M1_IN2_CH, driveDutyCycle_percent); 
					Set_CCRx(DC_M1_IN1_CH, driveDutyCycle_percent);
					break;
				case DC_COAST:
					Set_CCRx(DC_M1_IN2_CH, 0); 
					Set_CCRx(DC_M1_IN1_CH, 0);
					break;
				case DC_FORWARD:
					Set_CCRx(DC_M1_IN1_CH, driveDutyCycle_percent);
					Set_CCRx(DC_M1_IN2_CH, 0);
					break;
				case DC_BACKWARD:
					Set_CCRx(DC_M1_IN2_CH, driveDutyCycle_percent); 
					Set_CCRx(DC_M1_IN1_CH, 0);
					break;
				default:
					break;
			}
			break;
		case DC_M2:
			switch(motorDirection)
			{
				case DC_BRAKE:
					Set_CCRx(DC_M2_IN3_CH, driveDutyCycle_percent);
					Set_CCRx(DC_M2_IN4_CH, driveDutyCycle_percent);
					break;
				case DC_COAST:
					Set_CCRx(DC_M2_IN3_CH, 0);
					Set_CCRx(DC_M2_IN4_CH, 0);
					break;
				case DC_FORWARD:
					Set_CCRx(DC_M2_IN3_CH, driveDutyCycle_percent);
					Set_CCRx(DC_M2_IN4_CH, 0);
					break;
				case DC_BACKWARD:
					Set_CCRx(DC_M2_IN4_CH, driveDutyCycle_percent);
					Set_CCRx(DC_M2_IN3_CH, 0);
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}
