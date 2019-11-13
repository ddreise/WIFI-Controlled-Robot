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
#define CONTROLLER_PATH "/dev/input/js1"
#define UART_PORT "/dev/ttyUSB0"

#define ACK_BUFFER_SIZE 6
#define S_INPUT_BUFFER 256

// PROTOTYPES //

// FUNCTIONS //
int main()
{
	u_int8_t i = 0;
	
	char sControllerBuf[CONTROLLER_BUFFER_SIZE + 1];
	char saCommands[NUMBER_INPUTS][COMMAND_BUFFER_SIZE];
	char sAckBuf[ACK_BUFFER_SIZE];
	char sInputBuf[S_INPUT_BUFFER];

	ERR_VAL ret = 0;

	// INIT BUFFERS //
	BufferClear(sControllerBuf, sizeof(sControllerBuf));
	for(i = 0; i < NUMBER_INPUTS; i++) BufferClear(saCommands[i], sizeof(saCommands[i]));
	BufferClear(sAckBuf, sizeof(sAckBuf));
	BufferClear(sInputBuf, sizeof(sInputBuf));
	
	// CONTROLLER SETUP //
	ControllerInit(CONTROLLER_PATH);
	
	// UART SETUP //
	UARTInit(UART_PORT, O_RDWR | O_NOCTTY | O_SYNC, 9600, 
                 8, 0, 0);

	// SOCKET SETUP //
	SocketServerInit(PORT);

	//SocketRead(sAckBuf, sizeof(sAckBuf));
	//printf("DESKTOP: %s", sAckBuf);
	//BufferClear(sAckBuf, sizeof(sAckBuf));
	strcpy(sAckBuf, "$ACK%");
	
	while(1)
	{
		//printf("Input: ");
		//SocketWrite(sAckBuf, strlen(sAckBuf));   //send ACK to desktop
		//SocketRead(sInputBuf, sizeof(sInputBuf));   //wait for input
		//printf("%s\n", sInputBuf); //print input
		//BufferClear(sInputBuf, sizeof(sInputBuf));
		
		if(strcmp(sAckBuf, "ACK") == 0)
		{
			printf("ACKNOWLEDGED!\n");
			// GET CONTROLLER INPUT //
			/*ret = ControllerGetInput(sControllerBuf, sizeof(sControllerBuf));
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
			}*/

			// GET COMMANDS FROM DESKTOP //
			SocketWrite(sAckBuf, strlen(sAckBuf));
			for(i = 0; i < NUMBER_INPUTS; i++) SocketRead(saCommands[i], sizeof(saCommands[i]));
			printf("COMMANDS RECEIVED");

			// RE-INPUT '$' AND '%' VALUES THAT WERE TAKEN OUT WHEN READ
			for(i = 0; i < NUMBER_INPUTS; i++)
			{
				char sTempCommand[COMMAND_BUFFER_SIZE];
				BufferClear(sTempCommand, sizeof(sTempCommand));
				strcpy(sTempCommand, saCommands[i]);

				sprintf(saCommands[i], "$%s%%", sTempCommand);
			}

			for(i=0;i<NUMBER_INPUTS;i++) printf("%s\n", saCommands[i]);

			// SEND COMMANDS TO ROBOT //
			for(i = 0; i < NUMBER_INPUTS; i++) UARTWrite(saCommands[i], 
						                                 strlen(saCommands[i]));
			printf("Input: %s\n", sAckBuf);

			// SEND COMMANDS TO RPi //
			//for(i = 0; i < NUMBER_INPUTS; i++) SocketWrite(saCommands[i], 
			//			                                 strlen(saCommands[i]));
			
			
			BufferClear(sAckBuf, sizeof(sAckBuf));
		}
		else
		{
			BufferClear(sAckBuf, sizeof(sAckBuf));

			UARTRead(sAckBuf, sizeof(sAckBuf));
			printf("ACK: %s\n", sAckBuf);

			//SocketRead(sAckBuf, sizeof(sAckBuf));
		}
	}
	
	return (0);
}

