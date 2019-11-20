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
	int startup = 1;
	
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
	
	while(1)
	{
		if(strcmp(sAckBuf, "ACK") == 0 || startup)
		{
			startup = 0;
			
			// GET COMMANDS FROM DESKTOP //
			BufferClear(sAckBuf, sizeof(sAckBuf));
			strcpy(sAckBuf, "$ACK%");
			SocketWrite(sAckBuf, strlen(sAckBuf));
			for(i = 0; i < NUMBER_INPUTS; i++) 
			{
				BufferClear(saCommands[i], sizeof(saCommands[i]));
				SocketRead(saCommands[i], sizeof(saCommands[i]));
			}

			// RE-INPUT '$' AND '%' VALUES THAT WERE TAKEN OUT WHEN READ
			for(i = 0; i < NUMBER_INPUTS; i++)
			{
				char sTempCommand[COMMAND_BUFFER_SIZE];
				//char *pCmd = sTempCommand;
				BufferClear(sTempCommand, sizeof(sTempCommand));

				//memcpy(pCmd, "$", 1);
				//pCmd++;
				//memcpy(pCmd, saCommands[i], strlen(saCommands[i]));
				//pCmd += strlen(saCommands[i]);
				//memcpy(pCmd, "%\0", 2);

				sprintf(sTempCommand, "$%s%%", saCommands[i]);
				
				// SEND COMMANDS TO ROBOT //
				UARTWrite(sTempCommand, strlen(sTempCommand));
				//printf("COMMAND: %s\n", sTempCommand);
			}
			
			BufferClear(sAckBuf, sizeof(sAckBuf));
			
		}
		else
		{
			BufferClear(sAckBuf, sizeof(sAckBuf));

			UARTRead(sAckBuf, sizeof(sAckBuf));
			printf("ROBOT: %s\n", sAckBuf);
		}
	}
	
	return (0);
}

