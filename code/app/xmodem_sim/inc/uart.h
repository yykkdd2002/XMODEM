#ifndef  __USART_H__
#define  __USART_H__
 
//串口相关的头文件
#include<stdio.h>      /*标准输入输出定义*/
#include<stdlib.h>     /*标准函数库定义*/
#include<unistd.h>     /*Unix 标准函数定义*/
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>      /*文件控制定义*/
#include<termios.h>    /*PPSIX 终端控制定义*/
#include<errno.h>      /*错误号定义*/
#include<string.h>
#include "type.h"
#include "state.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************
 *                Constants
 *******************************************************/

/*******************************************************
 *                Type Definitions
 *******************************************************/
typedef enum uart_baud      uart_baud_t;
typedef enum uart_flowctrl  uart_flowctrl_t;
typedef enum uart_parity    uart_parity_t;
/*******************************************************
 *                Structures
 *******************************************************/
enum uart_baud
{
    UART_BAUD_9600,
    UART_BAUD_115200
};

enum uart_flowctrl
{
    UART_FLOWCTRL_NONE,
    UART_FLOWCTRL_HW,
    UART_FLOWCTRL_SW
};

enum uart_parity
{
    UART_PARITY_NONE,
    UART_PARITY_ODD,
    UART_PARITY_EVEN,
    UART_PARITY_SPACE
};

/*******************************************************
 *                Variables Declarations
 *******************************************************/

/*******************************************************
 *                Function Definitions
 *******************************************************/
type_int32 uart_open(type_char *port);
void uart_close(type_int32 fd) ; 
type_err uart_set(type_int32 fd,uart_baud_t baud,uart_flowctrl_t flow_ctrl,type_uint8 databits,type_int32 stopbits,uart_parity_t parity);
type_int32 uart_read(type_int32 fd, type_char *rcv_buf,type_int32 data_len);
type_int32 uart_write(type_int32 fd, type_char *send_buf,type_int32 data_len);

#ifdef __cplusplus
}
#endif

#endif