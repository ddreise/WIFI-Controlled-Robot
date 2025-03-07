/* commands.c
 * implementation of commands to be sent to robot
 * 
 * By: Justin Turcotte, Daniel Dreise, Ramtin Alikhani
 * October 4, 2019
 * 
 * ESE - Robot Project
 * 
 * All rights reserved.
 */

// INCLUDES //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <fenv.h>

#include "commands.h"
#include "interface.h"

// DEFINES //
#define HOME_COMMAND "$H%"

// GLOBAL VARIABLES //
int iCamFlag = 0;

// FUNCTIONS //
void cmd_Home(char cState)
{
	switch(cState)
	{
		case '1':
			strcpy(gs_Command, HOME_COMMAND);
			break;
		//default: //since we don't care about anything other than an on state
			//printf("NOT HOMING...\n");
		
	}
}

void cmd_Motor(char *sAxis)
{
	int i = 0;

	//store string numbers of x and y
	char cXNum[5];
	char cYNum[5];

	//ints of x and y
	int iXNum = 0;
	int iYNum = 0;

	//motor output
	int iLMotor = 0;
	int iRMotor = 0;

	char cLMDir = 'F';
	char cRMDir = 'F';

	//string motor output
	//char sLMotor[10];
	//char sRMotor[10];
	
	//clear both string numbers
	for(i = 0; i < 5; i++)
	{
		cXNum[i] = 0;
		cYNum[i] = 0;
		//sLMotor[i] = '0';
		//sRMotor[i] = '0';
	}

	//get x and y numbers - bytes 1,2,3 and 5,6,7 respectively
	memcpy(cXNum, &sAxis[0], 4);
	memcpy(cYNum, &sAxis[4], 4);
	
	//turn string numbers to integers
	iXNum = atoi(cXNum);
	iYNum = atoi(cYNum);

	//get motor output
	//iLMotor = iYNum + iXNum;
	//iRMotor = iYNum - iXNum;
	if(iYNum >= 0 && iXNum >= 0) //quadrant A
	{
		iRMotor = sqrt(pow(iXNum, 2) + pow(iYNum, 2));
		iLMotor = abs( (int) ( (double)((double)100 - (double)iXNum) * ((double)iRMotor/(double)100) ) );

		cLMDir = 'F';
		cRMDir = 'F';
	}
	else if(iYNum >= 0 && iXNum < 0) //quadrant B
	{

		iLMotor = sqrt(pow(iXNum, 2) + pow(iYNum, 2));
		iRMotor = abs( (int) ( (double)((double)(-100) - (double)iXNum) * ((double)iLMotor/(double)100) ) );
		
		cLMDir = 'F';
		cRMDir = 'F';
	}
	else if(iYNum < 0 && iXNum < 0) //quadrant C
	{

		iLMotor = sqrt(pow(iXNum, 2) + pow(iYNum, 2));
		iRMotor = abs( (int) ( (double)((double)(-100) - (double)iXNum) * ((double)iLMotor/(double)100) ) );
		
		cLMDir = 'B';
		cRMDir = 'B';
	}
	else if(iYNum < 0 && iXNum >= 0) //quadrant D
	{

		iRMotor = sqrt(pow(iXNum, 2) + pow(iYNum, 2));
		iLMotor = abs( (int) ( (double)((double)100 - (double)iXNum) * ((double)iRMotor/(double)100) ) );
		//iLMotor *= (-1);
		
		cLMDir = 'B';
		cRMDir = 'B';
	}

	//printf("MOTORS: %d : %d\n", iLMotor, iRMotor);
	
	//if motor output is under -100 or above 100, cap it to -100 or 100
	if(iLMotor > 100) iLMotor = 100;
	else if(iLMotor < -100) iLMotor = -100;
	
	if(iRMotor > 100) iRMotor = 100;
	else if(iRMotor < -100) iRMotor = -100;
	
	//put speed back into string
	//itoa(iLMotor, sLMotor, 10);
	//itoa(iRMotor, sRMotor, 10);

	//printf("MOTOR: %s : %s\n", sLMotor, sRMotor);

	//eliminate null terminator
	//for(i = 0; i < 10; i++)
	//{
	//	if(sLMotor[i] == 0) sLMotor[i] = '0';
	//	if(sRMotor[i] == 0) sRMotor[i] = '0';
	//}

	//form output command
	//looks like $M(sL)(dL)(sR)(dR)%

	//clear command
	for(i = 0; i < COMMAND_LENGTH; i++) gs_Command[i] = 0;
	
	//put together command
	sprintf(gs_Command, "$M%03d%c%03d%c%%", iRMotor, cRMDir, iLMotor, cLMDir);

	printf("AXIS: %d : %d\n", iXNum, iYNum);
	printf("COMMAND: %s\n", gs_Command);
	//memcpy(&gs_Command[0], "$M", 2);
	//memcpy(&gs_Command[2], &sLMotor[1], 3);
	//memcpy(&gs_Command[6], &sRMotor[1], 3);
	//memcpy(&gs_Command[10], "%", 1);

	//if(iLMotor > 0) memcpy(&gs_Command[5], "F", 1);
	//else memcpy(&gs_Command[5], "B", 1);
	
	//if(iRMotor > 0) memcpy(&gs_Command[9], "F", 1);
	//else memcpy(&gs_Command[9], "B", 1);
}

void cmd_Camera(char *sAxis)
{
	int i = 0;

	//store string numbers of x and y
	char sXNum[5];
	char sYNum[5];

	//ints of x and y
	int iXNum = 0;
	int iYNum = 0;

	//motor output
	static int iStepper = 0;
	static char cStepper = 0;
	static char cServo = 0;

	//string camera output
	char sStepper[10];
	
	//clear both string numbers
	for(i = 0; i < 5; i++)
	{
		sXNum[i] = 0;
		sYNum[i] = 0;
		sStepper[i] = '0';
	}

	//get x and y numbers - bytes 0 - 3 and 4 - 7 respectively
	memcpy(sXNum, &sAxis[0], 4);
	memcpy(sYNum, &sAxis[4], 4);
	
	//turn string numbers to integers
	iXNum = atoi(sXNum);
	iYNum = atoi(sYNum);

	//get servo output
	if(iXNum == 0) iStepper = 0;
	else iStepper = 1;

	if(iXNum > 0) cStepper = 'L';
	else cStepper = 'R';

	if(iYNum > 0) cServo = 'U';
	else if(iYNum < 0) cServo = 'D';
	else cServo = 'S';

	//put stepper back into string
	itoa(iStepper, sStepper, 10);

	//printf("%s\n", sStepper);

	//put together command
	//clear command
	for(i = 0; i < COMMAND_LENGTH; i++) gs_Command[i] = 0;
	
	memcpy(&gs_Command[0], "$C", 2);
	memcpy(&gs_Command[2], &cStepper, 1);
	memcpy(&gs_Command[3], &sStepper[0], 1);
	memcpy(&gs_Command[4], &cServo, 1);
	memcpy(&gs_Command[5], "%", 1);

	if(iStepper || cServo != 'S') iCamFlag = 1;
	else iCamFlag = 0;
}

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char* itoa(int value, char* result, int base) 
{
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}