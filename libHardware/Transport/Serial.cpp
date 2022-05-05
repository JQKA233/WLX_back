#include "Serial.hpp"
#include <cstdio>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
 
UART_CTRL pstCtrl = {0};
 
/* 信号的注册函数 */
void CleanUpFunc(int sig_num)
{
    if(pstCtrl.fd != -1)
    {
        close(pstCtrl.fd);
    }
    printf("Program exit via signal function!\n");
    exit(1);
}
 
/* 设置串口的属性，包括波特率，数据位数，奇偶校验位，停止位 */
static int SetUartAttr(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio;
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |= CLOCAL | CREAD;   //本地连接，接收使能
    newtio.c_cflag &= ~CSIZE;           //屏蔽数据位
   
    /* 设置数据位数 */
    switch( nBits )
    {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |= CS8;
            break;
    }
   
    /* 设置奇偶校验位 */
    switch( nEvent )
    {
        case 'O':   /* 奇数校验 */
            newtio.c_cflag |= PARENB;               //有校验
            newtio.c_cflag |= PARODD;               //奇校验
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'E':   /* 偶数校验 */
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;               //有校验
            newtio.c_cflag &= ~PARODD;              //偶校验
            break;
        case 'N':   /* 无校验 */
            newtio.c_cflag &= ~PARENB;              //无校验
            break;
    }
   
    /* 设置波特率 */
    switch( nSpeed )
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
    }
   
    /* 设置停止位 */
    if( nStop == 1 )
    {
        newtio.c_cflag &= ~CSTOPB;
    }        
    else if ( nStop == 2 )
    {
        newtio.c_cflag |= CSTOPB;
    }
       
    newtio.c_cc[VTIME] = 0;      // 等待时间，单位百毫秒 （读）
    newtio.c_cc[VMIN] = 0;       // 最小字节数 （读）
    tcflush(fd, TCIFLUSH);       // 刷清输入队列，即将配置信息立即刷入系统
    if((tcsetattr(fd, TCSANOW, &newtio)) != 0)  // TCSANOW立即生效
    {
        perror("com set error");
        return -1;
    }
 
    printf("SetUartAttr exec ok!\n");
    return 0;
}
 
/* 打开串口设备 */
static int OpenUartDev(char* strDevFile)
{
    int fd = 0;
    int ret = 0;
   
    printf("Opening device: \"%s\"\n", strDevFile);
 
    /* 打开设备文件 */
    fd = open(strDevFile, O_RDWR|O_NOCTTY|O_NDELAY);
    if (-1 == fd)
    {
        perror("Can't Open Serial Port");
        return -1;
    }
 
    /* 设置文件描述符的文件状态为0，即复位文件状态 */
    ret = fcntl(fd, F_SETFL, 0);
    if(ret < 0)
    {
        printf("fcntl failed!\n");
    }
 
    /* 判断文件描述符(标准输入)是否是为终端 */
    ret = isatty(STDIN_FILENO);
    if(ret == 0)
    {
        printf("Standard input is not a terminal device\n");
    }
 
    printf("Open device ok, fd=%d, dev=%s\n", fd, strDevFile);
    return fd;
}
 
/* 获取可用的串口设备 */
static int GetAvailableUartDev(char (*strDevList)[64], size_t u32MaxItem, const char* strCmd)
{
char buf[MAX_DEV_NAME_LENS] = {0};
char tmp[MAX_DEV_NAME_LENS] = {0};
FILE *pf = NULL;
int s32Size = 0;
/* 创建管道用来执行命令 */
pf = popen(strCmd, "r");
if(NULL == pf)
{
   perror("It's failed to open the shell of cmd");
return -1;
}
 
/* 分行获取数据 */
while(fgets(buf, sizeof(buf), pf) && s32Size < u32MaxItem)
{
   sscanf(buf, "%s\n", tmp);   /* 用以剔除末尾的回车 */
   memcpy(strDevList[s32Size], tmp, sizeof(tmp));
   s32Size++;
}
/* 关闭管道 */
pclose(pf);
/* 打印获取到的数据 */
printf("Device lists as follow: (%d items)\n", s32Size);
for(int i = 0; i < s32Size; ++i)
{
   printf("%d, %s\n", i, strDevList[i]);
}
return s32Size;
}
 
/* 串口初始化 */
int UartInit(int nSpeed, int nBits, char nEvent, int nStop)
{
    int ret = -1;
 
      /* 注册信号,用于回应"Ctrl+c" */
    signal(SIGINT, CleanUpFunc);
   
    /* 获取可用串口 */
    pstCtrl.u32DevNums = GetAvailableUartDev(pstCtrl.strDevList, MAX_DEV_NUMS, "ls /dev/ttyUSB* -l | awk '{print $NF}' | sort");
    if(pstCtrl.u32DevNums <= 0)
    {
        printf("There isn't any available device be found!\n");
        return -1;
    }
   
    /* 打开串口 */
    pstCtrl.fd = OpenUartDev(pstCtrl.strDevList[0]);
    if(pstCtrl.fd < 0)
    {
        perror("open_port error");
        return -2;
    }
   
    /* 设置串口属性，包括波特率、数据位数、奇偶校验、停止位等 */
    ret = SetUartAttr(pstCtrl.fd, nSpeed, nBits, nEvent, nStop);
    if(ret < 0)
    {
        perror("SetUartAttr error");
        return -3;
    }
 
    printf("UartInit exec ok!\n");
    return 0;
}
 
/* 串口发送数据 */
int UartSendData(const void *buf, size_t size)
{
    if(NULL == buf || 0 == size)
    {
        printf("The send data is error occured! buf=%p, size=%ld", buf, size);
        return -1;
    }
 
    write(pstCtrl.fd, buf, size);
    return 0;
}
 
/* 串口接收数据 */
size_t UartRecvData(void *buf, size_t size)
{
    if(NULL == buf || 0 == size)
    {
        printf("The send data is error occured! buf=%p, size=%lu", buf, size);
        return -1;
    }
 
    ssize_t s64RecvSize = 0;
    s64RecvSize = read(pstCtrl.fd, buf, size);
    return s64RecvSize;
}