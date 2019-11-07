/* controller.c - implementation of controller interface
 * ESE - Robot Project
 * 
 * Justin Turcotte
 * Daniel Dreise
 * Ramtin Alikhani
 * 
 * Nov 6, 2019
 */

// INCLUDES //
#include "controller.h"
#include "errorhandle.h"
#include "pipes.h"
#include "buffers.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <math.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>

// DEFINES //

// IMPORTANT VARIABLES //
int js = 0;

/**
 * Current state of an axis.
 */
struct axis_state {
    short x, y;
};

// PROTOTYPES //
ERR_VAL Read_Controller(char *buf);
int read_event(int fd, struct js_event *event);
size_t get_axis_count(int fd);
size_t get_button_count(int fd);
size_t get_axis_state(struct js_event *event, struct axis_state axes[3]);


// FUNCTIONS //
ERR_VAL ControllerInit(char *sDevicePath)
{
	int iAxisCount = 0;
	int iButtonCount = 0;
	
	js = open(sDevicePath, O_RDONLY);	//open device

	//check axis count
	iAxisCount = get_axis_count(js);
	if(iAxisCount ==  0)	
	{
		return AXIS_ERROR;
	}
	printf("Axis Count: %d\n", iAxisCount);
	
	//check button count
	iButtonCount = get_button_count(js);
	if(iButtonCount == 0)
	{
		return BUTTON_ERROR;
	}
	printf("Button Count: %d\n", iButtonCount);
	
	return SUCCESS;
}

// Gets controller input via a forked child and pipe
ERR_VAL ControllerGetInput(char *buf)
{
	pid_t forkResult;
	int fdControllerPipes[2];
	ERR_VAL ret = 0;
	char tempBuf[CONTROLLER_BUFFER_SIZE];

	BufferClear(tempBuf, sizeof(tempBuf));

	ret = PipeInit(fdControllerPipes);
	if(ret == FAILURE) 
	{
		return PIPE_ERROR;
	}
	
	forkResult = fork(); //child will read controller
	if(forkResult == -1) //fork creation error
	{
		return FORK_ERROR;
	}
	else if(forkResult == 0) //in child process
	{
		close(fdControllerPipes[0]);	//close read portion of pipe
		
		//read controller input
		//write(fdControllerPipes[1], "HI!!! FROM THE CHILD!\n", 24); //send result on pipe
		Read_Controller(tempBuf);
		write(fdControllerPipes[1], tempBuf, sizeof(tempBuf));
		
		close(fdControllerPipes[1]);	//close pipe
		
		exit(EXIT_SUCCESS);	//end child execution
	}
	else //in parent
	{
		close(fdControllerPipes[1]);	//close write portion of pipe

		wait(NULL);	//wait for child to finish executing

		read(fdControllerPipes[0], buf, CONTROLLER_BUFFER_SIZE);	//read pipe until nothing is left

		close(fdControllerPipes[0]); //close read portion of pipe
		
		return SUCCESS;	//controller input has been received, so exit
	}
	
	return SUCCESS;
}

ERR_VAL Read_Controller(char *buf)
{
	int aFlag = 0;
	int aDouble = 0;

	struct js_event event;
	struct axis_state axes[3] = {0};
	size_t axis;

	int xMotor = 0;
	int yMotor = 0;

	int stepperMotor = 0;
	int servoMotor = 0;
	
	read_event(js, &event);
	switch(event.type)
	{
		case JS_EVENT_BUTTON:
			if (event.number == 0) //Green Button A is pressed 
			{
				if (aDouble == 1) aDouble = 0;
				else
				{
					aFlag = 1;
					aDouble = 1;
				}
			}
			break;
		case JS_EVENT_AXIS:
			axis = get_axis_state(&event, axes);
		    if (axis == 1)   //Right analog (DC motors)
		    {	
		      	yMotor = (int) ( (double) ( ((double)axes[axis].y) * ((double)100/(double)32767) ));
		    	xMotor = (int) ( (double) ( ((double)axes[axis].x) * ((double)100/(double)32767) ));

				xMotor *= -1;
				yMotor *= -1;
			}

			if (axis == 0)   //Left  analog (Camera)
		    {
		    	stepperMotor = (int) ( (double) ( ((double)axes[axis].x) * ((double)100/(double)32767) ));
				stepperMotor *= -1;
				servoMotor = (int) ( (double) ( ((double)axes[axis].y) * ((double)100/(double)32767) ));

				if(stepperMotor < 100 && stepperMotor > -100) stepperMotor = 0;
				if(servoMotor < 100 && servoMotor > -100) servoMotor = 0;
			}
			break;
		default:
			break;
	}

	sprintf(buf, "BA%d$AR%+04d%+04d$AL%+04d%+04d$", aFlag, xMotor, yMotor, stepperMotor, servoMotor);
	aFlag = 0;
	
	return SUCCESS;
}

/**
 * Reads a joystick event from the joystick device.
 *
 * Returns 0 on success. Otherwise -1 is returned.
 */
int read_event(int fd, struct js_event *event)
{
    ssize_t bytes;
	//int fd2 = 0;

	// UNCOMMENT FOR NON-BLOCKING READ //
	//int flags = fcntl(fd, F_GETFL, 0);
	//fcntl(js, F_SETFL, flags | O_NONBLOCK);

	//fd2 = open("/dev/null", O_WRONLY);
	
    bytes = read(fd, event, sizeof(*event));

	tcflush(fd, TCIOFLUSH);

	//splice(fd2, NULL, fd);
	
    if (bytes == sizeof(*event))
        return 0;

    /* Error, could not read full event. */
    return -1;
}

/**
 * Returns the number of axes on the controller or 0 if an error occurs.
 */
size_t get_axis_count(int fd)
{
    __u8 axes;

    if (ioctl(fd, JSIOCGAXES, &axes) == -1)
        return 0;

    return axes;
}

/**
 * Returns the number of buttons on the controller or 0 if an error occurs.
 */
size_t get_button_count(int fd)
{
    __u8 buttons;
    if (ioctl(fd, JSIOCGBUTTONS, &buttons) == -1)
        return 0;

    return buttons;
}

/**
 * Keeps track of the current axis state.
 *
 * NOTE: This function assumes that axes are numbered starting from 0, and that
 * the X axis is an even number, and the Y axis is an odd number. However, this
 * is usually a safe assumption.
 *
 * Returns the axis that the event indicated.
 */
size_t get_axis_state(struct js_event *event, struct axis_state axes[3])
{
    size_t axis = event->number / 2;

    if (axis < 3)
    {
        if (event->number % 2 == 0)
            axes[axis].x = event->value;
        else
            axes[axis].y = event->value;
    }

    return axis;
}