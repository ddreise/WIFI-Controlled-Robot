//SysTick.c - Implementation to use the System Timer
//Justin Turcotte
//February 7, 2019

#include "SysTick.h"
#include "stm32f303xe.h"

//important variables
static volatile int32_t TimeDelay;

//FUNCTIONS
//initializes system timer for interrupts
void SysTick_Init(uint32_t ticks)
{
	SysTick->CTRL = 0;				//disable SysTick when configuring
	
	//calculation to get SysTick Interval -> Reload = Wanted_Interval * Clock_Freq - 1
	SysTick->LOAD = ticks-1;	//sets reload register with number of ticks until an interrupt
	
	//set interrupt priotity of SysTick to least urgency (largest priority value)
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	
	SysTick->VAL = 0;					//resets the SysTick counter value
	
	//select processor clock: 1 = processor clock, 0 = external clock
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	
	//enable SysTick interrupt: 1 = enable, 0 = disable
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	//enable SysTick
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

//will execute this code on interrupt -> interrupt servicde routine
void SysTick_Handler(void)
{
	if(TimeDelay > 0) TimeDelay--;	//decrement TimeDelay and prevent it from being negative
}

void Delay_us(uint32_t nTime)	//delays in us
{
	SysTick->VAL = 0;				//reset countdown timer
	TimeDelay = nTime;			//TimeDelay is volatile
	while(TimeDelay != 0);	//busy wait
}

void Delay_ms(uint32_t nTime)	//Takes nTime to specify the delay time length (ms)
{
	SysTick->VAL = 0;							//force reset of countdown timer
	TimeDelay = nTime * 1000;			//TimeDelay must be volatile, * 1000 to convert us to ms
	while(TimeDelay != 0);				//busy wait
}

void Delay_s(uint32_t nTime)	//delay in seconds
{
	SysTick->VAL = 0;							//reset countdown timer
	TimeDelay = nTime * 1000000;	//TimeDelay is volatile, * 1 000 000 to convert us to s
	while(TimeDelay != 0);				//busy wait
}
