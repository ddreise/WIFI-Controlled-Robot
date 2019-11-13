/* sockets.h - interface of socket implementation
 * ESE - Robot Project
 * 
 * Justin Turcotte
 * Daniel Dreise
 * Ramtin Alikhani
 * 
 * Nov 12, 2019
 */

#ifndef SOCKETS_H
#define SOCKETS_H

// INCLUDES //
#include "errorhandle.h"

#include <sys/types.h>

// DEFINES //
#define PORT 5000
#define HOST "10.192.158.130"	//RPi IPV4 address
#define BUFFER_SIZE 256

// PROTOTYPES //
ERR_VAL SocketClientInit(const char *hostName, u_short port);
ERR_VAL SocketServerInit(u_short port);
ERR_VAL SocketWrite(char *src, int srcSize);
ERR_VAL SocketRead(char *dest, int destSize);

#endif