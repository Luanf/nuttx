#ifdef __cplusplus
extern "C" {
#endif
	
#include <stdio.h>
#include <debug.h>
#include "ARCH_serial.h"

#include "vm.h"

volatile uint8_t has_command = 0;
volatile char command[3] = {0};
volatile char inBuffer[BUFFER_SZ+1];
volatile char outBuffer[BUFFER_SZ];

void send_byte(unsigned char byte)
{
	dbg("%c", byte);
}

char read_byte(void)
{
	char c = '\0';
	//scanf("%c", &c);
	return c;
}

void serial_configure(unsigned int baudrate)
{

}

void enable_commands(void) {
	
}

void printnum(uint32_t number)
{
	dbg("%d",number);
}

void print(const char *str)
{
	dbg("%s\n", str);
	printf("COISA SAYS: %s\n", str);
}
	
#ifdef __cplusplus
}
#endif

// #ifdef __cplusplus
// extern "C" {
// #endif
	
// #include "ARCH_serial.h"
// #include <stdint.h>

// volatile char rx_buff[2] = {0};
// volatile uint8_t rx_buff_pos = 0;
// // char tx_buff[2] = {0};

// volatile uint8_t has_command = 0;
// volatile char command[3] = {0};

// volatile char inBuffer[BUFFER_SZ+1] = {0};
// volatile char outBuffer[BUFFER_SZ] = {0};

// void USART1_IRQHandler()
// {
//   if(USART1->SR & USART_SR_RXNE)
//   {
// 	//Every command has 2 chars, which is enought for out TM :)
// 	//When this buffer is full, we copy it to save data and set a flag, so TM can handle it
// 	rx_buff[rx_buff_pos] = (USART1->DR & 0x1FF); //TODO: may have some sync problems, pack it later
// 	if (!has_command && ++rx_buff_pos == 2) {
// 		has_command = 1;
// 		rx_buff_pos = 0;
// 		command[0] = rx_buff[0];
// 		command[1] = rx_buff[1];
// 		NVIC_DisableIRQ(USART1_IRQn); // Stops receiving commands, so we can handle the received first
//   }
// }

// void send_byte(unsigned char byte)
// {
//    USART1->DR = data;

//    // wait for TX
//    while ((USART1->SR & USART_SR_TXE) == 0);
// }
// char read_byte(void)
// {
//    /* Wait until the data is ready to be received. */
//    while ((USART1->SR & USART_SR_RXNE) == 0);

//    // read RX data, combine with DR mask (we only accept a max of 9 Bits)
//    return USART1->DR & 0x1FF;
// }
// void serial_configure(unsigned int baudrate)
// {
// 	    RCC->APB2ENR = 0
//         // Turn on USART1
//         | RCC_APB2ENR_USART1EN
//         // Turn on IO Port A
//         | RCC_APB2ENR_IOPAEN;

//     	// Put PA9  (TX) to alternate function output push-pull at 50 MHz
//     	// Put PA10 (RX) to floating input
//     	GPIOA->CRH = 0x000004B0;
//     	USART1->BRR = 8000000/baudrate;
//     	USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
//     	NVIC_EnableIRQ(USART1_IRQn); 

// }

// void enable_commands(void) {
// 	NVIC_EnableIRQ(USART1_IRQn); 
// }

// void printnum(int32_t number)
// {
// 	if (number < 0) 
// 	{
// 		number = -number;
// 		send_byte('-');
// 	}
// 	int32_t temp = number;
// 	int32_t max = 10;
// 	int32_t counter = 0;
// 	do
// 	{
// 		temp /= 10;
// 		counter++;
// 	} while(temp > 0);
// 	for(temp = 0; temp < counter-1; temp++)
// 	{
// 		max *= 10;
// 	}
// 	for (; max > 1; max/=10)
// 	{
// 		send_byte((number%(max))/(max/10) + '0');
// 	}
// }
// void print(char *str)
// {
// 	int i = 0;
// 	while (str[i] != '\0')
// 	{
// 		send_byte(str[i++]);
// 	}
// }
	
// #ifdef __cplusplus
// }
// #endif