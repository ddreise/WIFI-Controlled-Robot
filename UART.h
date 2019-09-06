//uartsetup.h - Interface to initialize and handle UART operations
//By: Justin Turcotte
//Assisted by: Bill - ESE4 EMBSOFT Prof
//January 20, 2019

#ifndef UARTSETUP_H
#define UARTSETUP_H

// PIN LAYOUT //
#define UART_PORT			C
#define UART_PIN_DTX	4
#define UART_PIN_DRX	5
#define UART_PIN_RTS	10
#define UART_PIN_CTS	11

#define MAX_UART_BUFSIZE 32UL

void UARTinit(void);									//initializes USART1
void UARTputc(char cx);								//output a character to USART1
void UARTputs(char *str);							//output a string to USART1
void UARTprintf(char* params, ...);		//output "Justin Turcotte\ncount: 0xXXXX" with the hex value incrementing
void USART_Read(USART_TypeDef *USARTx, uint8_t *buffer, uint32_t nBytes);
void USART1_IRQHandler(void);
char* Get_Buffer(void);

#endif
