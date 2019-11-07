/* controller.h - interface of controller
 * ESE - Robot Project
 * 
 * Justin Turcotte
 * Daniel Dreise
 * Ramtin Alikhani
 * 
 * Nov 6, 2019
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

// INCLUDES //
#include "errorhandle.h"

// DEFINES //
#define CONTROLLER_BUFFER_SIZE 1024

// IMPORTANT VARIABLES //

// PROTOTYPES //
// Gets controller input via a forked child and pipe
ERR_VAL ControllerGetInput(char *buf);
ERR_VAL ControllerInit(char *sDevicePath);


#endif