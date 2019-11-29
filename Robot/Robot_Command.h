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

#include "stm32f303xe.h"

#define ACK "$ACK%"
#define NACK "$NACK%"


#define MAX_CMD_BUFSIZ 255						// max size of the command buff size

extern int16_t servo_position;
extern uint16_t left_speed;
extern uint16_t right_speed;

int CMD(char *str);

int stop_Robot(void);
uint32_t get_left_driveValue(void);
uint32_t get_right_driveValue(void);


#endif 
