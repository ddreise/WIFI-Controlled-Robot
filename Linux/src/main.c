/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2019 Justin, Dan, Ramtin <linuxuser@ubuntulab>
 * 
 * Linux is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Linux is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Linux Interface for Robot & Controller
 * Justin Turcotte, Daniel Dreise, Ramtin Alikhani
 * September 27, 2019
 */

// INCLUDE //
#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

//#include <iostream.h>

#include "interface.h"
#include "receive.h"
#include "commands.h"

// DEFINE //
#define DISPLAY_STRING 1

#define FULL_TEST

// GLOBAL VARIABLES //


//read rs232 and output what the robot sends
//based off of: https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void set_mincount(int fd, int mcount)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5;        /* half second timer */

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}


int main()
{
	
#ifdef RS232
	
    char *portname = "/dev/ttyS1";
    int fd;
    int wlen;

	char *output = "Operational\r\n";
	
    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }
    /*baudrate 9600, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(fd, B9600);
    //set_mincount(fd, 0);                /* set to pure timed read */

    /* simple output */
    wlen = write(fd, output, strlen(output));
    if (wlen != strlen(output)) {
        printf("Error from write: %d, %d\n", wlen, errno);
    }
    tcdrain(fd);    /* delay for output */


    /* simple noncanonical input */
    do {
        unsigned char buf[256];
        int rdlen;

        rdlen = read(fd, buf, sizeof(buf) - 1);
        if (rdlen > 0) {
#ifdef DISPLAY_STRING
            buf[rdlen] = 0;
            printf("%s", buf);
#else /* display hex */
            unsigned char   *p;
            //printf("Read %d:", rdlen);
            for (p = buf; rdlen-- > 0; p++)
                printf(" 0x%x", *p);
            printf("\n");
#endif
        } else if (rdlen < 0) {
            printf("Error from read: %d: %s\n", rdlen, strerror(errno));
        } else {  /* rdlen == 0 */
            printf("Timeout from read\n");
        }               
        /* repeat read to get full message */
    } while (1);
#endif

#ifdef TESTING_BUTTON
	ParseJoyconCmd("BA1");
	printf("%s\n", gs_Command);
	ParseJoyconCmd("BA0");
	ParseJoyconCmd("BB0");
#endif

#ifdef TESTING_ANALOG
	ParseJoyconCmd("AR+050-050");
	
	ParseJoyconCmd("AL+100-100");
	printf("%s\n", gs_Command);
	
	ParseJoyconCmd("AG+100+100");
	ParseJoyconCmd("E34");
#endif

#ifdef RS232_OUTPUT

	char *portname = "/dev/ttyS0";
    int fd;
    int wlen;
	
    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }
    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(fd, B9600);
    //set_mincount(fd, 0);                /* set to pure timed read */

	ParseJoyconCmd("AR+000-000");
	
    /* simple output */
	printf("Sending: %d bytes: %s\n", (int)strlen(gs_Command), gs_Command);
    wlen = write(fd, gs_Command, strlen(gs_Command));
    if (wlen != strlen(gs_Command)) {
        printf("Error from write: %d, %d\n", wlen, errno);
    }
	else printf("Wrote %d bytes!\n", wlen);

    tcdrain(fd);    /* delay for output */
#endif

#ifdef INPUT_TEST
	rec_JoystickInit();

	while(1)
	{
		rec_JoystickInput();
	}

	rec_JoystickClose();
#endif

#ifdef FULL_TEST
	int i = 0;

	int fdInput;
	char *infifo = "/tmp/jsfifo";
	
	system("sudo chmod 666 /dev/ttyS0");
	mkfifo(infifo, 0666);
	
	char *portname = "/dev/ttyS0";
    int fd;
    int wlen;

	char *cOutput = "1";

	rec_JoystickInit();
	
    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	fdInput = open(infifo, O_WRONLY);
	
    if (fd < 0) {
    	printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }

	/*baudrate 115200, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(fd, B9600);
   	//set_mincount(fd, 0);                /* set to pure timed read */
	
	while(1)
	{	
		rec_JoystickInput();

		//printf("%s\n", gs_JoystickBuffer);
		
		while(gs_RawCommand != NULL)
		{
			//get current command
			//rec_CommandList();

			ParseJoyconCmd(gs_RawCommand);
			//printf("Command: %s\n", gs_RawCommand);
	
    		/* simple output */
			//printf("Sending: %d bytes: %s\n", (int)strlen(gs_Command), gs_Command);
    		wlen = write(fd, gs_Command, strlen(gs_Command));
    		if (wlen != strlen(gs_Command)) {
        		printf("Error from write: %d, %d\n", wlen, errno);
    		}
			else 
			{
				//printf("Wrote %d bytes!: %s\n", wlen, gs_Command);
			}
    		//tcdrain(fd);    /* delay for output */

			rec_CommandList();
		}

		if(iCamFlag)
		{
			wlen = write(fd, gs_Command, strlen(gs_Command));
    		if (wlen != strlen(gs_Command)) {
        		printf("Error from write: %d, %d\n", wlen, errno);
    		}
			else 
			{
				//printf("Wrote %d bytes!: %s\n", wlen, gs_Command);
			}
		}

		usleep(300);
		
		for(i = 0; i < MAX_JOYSTICK_BUFFER; i++) gs_JoystickBuffer[i] = 0;
		//clear_gs_Command();

		// request joystick input //
		if(gs_RawCommand == NULL)
		{
			write(fdInput, cOutput, strlen(cOutput));
		}
	}
#endif

	close(fdInput);
	
	return 0;
}

