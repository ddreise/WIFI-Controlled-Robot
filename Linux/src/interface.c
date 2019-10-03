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

#include "interface.h"

// DEFINES //

// FUNCTIONS //
char *ParseJoyconCmd(char *sInput)
{
	//sInput would look something like this: BA1 for button A is on
	//									 or: ARF50B50 for right analog is pressed forward 50% for m1 and backwards 50% for m2
	//ex: sInput = "BA1"
	switch(sInput[0])
	{
		case 'B':
			printf("Button press!\n");
			break;
		case 'A':
			printf("Analog detect!\n");
			break;
		default:
			printf("Error: Malformed command.\n");
	}
}