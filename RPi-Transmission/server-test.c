/*	server-test.c - test implementation for socket communications to a RPi from Linux host
 *	By: Justin Turcotte
 *	Wednesday November 12, 2019
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 5000	//port to use for socket communications

int main(void)
{
	// SERVER INIT //
	int serverSocket = 0, clientSocket = 0;
	struct sockaddr_in serverAddr, clientAddr;
	int client_len, i;
	FILE *p;
	
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
	
	return 0;
}
