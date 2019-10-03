/* Robot_Command.h
 * 
 * Purpose: Functions to receive commands in proper protocol form from host computer
 * 
 *
 * By: Daniel Dreise
 * All Rights Reserved
 *
 * Date start: October 1, 2019
 * 
 * Semester: Fall 2019
 * Embedded Systems Software - Conestoga College
 */
 
#ifndef	ROBOT_COMMAND_H
#define ROBOT_COMMAND_H

#define ACK "$ACK%"
#define NACK "$NACK%"

#define MAX_CMD_BUFSIZ 255						// max size of the command buff size

int CMD(char *str);

#endif 