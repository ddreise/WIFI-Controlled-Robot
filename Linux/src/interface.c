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

#include "interface.h"
#include "commands.h"

// DEFINES //

// GLOBAL VARIABLES //
char gs_Command[COMMAND_LENGTH];

// FUNCTIONS //
void ParseJoyconCmd(char *sInput)
{
	//sInput would look something like this: BA1 for button A is on
	//									 or: AR+050-050 for right analog is pressed forward 50% for m1 and backwards 50% for m2
	//ex: sInput = "BA1"
	
	switch(sInput[0])
	{
		case 'B':
			ButtonPress(sInput);
			break;
		case 'A':
			printf("Analog detect!\n");
			break;
		default:
			printf("Error: Unknown command identifier: %c\n", sInput[0]);
	}
}

void ButtonPress(char* sInput)
{	
	switch(sInput[1])
	{
		case 'A':
			cmd_Home(sInput[2]);
			break;
		default:
			printf("Error: Unknown button pressed: %c\n", sInput[1]);
	}
}