#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"

int main()
{
	OLED_Init();
	serial_Init();
	OLED_ShowString(1, 3, "Hello World!");
	uint8_t serialData;
	while(1)
	{
		if(serial_GetRX_ReceiveFlag() == 1)
		{
			serialData = serial_GetRX_Data();
			serial_SendByte(serialData);
			OLED_ShowHexNum(2, 1, serialData, 2);			
		}
//		//查询方法接收串口数据
//		if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
//		{
//			OLED_ShowHexNum(2, 1, USART_ReceiveData(USART1), 2);
//		}
	}
}
