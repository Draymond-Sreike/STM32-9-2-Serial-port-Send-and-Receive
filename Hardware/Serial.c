#include <stdio.h>
#include "stm32f10x.h"                  // Device header

uint8_t data;
uint8_t receiveFlag;

void serial_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	//校验位配置
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//开启USART_RXNE到NVIC的中断通道,当STM32中USART接收到数据时申请中断进行处理
	//现在一旦RXNE标志位置1（即接收到数据并且接收移位寄存器接收后将其存储到了接收数据寄存器）就会申请中断
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	/***********配置NVIC***********/
	//先分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}

void serial_SendByte(uint8_t byte)
{
	USART_SendData(USART1, byte);
	//while循环等待发送数据寄存器将数据写入至发送移位寄存器，不等待可能导致写入数据过多过快，从而导致数据被覆盖
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void serial_SendArray(uint8_t* arrayAddr, uint16_t arrayLenth)
{
	uint16_t i;
	for(i = 0; i < arrayLenth; i++)
	{
		USART_SendData(USART1, arrayAddr[i]);
		//while循环等待发送数据寄存器将数据写入至发送移位寄存器，不等待可能导致写入数据过多过快，从而导致数据被覆盖
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}	
}

void serial_SendString(char* string)
{
	uint16_t i;
	for(i = 0; string[i] != 0; i++)
	{
		USART_SendData(USART1, string[i]);
		//while循环等待发送数据寄存器将数据写入至发送移位寄存器，不等待可能导致写入数据过多过快，从而导致数据被覆盖
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}

uint32_t serial_Pow(uint32_t num, uint8_t pow)
{
	uint32_t result = 1;
	while(pow--)
	{
		result *= num;
	}
	return result;
}

void serial_SendNum(uint32_t num, uint8_t numLength)
{
	uint8_t i;
	for(i = 1; i <= numLength; i++)
	{
		//假设输入的数字是12345，长度选2，则第一次循环输出4，需要10的1次方
		//第二次循环输出5，需要10的0次方
		//综上所述循环中需写入（length-i）次方，模10得到对应数字，再加上ASCII的编码偏移
		//使得“值”变为相应数字字符的ASCII码值，便于文本显示
		USART_SendData(USART1, (num / serial_Pow(10, numLength - i)) % 10 + '0');
		//while循环等待发送数据寄存器将数据写入至发送移位寄存器，不等待可能导致写入数据过多过快，从而导致数据被覆盖
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}

//重写printf函数的底层函数fputc()，他会指定printf的输出位置，默认是到显示屏，但STM32没有显示屏
//所以我们这里将输出位置转移到串口
int fputc(int ch, FILE* f)
{
	USART_SendData(USART1, ch);
	//while循环等待发送数据寄存器将数据写入至发送移位寄存器，不等待可能导致写入数据过多过快，从而导致数据被覆盖
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	return ch;
}

uint8_t serial_GetRX_ReceiveFlag()
{
	if(receiveFlag == 1)
	{
		receiveFlag = 0;
		return 1;
	}
	return 0;
}
uint8_t serial_GetRX_Data()
{
	return data;
}

void USART1_IRQHandler()
{
	if(USART_GetFlagStatus(USART1, USART_IT_RXNE) == SET)
	{
		data = USART_ReceiveData(USART1);
		receiveFlag = 1;
	}
}
