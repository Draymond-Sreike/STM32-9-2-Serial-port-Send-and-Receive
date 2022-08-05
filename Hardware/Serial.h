#ifndef __LED_H
	#define __LED_H
	
	#include <stdio.h>
	
	void serial_Init();
	void serial_SendByte(uint8_t byte);
	void serial_SendArray(uint8_t* arrayAddr, uint16_t arrayLenth);
	void serial_SendString(char* string);
	void serial_SendNum(uint32_t num, uint8_t numLength);
	uint8_t serial_GetRX_ReceiveFlag();
	uint8_t serial_GetRX_Data();
	void USART1_IRQHandler();


#endif
