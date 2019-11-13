/*	client-test.c - test implementation for socket communications to a RPi from Linux host
 *	By: Justin Turcotte
 *	Wednesday November 12, 2019
 */
 
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
 
 #define PORT 5000
 #define HOST "raspberrypi"
 
 int main(void)
 {
 	// CLIENT INIT //
 	int clientSocket;
 	struct sockaddr_in serverAddr;
 	struct hostent *host;
 	
 	//get host info
 	host = gethostbyname(HOST);
 	if(host == NULL)
 	{
 		printf("ERROR: Could not get host by name!\n");
 		return -1;
 	}
 	else printf("SUCCESS: Host info received!");
 	
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
 	
 	return -1;
 }
