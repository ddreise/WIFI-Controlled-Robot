/* pipes.c - implementation of pipe interface
 * ESE - Robot Project
 * 
 * Justin Turcotte
 * Daniel Dreise
 * Ramtin Alikhani
 * 
 * Nov 6, 2019
 */

// INCLUDES //
#include "pipes.h"
#include "errorhandle.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// DEFINES //


// FUNCTIONS //
// Initialize a pipe for use
ERR_VAL PipeInit(int *fd)
{
	if(pipe(fd) != 0) //pipe creation error
	{
		return FAILURE;
	}

	return SUCCESS;
}

