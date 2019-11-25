/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2019 Justin <justin@justin-VirtualBox>
 * 
 * Modularize is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Modularize is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// INCLUDES //
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <termios.h>
#include <fcntl.h> 

#include "buffers.h"
#include "pipes.h"
#include "controller.h"
#include "errorhandle.h"
#include "interface.h"
#include "uart.h"
#include "sockets.h"

// DEFINES //
#define CONTROLLER_PATH "/dev/input/js2"
#define UART_PORT "/dev/ttyS0"

#define ACK_BUFFER_SIZE 6

// PROTOTYPES //

// FUNCTIONS //
int main()
{
	u_int8_t i = 0;
	
	char sControllerBuf[CONTROLLER_BUFFER_SIZE + 1];
	char saCommands[NUMBER_INPUTS][COMMAND_BUFFER_SIZE];
	char sAckBuf[ACK_BUFFER_SIZE];

	ERR_VAL ret = 0;

	// INIT BUFFERS //
	BufferClear(sControllerBuf, sizeof(sControllerBuf));
	for(i = 0; i < NUMBER_INPUTS; i++) BufferClear(saCommands[i], sizeof(saCommands[i]));
	BufferClear(sAckBuf, sizeof(sAckBuf));
	
	// CONTROLLER SETUP //
	ControllerInit(CONTROLLER_PATH);

	// SOCKET SETUP //
	SocketClientInit(HOST, PORT);

	//strcpy(sAckBuf, "Hi\n");
	//SocketWrite(sAckBuf, strlen(sAckBuf));
	//BufferClear(sAckBuf, sizeof(sAckBuf));
	
	while(1)
	{
		if(strcmp(sAckBuf, "ACK") == 0)
		{
			// GET CONTROLLER INPUT //
			ret = ControllerGetInput(sControllerBuf, sizeof(sControllerBuf));
			printf("CONTROLLERBUF: %s\n", sControllerBuf);
			if(ret != SUCCESS)
			{
				switch(ret)
				{
					case PIPE_ERROR:
						printf("ERROR: Pipe failed to create!\n");
						break;
					case FORK_ERROR:
						printf("ERROR: Fork failed to create!\n");
					default:
						break;
				}

				return -1;
			}

			// CONVERT INPUT TO COMMAND //
			ret = GetCommand(saCommands, sControllerBuf);	// 3 commands per input
			if(ret != SUCCESS)
			{
				printf("ERROR: Problem converting input to command!\n");
				return -1;
			}

			//for(i=0;i<NUMBER_INPUTS;i++) printf("%s\n", saCommands[i]);

			// SEND COMMANDS TO RPi //
			for(i = 0; i < NUMBER_INPUTS; i++) 
			{
				SocketWrite(saCommands[i], strlen(saCommands[i]));
				printf("%s\n", saCommands[i]);
			}
			
			BufferClear(sAckBuf, sizeof(sAckBuf));
		}
		else
		{
			BufferClear(sAckBuf, sizeof(sAckBuf));

			SocketRead(sAckBuf, sizeof(sAckBuf));
		}
	}
	
	return (0);
}

