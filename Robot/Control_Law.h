// Control_Law.h
//
// Interface for functions and implementation of the Control Law loop to compensate for error using DC Motors and Encoders
//
// November 13, 2019
// By: Daniel Dreise


#ifndef CONTROL_LAW_H
#define CONTROL_LAW_H

#include "stm32f303xe.h"


volatile extern int16_t left_setpoint_dutyCycle, right_setpoint_dutyCycle;
volatile extern int16_t left_dutyCycle, right_dutyCycle;

void Control_Law_Init(void);
void Control_Law(void);


#endif
