/* receive.h
 * interface of receiving joystick input
 * 
 * By: Justin Turcotte, Daniel Dreise, Ramtin Alikhani
 * October 4, 2019
 * 
 * ESE - Robot Project
 * 
 * All rights reserved.
 */

#ifndef RECEIVE_H
#define RECEIVE_H

#define MAX_JOYSTICK_BUFFER 27
#define COMMAND_LIST 3

extern char gs_JoystickBuffer[MAX_JOYSTICK_BUFFER];
extern char *gs_RawCommand;

//open pipe
void rec_JoystickInit(void);

//close pipe
void rec_JoystickClose(void);

//read named pipe
void rec_JoystickInput(void);

void rec_CommandList(void);

#endif