/* Interface.c
 * Junction implementation between joystick and robot communications (this is the host)
 * 
 * By: Justin Turcotte, Daniel Dreise, Ramtin Alikhani
 * October 1st, 2019
 * 
 * ESE - Robot Project
 * 
 * All rights reserved.
 */

// INCLUDES //
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "interface.h"
#include "commands.h"
//#include "receive.h"
#include "buffers.h"
#include "controller.h"

// DEFINES //

// GLOBAL VARIABLES //
char gs_Command[COMMAND_LENGTH];

// PROTOTYPES //
ERR_VAL ParseInput(char *command, char *input);

// FUNCTIONS //
ERR_VAL GetCommand(char commands[NUMBER_INPUTS][COMMAND_BUFFER_SIZE], char *controllerInput)
{
	u_int8_t i = 0;

	char tempBuf[COMMAND_BUFFER_SIZE];
	char *tempInputBuf;
	
	char delim = '$';

	BufferClear(tempBuf, sizeof(tempBuf));
	
	//process each input and save into its respective command
	tempInputBuf = strtok(controllerInput, &delim);

	for(i = 0; tempInputBuf != 0; i++)
	{
		ParseInput(tempBuf, tempInputBuf);
		
		strcpy(commands[i], tempBuf);
		
		tempInputBuf = strtok(NULL, &delim);
	}

	return SUCCESS;
}

ERR_VAL ParseInput(char *command, char *input)
{
	switch(input[0])
	{
		case 'B':
			ButtonInput(command, input);
			break;
		case 'A':
			AnalogInput(command, input);
			break;
		default:
			break;
	}

	return SUCCESS;
}

void ButtonInput(char *command, char* sInput)
{	
	switch(sInput[1])
	{
		case 'A':
			cmd_Home(command, sInput[2]);
			break;
		default:
			printf("Error: Unknown button pressed: %c\n", sInput[1]);
	}
}

void AnalogInput(char *command, char *sInput)
{
	char sAxis[COMMAND_LENGTH];
	
	switch(sInput[1])
	{
		case 'L':
			//extract values from motor out of string
			memcpy(sAxis, &sInput[2], 8);
			cmd_Camera(command, sAxis);
			break;
		case 'R':
			//extract values from motor out of string
			memcpy(sAxis, &sInput[2], 8);
			cmd_Motor(command, sAxis);
			break;
		default:
			printf("Error: Unknown analog input: %c\n", sInput[1]);
	}
}