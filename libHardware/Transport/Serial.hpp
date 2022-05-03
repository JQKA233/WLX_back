#pragma once
#include <cstdio>
 
 
#define MAX_DEV_NAME_LENS (64)  /* 每个设备的名字最大长度 */
#define MAX_DEV_NUMS      (64)  /* 一次最大可以扫描到的设备数量 */
 
typedef struct _UART_CTRL_
{
    int fd;     //串口设备的文件描述符
    int u32DevNums;     //扫描到的串口设备的数量
    char strDevList[MAX_DEV_NUMS][MAX_DEV_NAME_LENS];   //设备列表，包含扫描到的所有串口设备
}UART_CTRL;

/* 串口初始化，需设置串口的波特率，数据位数，奇偶校验位，停止位 */
int UartInit(int nSpeed, int nBits, char nEvent, int nStop);

/* 串口发送数据 */
int UartSendData(const void *buf, size_t size);

/* 串口接收数据 */
size_t UartRecvData(void *buf, size_t size);