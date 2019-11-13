/* sockets.c - implementation of socket interface
 * ESE - Robot Project
 * 
 * Justin Turcotte
 * Daniel Dreise
 * Ramtin Alikhani
 * 
 * Nov 12, 2019
 */

// INCLUDES //
#include "sockets.h"
#include "errorhandle.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// DEFINES //

// IMPORTANT VARIABLES //
int clientSocket = 0, serverSocket = 0;

// PROTOTYPES //

// FUNCTIONS //
ERR_VAL SocketClientInit(const char *hostName, u_short port)
{
	// CLIENT INIT //
 	struct sockaddr_in serverAddr;
 	struct hostent *host;
 	//const char* hostName = HOST;
 	
 	//get host info
 	host = gethostbyname(hostName);
 	if(host == NULL)
 	{
 		printf("ERROR: Could not get host by name!\n");
 		return -1;
 	}
 	else printf("SUCCESS: Host info received!\n");
 	
 	//create client socket
 	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
 	if(clientSocket == -1)
 	{
 		printf("ERROR: Could not create client socket!\n");
 		return -1;
 	}
 	else printf("SUCCESS: Client socket created!\n");
 	
 	//init serverAddr info
 	memset(&serverAddr, 0, sizeof(serverAddr));
 	serverAddr.sin_family = AF_INET;
 	serverAddr.sin_port = htons(PORT);
 	memcpy(&serverAddr.sin_addr, host->h_addr, host->h_length);
 	
 	//connect to server
 	if( connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
 	{
 		printf("ERROR: Could not connect to host!\n");
 		close(clientSocket);
 		return -1;
 	}
 	else printf("SUCCESS: Host connection acquired!\n");
 	
 	//ready for socket communication
 	printf("CLIENT SOCKET COMMUNICATION ONLINE!\n");

	return SUCCESS;
}

ERR_VAL SocketServerInit(u_short port)
{
	// SERVER INIT //
	struct sockaddr_in serverAddr, clientAddr;
	u_int client_len;
	
	//create socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);	//internet comms via socket stream with TCP
	if(serverSocket == -1)
	{
		printf("ERROR: Could not create server socket!\n");
		return -1;
	}
	else printf("SUCCESS: Server socket created!\n");
	
	//setup serverAddr structure
	memset(&serverAddr, 0, sizeof(serverAddr)); //clear for initialization
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);	//use current systems IP address info
	
	//bind socket
	if( bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	{
		printf("ERROR: Could not bind server socket to port %d!\n", PORT);
		return -1;
	}
	else printf("SUCCESS: Server socket bound to port %d!\n", PORT);
	
	//listen for connection request
	if( listen(serverSocket, 1) == -1)	//accept only 1 connection request
	{
		printf("ERROR: Listen failed!\n");
		return -1;
	}
	else printf("SUCCESS: Connection spot available!\n");
	
	//accept connection
	client_len = sizeof(clientAddr);
	clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &client_len);
	if(clientSocket == -1)
	{
		printf("ERROR: Could not accept client connection request!\n");
		close(serverSocket);
		return -1;
	}
	else printf("SUCCESS: Client connection request accepted!\n");
	
	//ready for socket communication!
	printf("SERVER SOCKET COMMUNICATIONS ONLINE!\n");

	return SUCCESS;
}

ERR_VAL SocketWrite(char *src, int srcSize)
{
	write(clientSocket, src, srcSize);

	return SUCCESS;
}

ERR_VAL SocketRead(char *dest, int destSize)
{
	read(clientSocket, dest, destSize);

	return SUCCESS;
}
