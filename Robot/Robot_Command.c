/* Robot_Command.c
 * 
 * Purpose: Functions to receive commands in proper protocol form from host computer
 * 
 *
 * By: Daniel Dreise
 * All Rights Reserved
 *
 * Date start: October 1, 2019
 * 
 * Semester: Fall 2019
 * Embedded Systems Software - Conestoga College
 */
 
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "stm32f303xe.h"
#include "SysClock.h"
#include "Macros.h"
#include "LED.h"
#include "SysTick.h"
#include "LCD.h"
#include "Stepper_Motor.h"
#include "RC_Servo.h"
#include "DC_Motor.h"
#include "UART.h"
#include "Encoder.h"
#include "Robot_Command.h"
#include "Control_Law.h"

#define WHEEL_SETPOINT 100
#define FORWARD 0
#define BACKWARD 1


char command_str[MAX_CMD_BUFSIZ];
int16_t servo_position;
char temp_str[8];
uint16_t left_speed = 0;
uint16_t right_speed = 0;
uint8_t left_current_direction, right_current_direction;

//void TIM7_Init(void)
//{
//	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
//	
//	//TIM7->CR1 &= ~TIM_CR1_DIR;
//	
//	TIM7->PSC = 799;
//	
//	TIM7->ARR = 0xFFFF;
//	TIM7->CCR1 = 0xFFFF;
//	
//	TIM7->DIER |= TIM_DIER_UIE;
//	
//	NVIC_EnableIRQ(TIM7_IRQn);
//	
//	TIM7->EGR |= TIM_EGR_UG;
//	
//	TIM7->BDTR |= TIM_BDTR_MOE;
//	
//	TIM7->CCMR1 &= ~TIM_CCMR1_OC1M;
//	
//	TIM7->CCMR1 |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1;
//	
//	TIM7->CCER &= ~TIM_CCER_CC1NP;
//	
//	TIM7->CCER |= TIM_CCER_CC1NE;
//	
//	NVIC_SetPriority(TIM7_IRQn, (1 << __NVIC_PRIO_BITS) - 2);
//	
//	TIM7->CR1 |= TIM_CR1_CEN;
//}

//void TIM7_IRQHandler(void){
//	UART1_printf("$ACK");
//}








int CMD(char *str){
	
	// Home Camera
	if (str[1] == 'H'){
		Stepper_Home();
		RC_Position(15);
		servo_position = 15;
	}
	
	// Move Camera
	else if (str[1] == 'C') {
		
		// Stepper	
		if (str[2] == 'L'  && str[3] == '1'){
			
			if(!(Get_LimSwitch_State() == LIM_LEFT_TOUCH))
			{
				Set_Stepper_Steps(2);
				Set_Stepper_Mode(STPR_HALF_BCKWRD);
			}
			else Set_Stepper_Steps(0);
		}
		else if (str[2] == 'R' && str[3] == '1'){
			
			if(!(Get_LimSwitch_State() == LIM_RIGHT_TOUCH))
			{
				Set_Stepper_Steps(2);
				Set_Stepper_Mode(STPR_HALF_FRWRD);
			}
			else Set_Stepper_Steps(0);
		}
		else if (str[3] == '0') Set_Stepper_Steps(0);
		
		// Servo
		if (str[4] == 'D'){
			// RC position +1 degree
			//if (servo_position > 90) servo_position = 90;				// Max tilt check
			//RC_Position(servo_position++);
			
			Set_Servo_State(2);
		}
		else if (str[4] == 'U'){
			// RC position -1 degree
			//if (servo_position < (-10)) servo_position = -10;		// Max tilt check
			//RC_Position(servo_position--);
			
			Set_Servo_State(1);
		}
		else if(str[4] == 'S') Set_Servo_State(0);

	}
	
	// Move robot
	else if (str[1] == 'M') {
		
		// need to change 3 digit value to string 
		strncpy(temp_str, str + 2, 3);
		temp_str[3] = '\0';
		
//		NVIC_DisableIRQ(TIM16_IRQn);
		//right_setpoint_dutyCycle = WHEEL_SETPOINT;
		right_setpoint_dutyCycle = atoi(temp_str);
//		NVIC_EnableIRQ(TIM16_IRQn);
		
		strncpy(temp_str, str + 6, 3);
		temp_str[3] = '\0';
		//left_setpoint_dutyCycle = WHEEL_SETPOINT;
		left_setpoint_dutyCycle = atoi(temp_str);



		// DC Motor
		// Right Motor
//		NVIC_DisableIRQ(TIM16_IRQn);		
//		if (str[5] == 'F') {
//			if (right_current_direction == FORWARD) Motor(DC_M1, right_dutyCycle, DC_FORWARD);
//			
//			else if (right_current_direction == BACKWARD){
//				Motor(DC_M1, 0, DC_BACKWARD);										// Bring robot to stop
//				if ( get_right_driveValue() <= 0x8 ) right_current_direction = FORWARD;
//			}
//		}
//		
//		else if (str[5] == 'B') {
//			if (right_current_direction == BACKWARD) Motor(DC_M1, right_dutyCycle, DC_BACKWARD);
//			
//			else if (right_current_direction == FORWARD){
//				Motor(DC_M1, 0, DC_FORWARD);										// Bring robot to stop
//				if ( get_right_driveValue() <= 0x8 ) right_current_direction = BACKWARD;
//			}
//		}
//		else;
//		
//		
//		
//		if (str[9] == 'F') {
//			if (left_current_direction == FORWARD) Motor(DC_M2, left_dutyCycle, DC_FORWARD);
//			
//			else if (left_current_direction == BACKWARD){
//				Motor(DC_M2, 0, DC_BACKWARD);										// Bring robot to stop
//				if ( get_left_driveValue() <= 0x8 ) left_current_direction = FORWARD;
//			}
//		}
//		
//		else if (str[9] == 'B') {
//			if (left_current_direction == BACKWARD) Motor(DC_M2, left_dutyCycle, DC_BACKWARD);
//			
//			else if (left_current_direction == FORWARD){
//				Motor(DC_M2, 0, DC_FORWARD);										// Bring robot to stop
//				if ( get_left_driveValue() <= 0x8 ) left_current_direction = BACKWARD;
//			}
//		}
//		else;
//		
//		
//		
//		NVIC_EnableIRQ(TIM16_IRQn);
		
		// Right Motor
		if (str[5] == 'F') Motor(DC_M1, right_dutyCycle, DC_FORWARD);
		else if (str[5] == 'B') Motor(DC_M1, right_dutyCycle, DC_BACKWARD);

		// Left Motor
		if (str[9] == 'F') Motor(DC_M2, left_dutyCycle, DC_FORWARD);
		else if (str[9] == 'B') Motor(DC_M2, left_dutyCycle, DC_BACKWARD);
		else;

	}
	else;
	
	

	
	return 0;
	
	
}

int stop_Robot(void){
	
	Motor(DC_M1, 0, DC_FORWARD);
	Motor(DC_M2, 0, DC_BACKWARD);
	
	Set_Servo_State(0);
	
	Set_Stepper_Steps(0);
	
	return(0);
	
}
