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
#include <string.h>

#include "commands.h"
#include "interface.h"

// DEFINES //

// FUNCTIONS //
void cmd_Home(char cState)
{
	switch(cState)
	{
		case '1':
			strcpy(gs_Command, "$H%");
			break;
		default: //since we don't care about anything other than an on state
			printf("NOT HOMING...\n");
	}
}