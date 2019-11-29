/* buffers.c - implementation of helpful buffer functions
 * ESE - Robot Project
 * 
 * Justin Turcotte
 * Daniel Dreise
 * Ramtin Alikhani
 * 
 * Nov 6, 2019
 */

// INCLUDES //
#include "buffers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// DEFINES //


// FUNCTIONS //

// Clears a buffer - initing its values to 0 //
void BufferClear(char *buf, int bufsize)
{
	memset(buf, 0, bufsize);
}
