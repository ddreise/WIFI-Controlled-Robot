/* uart.c - implementation of uart interface
 * ESE - Robot Project
 * 
 * Justin Turcotte
 * Daniel Dreise
 * Ramtin Alikhani
 * 
 * Nov 6, 2019
 */

// INCLUDES //
#include "uart.h"
#include "errorhandle.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

// DEFINES //

// IMPORTANT VARIABLES //
int fdUART = 0;

// PROTOTYPES //
int set_interface_attribs(int fd, int speed);
void set_mincount(int fd, int mcount);

// FUNCTIONS //
ERR_VAL UARTInit(char *portname, int flags, int baudrate, 
                 int bits, int parity, int stopbits)
{
	fdUART = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	if(fdUART < 0)
	{
		return FAILURE;
	}

	//flags = fcntl(fdUART, F_GETFL, 0);
	//fcntl(fdUART, F_SETFL, flags | O_NONBLOCK);

	set_interface_attribs(fdUART, B9600);

	return SUCCESS;
}

ERR_VAL UARTRead(char *dest, int destSize)
{
	char c = 0;
	int i = 0;
	int start = 0;
	//read(fdUART, dest, destSize);

	do
	{
		read(fdUART, &c, 1); //read one character

		if(start && (c != '%') )
		{
			if(i < (destSize - 1))
			{
				dest[i] = c;

				i++;

				//printf("Char: %s\n", dest);
			}
		}

		if(c == '$') start = 1;
		
	}while(c != '%');

	start = 0;
		
	return SUCCESS;
}
ERR_VAL UARTWrite(char *src, int srcSize)
{
	write(fdUART, src, srcSize);
	return SUCCESS;
}

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