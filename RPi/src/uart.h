/* uart.h - interface of uart implementation
 * ESE - Robot Project
 * 
 * Justin Turcotte
 * Daniel Dreise
 * Ramtin Alikhani
 * 
 * Nov 6, 2019
 */

#ifndef UART_H
#define UART_H

// INCLUDES //
#include "errorhandle.h"

// DEFINES //

// PROTOTYPES //
ERR_VAL UARTInit(char *portname, int flags, int baudrate, 
                 int bits, int parity, int stopbits);

ERR_VAL UARTRead(char *dest, int destSize);
ERR_VAL UARTWrite(char *src, int srcSize);

#endif