/* commands.h
 * interface of commands that will be sent out to the robot
 * 
 * By: Justin Turcotte, Daniel Dreise, Ramtin Alikhani
 * October 4, 2019
 * 
 * ESE - Robot Project
 * 
 * All rights reserved.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

// DEFINES //

// GLOBAL VARIABLES //
extern int iCamFlag;

// PROTOTYPES //
// form and return homing command
void cmd_Home(char cState);
void cmd_Motor(char *sAxis);
void cmd_Camera(char *sAxis);

char* itoa(int value, char* result, int base);

#endif
