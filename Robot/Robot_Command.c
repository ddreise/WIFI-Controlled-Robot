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



char command_str[MAX_CMD_BUFSIZ];
int16_t servo_position;
char temp_str[8];
uint16_t left_speed = 0;
uint16_t right_speed = 0;

int CMD(char *str){
	
	// Check for beginning of command
	while (str != "$")
	
	
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
			Set_Stepper_Steps(1);
			Set_Stepper_Mode(STPR_HALF_BCKWRD);
		}
		else if (str[2] == 'R' && str[3] == '1'){
			Set_Stepper_Steps(1);
			Set_Stepper_Mode(STPR_HALF_FRWRD);
		}
		else;
		
		// Servo
		if (str[4] == 'D'){
			// RC position +1 degree
			if (servo_position > 90) servo_position = 90;				// Max tilt check
			RC_Position(servo_position++);
		}
		else if (str[4] == 'U'){
			// RC position -1 degree
			if (servo_position < (-10)) servo_position = -10;		// Max tilt check
			RC_Position(servo_position--);
		}
		else;			
	}
	
	// Move robot
	else if (str[1] == 'M') {
		
		// need to change 3 digit value to string 
		strncpy(temp_str, str + 2, 3);
		temp_str[3] = '\0';
		left_speed = atoi(temp_str);
		
		strncpy(temp_str, str + 6, 3);
		temp_str[3] = '\0';
		right_speed = atoi(temp_str);
		
		// DC Motor
		// Right Motor
		if (str[5] == 'F') Motor(DC_M1, left_speed, DC_FORWARD);
		else if (str[5] == 'B') Motor(DC_M1, left_speed, DC_BACKWARD);
		else;
		
		// Left Motor
		if (str[9] == 'F') Motor(DC_M2, right_speed, DC_FORWARD);
		else if (str[9] == 'B') Motor(DC_M2, right_speed, DC_BACKWARD);
		else;

	}
	

	
	return 0;
	
	
}