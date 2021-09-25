#include "xmodem.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <termio.h>
#include "type.h"
#include "uart.h"
#include <pthread.h>

//xmodem数据发送过程
type_err xmodem_tx(type_uint8 *buf, type_uint16 len);
//xmodem数据接收过程
xmodem_rx_st_t xmodem_rx(type_uint8 *buf, type_uint16 buf_len, type_uint16 *len, type_uint16 timeout);
//xmodem数据回调
void xmodem_cb(type_uint8 *buf, type_uint16 len);

type_int32 uart_fd=-1;
pthread_t ntid;

//定义XMODEM对象
static xmodem_t xmodem_obj={
    XMODEM_DATA_128,
    XMODEM_CHK_MD_ADD,
    XMODEM_REQ_NONE,
    XMODEM_STATE_IDLE,
    80,                 //等待80次
    0,
    XMODEM_RX_ST_IDLE,
    10,                 //超时10次
    0,
    xmodem_tx,          //注册发送过程
    xmodem_rx,          //注册接收过程
    xmodem_cb,          //注册回调过程
    {0},                //缓冲区
    0,                  //当前接收长度
    0,                  //帧计数
};

//xmodem发送过程，配置给串口
type_err xmodem_tx(type_uint8 *buf, type_uint16 len)
{
    uart_write(uart_fd,(type_char *)buf,len);
    return STATE_OK;
}

//xmodem接收过程，配置给串口
xmodem_rx_st_t xmodem_rx(type_uint8 *buf, type_uint16 buf_len, type_uint16 *len, type_uint16 timeout)
{
    type_int16 rx_len=0;
    type_uint8 data;
    printf("RECEIVING\n\r");
    //循环接收，直到满一帧数据
    while(rx_len<buf_len)
    {
        //阻塞方式接收，每次接收一个字节，若超时则返回超时
        if(uart_read(uart_fd,(type_char *)&data,1)<=0)
        {
            printf("RX TIMEOUT\n\r");
            return XMODEM_RX_ST_TIMEOUT;
        }
        //若收到EOT则直接返回接收完成
        else if((0x04==data)&&(0==rx_len))
        {
            buf[rx_len]=data;
            rx_len++;
            *len=rx_len;
            printf("NEW PACKAGE,EOT\n\r");
            return XMODEM_RX_ST_OK;
        }
        //接收数据
        else
        {
            buf[rx_len]=data;
            rx_len++;
        }
    }
    //收满一帧，返回接收完成
    *len=rx_len;
    printf("NEW PACKAGE,id=%d\n\r",buf[1]);
    return XMODEM_RX_ST_OK;
}

//上层接收函数
void xmodem_cb(type_uint8 *buf, type_uint16 len)
{
    type_uint16 pos=0;
    //若传递空缓冲，则显示错误
    if(!buf)
    {
        printf("XMODEM ERR!\n\r");
        return;
    }
    //若缓冲有效且长度为0，则显示接收完成
    if((buf)&&(0==len))
    {
        printf("XMODEM FINISH!\n\r");
        return;
    }
    //打印接收数据
    printf("===============================================\n\r");
    printf("00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15\n\r");
    printf("===============================================\n\r");
    while(pos<len)
    {
        for(type_uint8 i=0;i<16;i++)
        {
            if(i==15)
                printf("%02x",buf[pos++]);
            else
                printf("%02x,",buf[pos++]);
            if(pos>=len)
            {
                printf("\n\r");
                return;
            }
        }
        printf("\n\r");
    }
}

//读取按键，无需回车
type_int16 getch()
{
    type_int16 key;
    struct termios tm;
    struct termios tm_old;
    if (tcgetattr(0,&tm)<0)
        return -1;
    tm_old=tm;
    cfmakeraw(&tm);
    if(tcsetattr(0,TCSANOW,&tm)<0)
    {
        return -1;
    }
    key=getchar();
    if(tcsetattr(0,TCSANOW,&tm_old)<0)
    {
        return -1;
    }
    return key;
}

//xmodem处理线程
void *xmodem_pth(void *arg)
{
    while(1)
    {
        xmodem_proc(&xmodem_obj);
    }
}

void main()
{
    int key;
    type_char lf=0x0A;
    //打开串口，此处根据实际情况填写
    uart_fd=uart_open("/dev/ttyUSB0");
    if(0>uart_fd)
    {
        printf("UART OPEN ERR！\n\r");
        return;
    }
    //串口配置
    if(uart_set(uart_fd,UART_BAUD_115200,UART_FLOWCTRL_NONE,8,1,UART_PARITY_NONE)!=STATE_OK)
    {
        printf("UART CFG ERR！\n\r");
        uart_close(uart_fd);
        return;
    }
    //初始化xmodem对象
    xmodem_init(&xmodem_obj);
    //创建xmodem处理线程
    pthread_create(&ntid, NULL, xmodem_pth, NULL);
    while(1)
    {
        printf("************************************\n\r");
        printf("Press X to start XMODEM transmission\n\r");
        printf("Press S to stop XMODEM transmission\n\r");
        printf("Press E to exit\n\r");
        printf("************************************\n\r");
        while(1)
        {
            //获取按键
            key=getch();
            switch(key)
            {
                case 'X':
                case 'x':
                    //请求开始xmodem传输
                    printf("XMODEM START REQ\n\r");
                    tcflush(uart_fd,TCIFLUSH);
                    xmodem_req(&xmodem_obj,XMODEM_REQ_START);
                    break;
                case 'S':
                case 's':
                    //请求结束xmodem传输
                    printf("XMODEM STOP REQ\n\r");
                    xmodem_req(&xmodem_obj,XMODEM_REQ_STOP);
                    break;
                case 'E':
                case 'e':
                    uart_close(uart_fd);
                    exit(0);
                default:
                    break;
            }
        }
    }


}