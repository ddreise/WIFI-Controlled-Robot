/*	Socket-Summary.c - summary of essential socket information from 
 *	By: Justin Turcotte
 *	Wednesday November 12, 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

/*	NOTES:
 *	- sys/types.h defines u_short as unsigned short
 *	- unix treats the socket handle as an int data type
 *	- accept() returns the socket handle, an int in UNIX and SOCKET (same as int) in windows
 *
 *	Socket Process:
 	1. socket() requires 3 parameters
 		- family -> AF_INET for internet communications
 		- type -> SOCK_STREAM for stream oriented connections
 		- protocol -> 0 for default for selected family (TCP/IP for AF_INET)
 		- this is done on both server and client side
 		- if -1 is returned, an error occured
 	2. Socket is now unbound
 	3. bind() attaches a name to the socket (IP address and port number for the given system)
 		- initialize sockaddr_in struct to setup binding (see sample code for server)
 		- note: server internet address typically htonl(INADDR_ANY) -> indicating to use current system's ip address info
 		- error indicated by -1 return
 		- htons() converts native host value into a "network" value
 	4. Binding is done -> create a queue to start the server on its way
 	5. listen()
 		- takes socket value from socket() and integer for queue size
 		- -1 return indicates error
 	6. accept() needs the socket, a pointer to sockaddr_in structure so details of incoming connection can be extracted
 		- -1 return indicates error
 		- if no error -> socket number of incoming connection is returned
 	7. Communication to remote client:
 		- use client socket number as a file handle -> allows standard UNIX read() write() close() calls
 	8. Client uses gethostbyname() to find host to communicate with
 	9. gethostbyname()
 		- returns pointer to hostent structure -> supplies details such as true name of host, aliases, the type of address host is, length of the address, list of addresses DNS know about for host
 	10. Client needs to initialize socket like server
 		- client initializes sockaddr_in structure with host address information given from gethostbyname()
 	11. connect() call from client binds client's socket with server information supplied by sockaddr_in structure
 		- -1 return indicates error (lack of server at remote end is one possible error)
 	12. NOW both server and client can read() and write() data
 	13. When finished - close sockets at both ends
 */
 
 // PROTOTYPES //
 int socket(int family, int type, int protocol);
 int bind(int socket, struct sockaddr *name, int namelength);
 int listen(int socket, int backlog);
 int accept(int socket, struct sockaddr *addr, int *addrlen);
 int connect(int socket, struct sockaddr *name, int namelength);
 struct hostent *gethostbyname(char *name);
 u_short htons(u_short host);
 int read(int socket, void *buffer, int len);
 int write(int socket, void *buffer, int len);
 int close(int socket);
  
 // STRUCTURE //
 struct sockaddr_in {
 	short sin_family;			//AF_INET
 	u_short sin_port;			//16 bit port number 0 - 65535
 	struct in_addr sin_addr;	//address structure
 	char sin_zero[0]			//for future expansion
 };

struct hostent {
	char *h_name;		//official name of host
	char **h_aliases;	//list of aliases
	int h_addrtype;		//host address type
	int h_length;		//length of address
	char **h_addr_list;	//list of addresses for this host
};

void main(void)
{
	
	
}
