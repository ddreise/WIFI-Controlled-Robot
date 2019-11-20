/* Interface.h
 * Junction interface between joystick and robot communications (this is the host)
 * 
 * By: Justin Turcotte, Daniel Dreise, Ramtin Alikhani
 * October 1st, 2019
 * 
 * ESE - Robot Project
 * 
 * All rights reserved.
 */

/* Recipe:
 * 1 - non-busywaiting joystick input as defined
 * 2 - parse joystick input
 * 3 - form appropriate command syntax
 * 4 - add command to output buffer which sends to robot
 * 5 - send commands from FIFO output buffer to robot and wait for response
 * 6 - if response is '$ACK%', continue to next command
 * 7 - if response is '$NACK%', re-send command - if after 3 re-sends '$NACK%' is still received, drop the command and move on
 */ 

#ifndef INTERFACE_H
#define INTERFACE_H

// INCLUDES //
#include "errorhandle.h"

// DEFINES //
#define COMMAND_LENGTH 20

#define COMMAND_BUFFER_SIZE 20
#define NUMBER_INPUTS 3

// GLOBAL VARIABLES //
extern char gs_Command[COMMAND_LENGTH];

// PROTOTYPES //
ERR_VAL GetCommand(char commands[NUMBER_INPUTS][COMMAND_BUFFER_SIZE], char *controllerInput);

void ButtonInput(char *command, char *sInput);
void AnalogInput(char *command, char *sInput);

#endif