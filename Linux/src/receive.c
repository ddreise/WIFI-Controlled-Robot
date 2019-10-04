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
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "receive.h"

static int fd;
char * myfifo = "/tmp/myfifo";
char gs_JoystickBuffer[MAX_JOYSTICK_BUFFER];

//open pipe
void rec_JoystickInit(void)
{
	fd = open(myfifo, O_RDONLY);
}

//close pipe
void rec_JoystickClose(void)
{
	close(fd);
}

//read named pipe
void rec_JoystickInput()
{
	read(fd, gs_JoystickBuffer, MAX_JOYSTICK_BUFFER);
    printf("Received: %s\n", gs_JoystickBuffer);
}