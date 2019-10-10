/* receive.c
 * implementation receiving joystick commands
 * 
 * By: Justin Turcotte, Daniel Dreise, Ramtin Alikhani
 * October 4, 2019
 * 
 * ESE - Robot Project
 * 
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#include "receive.h"

static int fdJ;
char * myfifo = "/tmp/myfifo";
char gs_JoystickBuffer[MAX_JOYSTICK_BUFFER];
char *gs_RawCommand;

char delim = '$';

//open pipe
void rec_JoystickInit(void)
{
	fdJ = open(myfifo, O_RDONLY);
	fcntl(fdJ, F_SETFL, O_NONBLOCK);
	fcntl(fdJ, F_SETPIPE_SZ, MAX_JOYSTICK_BUFFER);
}

//close pipe
void rec_JoystickClose(void)
{
	close(fdJ);
}

//read named pipe
void rec_JoystickInput(void)
{
	//for(i = 0; i < MAX_JOYSTICK_BUFFER; i++) gs_JoystickBuffer[i] = 0;
	
	read(fdJ, gs_JoystickBuffer, MAX_JOYSTICK_BUFFER);
	//tcdrain(fdJ);
    //printf("Received: %s\n", gs_JoystickBuffer);

	gs_RawCommand = strtok(gs_JoystickBuffer, &delim);
}

void rec_CommandList(void)
{
	gs_RawCommand = strtok(NULL, &delim);
}