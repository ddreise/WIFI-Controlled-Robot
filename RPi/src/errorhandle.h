/* errorhandle.h - interface of controller
 * ESE - Robot Project
 * 
 * Justin Turcotte
 * Daniel Dreise
 * Ramtin Alikhani
 * 
 * Nov 6, 2019
 */

#ifndef ERRORHANDLE_H
#define ERRORHANDLE_H

// DEFINES //

// IMPORTANT VARIABLES //
typedef enum ERR_VAL
{
	FAILURE,
	SUCCESS,
	PIPE_ERROR,
	FORK_ERROR,
	AXIS_ERROR,
	BUTTON_ERROR,
	WIFI_ERROR
}ERR_VAL;

// PROTOTYPES //


#endif