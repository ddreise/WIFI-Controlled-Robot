//uartsetup.h - Implementation to initialize and handle UART operations
//By: Justin Turcotte
//Assisted by: Bill - ESE4 EMBSOFT Prof
//January 20, 2019

#include <stdarg.h>
#include <stdio.h>
#include "stm32f303xe.h"
#include "UART.h"
#include "Macros.h"

// STATIC VARIABLES //
static char USART1_Buffer_Rx[MAX_UART_BUFSIZE];
static volatile uint32_t Rx1_Counter = 0;

// STATIC FUNCTIONS //
static void Receive(void)
{
		USART1_Buffer_Rx[Rx1_Counter] = USART1->RDR;
		(Rx1_Counter)++;
		if((Rx1_Counter) >= MAX_UART_BUFSIZE)
		{
			(Rx1_Counter) = 0;
		}
}

// INTERRUPTS //

void USART1_IRQHandler(void)
{
	Receive();
}


// FUNCTIONS //

void UARTinit(void) {
	
// *******************************************
// *** turn on clocks for USART 1 - 72 MHz ***
// *******************************************
	
	// Enable the clock of USART 1
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;  // Enable USART 1 clock		
	// Select the USART1 clock source
	// 00: PCLK selected as USART2 clock
	// 01: System clock (SYSCLK) selected as USART1 clock
	// 10: HSI16 clock selected as USART1 clock
	// 11: LSE clock selected as USART1 clock
	RCC->CFGR3 &= ~RCC_CFGR3_USART1SW;
	RCC->CFGR3 |=  RCC_CFGR3_USART1SW_0;

// ***************************************************************************************	
// *** configure GPIO port C so that bits 4 and 5 are connected to Tx and Rx of USART1 ***	
// ***************************************************************************************
	// PC4 = USART1_TX (AF7)
	// PC5 = USART1_RX (AF7)
	// Alternate function, High Speed, Push pull, Pull up
	// **********************************************************	
		
	// Enable the peripheral clock of GPIO Port B 
	GPIO_PORT_ENABLE(UART_PORT);
	
	GPIO_PIN_MODE(UART_PORT, UART_PIN_DTX, AF);
	GPIO_PIN_MODE(UART_PORT, UART_PIN_DRX, AF);
	
	GPIOC->AFR[0] &= ~(0xF << (UART_PIN_DTX * 4));
	GPIOC->AFR[0] &= ~(0xF << (UART_PIN_DRX * 4));
	
	GPIOC->AFR[0] |= 0x7 << (UART_PIN_DTX * 4);
	GPIOC->AFR[0] |= 0x7 << (UART_PIN_DRX * 4);
	
	GPIO_PIN_SPEED(UART_PORT, UART_PIN_DTX, OSPEEDR_HIGH);
	GPIO_PIN_SPEED(UART_PORT, UART_PIN_DRX, OSPEEDR_HIGH);
	
	GPIO_PIN_PUPD(UART_PORT, UART_PIN_DTX, PUPDR_PU);
	GPIO_PIN_PUPD(UART_PORT, UART_PIN_DRX, PUPDR_PU);
	
	GPIO_PIN_DRV_TYPE(UART_PORT, UART_PIN_DTX, OTYPER_PUSH_PULL);
	GPIO_PIN_DRV_TYPE(UART_PORT, UART_PIN_DRX, OTYPER_PUSH_PULL);

// ***************************************************************************************	
// *** Configure USART1 as:
// ***      No hardware flow control, 8 data bits, no parity, 1 start bit and 1 stop bit
// ***************************************************************************************	
	
	USART1->CR1 &= ~USART_CR1_UE;  // Disable USART
	
	// Configure word length to 8 bit
	USART1->CR1 &= ~USART_CR1_M;   // M: 00 = 8 data bits, 01 = 9 data bits, 10 = 7 data bits
	
	// Configure oversampling mode: Oversampling by 16 
	USART1->CR1 &= ~USART_CR1_OVER8;  // 0 = oversampling by 16, 1 = oversampling by 8
	
	// Configure stop bits to 1 stop bit
	//   00: 1 Stop bit;      01: 0.5 Stop bit
	//   10: 2 Stop bits;     11: 1.5 Stop bit
	USART1->CR2 &= ~USART_CR2_STOP;   
                                    
	// CSet Baudrate to 9600 using APB frequency (72,000,000 Hz)
	// If oversampling by 16, Tx/Rx baud = f_CK / USARTDIV,  
	// If oversampling by 8,  Tx/Rx baud = 2*f_CK / USARTDIV
  // When OVER8 = 0, BRR = USARTDIV
	// USARTDIV = 72MHz/9600 = 7500 = 0x208D
	USART1->BRR  = 0x1D4C; // Limited to 16 bits

	USART1->CR1  |= (USART_CR1_RE | USART_CR1_TE);  	// Transmitter and Receiver enable
	
	USART1->CR3 |= USART_CR3_OVRDIS;
	
	USART1->CR1  |= USART_CR1_UE; // USART enable                 
	
	while ( (USART1->ISR & USART_ISR_TEACK) == 0); // Verify that the USART is ready for reception
	while ( (USART1->ISR & USART_ISR_REACK) == 0); // Verify that the USART is ready for transmission
	
	// Enable Interrupt
	USART1->CR1 |= USART_CR1_RXNEIE;
	
	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);
}

//Sends 8bits to USART1 on PB6 when ready
void UARTputc(char cx)
{
	while(!(USART1->ISR & USART_ISR_TXE));
	USART1->TDR = (uint8_t)cx;
}

//Sends a single string to USART1 on PB6
void UARTputs(char *str)
{
	while(*str) UARTputc(*str++);
}

//Sends a string formatted similarly to printf
void UARTprintf(char* params, ...)
{
	char buf[MAX_UART_BUFSIZE];												//buffer to store final string output
	
	va_list args;																			//parameter for functions retrieving variable arguments
	va_start(args, params);														//initializes args to retrieve argument inputted after params
	
	vsnprintf(buf, MAX_UART_BUFSIZE, params, args);		//formats input and arguments as if it were a printf
	
	va_end(args);																			//performs actions necessary to fascilitate a return when using a vs_list
	
	UARTputs(buf);
}

void USART_Read(USART_TypeDef *USARTx, uint8_t *buffer, uint32_t nBytes)
{
	uint32_t i = 0;
	
	for(i = 0; i < nBytes; i++)
	{
		while (!(USARTx->ISR & USART_ISR_RXNE));	//wait until hardware sets RXNE
		buffer[i] = USARTx->RDR;									//reading RDR clears RXNE
	}
}

char* Get_Buffer(void)
{
	return USART1_Buffer_Rx;
}
