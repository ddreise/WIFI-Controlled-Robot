//Stepper_Motor.c - Implementation to handle the stepper motor output
//Justin Turcotte
//February 4, 2019

#include "Stepper_Motor.h"
#include "stm32f303xe.h"
#include "Macros.h"
#include "SysTick.h"
#include <stdlib.h>

// IMPORTANT VARIABLES //

static volatile int32_t stepsRemaining = 0;
static uint8_t stepMode = STPR_STOP;
static uint32_t totalSteps = 0; 
static uint32_t currentStepPos = 0;

// FUNCTIONS //

// STEPPER MOTOR //

void TIM3_IRQHandler(void)
{
	if((TIM3->SR & TIM_SR_UIF) != 0)
	{
		// Check if there are any steps to take
		if(!(stepsRemaining <= 0))
		{
			// Take a step and decrement the number of steps left
			stepperRun(stepMode);
			
			switch(stepMode)
			{
				case STPR_FULL_FRWRD:
				case STPR_FULL_BCKWRD:
					stepsRemaining -= 2;
					break;
				case STPR_HALF_FRWRD:
				case STPR_HALF_BCKWRD:
					stepsRemaining--;
					break;
			}
		}
		else
		{
			stepMode = STPR_STOP;
			stepsRemaining = 0;
		}
		
		TIM3->SR &= ~TIM_SR_UIF;
	}
}

//Initializes neccessary pins for stepper motor
void stepperInit(void)
{
	GPIO_PORT_ENABLE(STPR_PORT);

	GPIO_PIN_MODE(STPR_PORT, STPR_PIN_IN1, MODER_OUT);
	GPIO_PIN_MODE(STPR_PORT, STPR_PIN_IN2, MODER_OUT);
	GPIO_PIN_MODE(STPR_PORT, STPR_PIN_IN3, MODER_OUT);
	GPIO_PIN_MODE(STPR_PORT, STPR_PIN_IN4, MODER_OUT);
//	GPIO_PIN_MODE(STPR_PORT, STPR_PIN_NSLEEP, MODER_OUT);
//	GPIO_PIN_MODE(STPR_PORT, STPR_PIN_NFAULT, MODER_OUT);
//	GPIO_PIN_MODE(STPR_PORT, STPR_PIN_TRQ, MODER_OUT);
//	GPIO_PIN_MODE(STPR_PORT, STPR_PIN_MODE, MODER_OUT);
//	GPIO_PIN_MODE(STPR_PORT, STPR_PIN_ISEN12, MODER_OUT);
//	GPIO_PIN_MODE(STPR_PORT, STPR_PIN_ISEN34, MODER_OUT);
	
	GPIO_PIN_DRV_TYPE(STPR_PORT, STPR_PIN_IN1, OTYPER_PUSH_PULL);
	GPIO_PIN_DRV_TYPE(STPR_PORT, STPR_PIN_IN2, OTYPER_PUSH_PULL);
	GPIO_PIN_DRV_TYPE(STPR_PORT, STPR_PIN_IN3, OTYPER_PUSH_PULL);
	GPIO_PIN_DRV_TYPE(STPR_PORT, STPR_PIN_IN4, OTYPER_PUSH_PULL);
//	GPIO_PIN_DRV_TYPE(STPR_PORT, STPR_PIN_NSLEEP, OTYPER_PUSH_PULL);
//	GPIO_PIN_DRV_TYPE(STPR_PORT, STPR_PIN_NFAULT, OTYPER_PUSH_PULL);
//	GPIO_PIN_DRV_TYPE(STPR_PORT, STPR_PIN_TRQ, OTYPER_PUSH_PULL);
//	GPIO_PIN_DRV_TYPE(STPR_PORT, STPR_PIN_MODE, OTYPER_PUSH_PULL);
//	GPIO_PIN_DRV_TYPE(STPR_PORT, STPR_PIN_ISEN12, OTYPER_PUSH_PULL);
//	GPIO_PIN_DRV_TYPE(STPR_PORT, STPR_PIN_ISEN34, OTYPER_PUSH_PULL);
}

void TIM3_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	TIM3->CR1 &= ~TIM_CR1_DIR;
	
	TIM3->PSC = 799;
	
	TIM3->ARR = 0xFFFF;
	TIM3->CCR1 = 0xFFFF;
	
	TIM3->DIER |= TIM_DIER_UIE;
	
	NVIC_EnableIRQ(TIM3_IRQn);
	
	TIM3->EGR |= TIM_EGR_UG;
	
	TIM3->BDTR |= TIM_BDTR_MOE;
	
	TIM3->CCMR1 &= ~TIM_CCMR1_OC1M;
	
	TIM3->CCMR1 |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1;
	
	TIM3->CCER &= ~TIM_CCER_CC1NP;
	
	TIM3->CCER |= TIM_CCER_CC1NE;
	
	TIM3->CR1 |= TIM_CR1_CEN;
}

// Homes the stepper, configuring the stepper from 0 to totalSteps (forward to backwards)
void Stepper_Home(void)
{
	//move stepper forward by half steps until limit switch is hit
	while(!(Get_LimSwitch_State() == LIM_RIGHT_TOUCH))
	{
		stepperRun(STPR_HALF_FRWRD);
		Delay_us(2500);
	}
	
	totalSteps = 0;	//set totalSteps to 0
	
	//move stepper backwards by half steps until other limit switch is hit, increment totalSteps after each step
	while(!(Get_LimSwitch_State() == LIM_LEFT_TOUCH))
	{
		stepperRun(STPR_HALF_BCKWRD);
		totalSteps++;
		Delay_us(2500);
	}
	
	currentStepPos = 0;
	
	Stepper_Set(STPR_FULL, 0x13B, 0);	//goto stepper home
	
}

//Runs the stepper motor in usr_mode
void stepperRun(uint8_t usr_mode)
{
	//important variables
	static uint8_t mode = STPR_INVALID;																																				//stores the current mode of the stepper motor
	static uint8_t state = 0;																																									//stores the current state of the stepper motor
	static uint16_t stepperStates[STATES] = {STATE0, STATE1, STATE2, STATE3, STATE4, STATE5, STATE6, STATE7};	//holds all the steps of the stepper motor, 
																																																						//for half step go through each one, for full step, go through by 2
	
	//set the mode of the stepper motor
	//if the usr_mode is STPR_INVALID do not do anything
	if(!(usr_mode == STPR_INVALID)) mode = usr_mode;
	
	//switch states based on mode
	switch(mode)
	{
		case STPR_STOP:
			break;
		case STPR_HALF_FRWRD:
			state += STEP_HALF_FRWRD;
			currentStepPos += STEP_HALF_FRWRD;
			break;
		case STPR_FULL_FRWRD:
			state += STEP_FULL_FRWRD;
			currentStepPos += STEP_FULL_FRWRD;
			break;
		case STPR_HALF_BCKWRD:
			state -= STEP_HALF_BCKWRD;
			currentStepPos -= STEP_HALF_BCKWRD;
			break;
		case STPR_FULL_BCKWRD:
			state -= STEP_FULL_BCKWRD;
			currentStepPos -= STEP_FULL_BCKWRD;
			break;
		default:
			break;
	}
	
	FORCE_BITS(GPIOA->ODR, STPR_OUT_MASK, stepperStates[state & COUNTER_MAX]);
}

// Will set the mode (full/half step), sets the speed of the step (ms delay per step), sets the position to go to in degrees (stepper will home to +/- 90 degrees)
void Stepper_Set(uint8_t mode, uint16_t speed, int16_t position_degrees)
{
	double stepPos = 0;
	
	Set_Stepper_Speed(speed);	//set the speed
	
	stepPos = (((double)totalSteps) / 180) * (((double)position_degrees) + 90);	//convert position into appropriate step position
	
	//use stepPos - currentStepPos to determine CW (+) and CCW (-) stepping
	if((currentStepPos - stepPos) > 0)
	{
		if(mode == STPR_FULL) Set_Stepper_Mode(STPR_FULL_BCKWRD);
		else if(mode == STPR_HALF) Set_Stepper_Mode(STPR_HALF_BCKWRD);
	}
	else if ((currentStepPos - stepPos) < 0)
	{
		if(mode == STPR_FULL) Set_Stepper_Mode(STPR_FULL_FRWRD);
		else if(mode == STPR_HALF) Set_Stepper_Mode(STPR_HALF_FRWRD);
	}
	
	//if(mode == STPR_FULL)
	//{
	//	stepsRemaining = abs(currentStepPos - stepPos)/2;	//set the number of steps left to step
	//}
	//else
	//{
	//	stepsRemaining = abs(currentStepPos - stepPos);	//set the number of steps left to step
	//}
	
	stepsRemaining = abs(currentStepPos - stepPos);	//set the number of steps left to step
}

void Set_Stepper_Mode(uint8_t mode) 
{
	stepMode = mode;
}

uint8_t Get_Stepper_Mode(void)
{
	return stepMode;
}

void Set_Stepper_Steps(uint16_t steps)
{
	stepsRemaining = steps;
}

uint16_t Get_Stepper_Steps(void)
{
	return stepsRemaining;
}

int8_t Get_Stepper_Position(void)
{
	return ((int8_t)((180 / totalSteps) * currentStepPos)) - 90;
}

// LIMIT SWITCHES //
void LimSwitch_Init(void)
{
	GPIO_PORT_ENABLE(LIM_PORT);
	
	GPIO_PIN_MODE(LIM_PORT, LIM_PIN_LEFT, MODER_IN);
	GPIO_PIN_MODE(LIM_PORT, LIM_PIN_RIGHT, MODER_IN);
	
	GPIO_PIN_DRV_TYPE(LIM_PORT, LIM_PIN_LEFT, OTYPER_PUSH_PULL);
	GPIO_PIN_DRV_TYPE(LIM_PORT, LIM_PIN_RIGHT, OTYPER_PUSH_PULL);
	
	GPIO_PIN_PUPD(LIM_PORT, LIM_PIN_LEFT, PUPDR_NO_PUPD);
	GPIO_PIN_PUPD(LIM_PORT, LIM_PIN_RIGHT, PUPDR_NO_PUPD);
}

uint8_t Get_LimSwitch_State(void)
{
	//take copy of input
	uint16_t limIn = LIM_IN;
	
	//check to see which pin is pressed (low means press)
	if(!(limIn & (0x1UL << LIM_PIN_LEFT))) return LIM_LEFT_TOUCH;
	else if(!(limIn & (0x1UL << LIM_PIN_RIGHT))) return LIM_RIGHT_TOUCH;
	else return LIM_NO_TOUCH;
}

void Set_Stepper_Speed(uint16_t speed)
{
	TIM3->ARR = speed;
	TIM3->CCR1 = speed;
}
