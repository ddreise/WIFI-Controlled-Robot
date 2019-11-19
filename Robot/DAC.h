
#ifndef DAC_H
#define DAC_H

#include "stm32f303xe.h"

#define DAC_PORT A
#define DAC_PIN 4

void DAC_Init(void);
void DAC_output(uint16_t wheel_speed);

#endif
