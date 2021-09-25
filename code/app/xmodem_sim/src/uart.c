
#include "uart.h"
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

/*******************************************************
 *                Structures
 *******************************************************/

/*******************************************************
 *                Variables Declarations
 *******************************************************/

/*******************************************************
 *                Function Definitions
 *******************************************************/
type_int32 uart_open(type_char *port)
{
    type_int32 fd=-1;
    return open(port,O_RDWR|O_NOCTTY);
}

void uart_close(type_int32 fd)
{
    close(fd);
}

type_err uart_set(type_int32 fd,uart_baud_t baud, uart_flowctrl_t flow_ctrl,type_uint8 databits,type_int32 stopbits,uart_parity_t parity)
{
    type_int32 status;

    struct termios options;
       
    //获取串口参数
    if(tcgetattr(fd,&options)!=0)
        return STATE_FAIL;

    switch(baud)
    {
        case UART_BAUD_9600:
            cfsetispeed(&options, B9600);
            cfsetospeed(&options, B9600);
        break;
        case UART_BAUD_115200:
            cfsetispeed(&options, B115200);
            cfsetospeed(&options, B115200);
        break;
        default:
            return STATE_FAIL;
    }

    //修改控制模式，保证程序不会占用串口
    options.c_cflag |= CLOCAL;
    //修改控制模式，使得能够从串口中读取输入数据
    options.c_cflag |= CREAD;

    options.c_iflag &= ~ (INLCR | ICRNL | IGNCR);
    options.c_oflag &= ~ (ONLCR | OCRNL | IXOFF);
    options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    //设置数据流控制
    switch(flow_ctrl)
    {    
          
        case UART_FLOWCTRL_NONE:
              options.c_cflag &= ~CRTSCTS;
        break;
        case UART_FLOWCTRL_HW:
              options.c_cflag |= CRTSCTS;
        break;
        case UART_FLOWCTRL_SW:
              options.c_cflag |= IXON | IXOFF | IXANY;
        break;
        default:
            return STATE_FAIL;
    }

    //设置数据位
    //屏蔽其他标志位
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
        case 5:
            options.c_cflag |= CS5;
        break;
        case 6:
            options.c_cflag |= CS6;
        break;
        case 7:
            options.c_cflag |= CS7;
        break;
        case 8:
            options.c_cflag |= CS8;
        break;
        default:
            return STATE_FAIL;
    }
    //设置校验位
    switch (parity)    
    {
        case UART_PARITY_NONE:
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
        break;     
        case UART_PARITY_ODD:
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;
        break;
        case UART_PARITY_EVEN:
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
        break;
        case UART_PARITY_SPACE:
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
        break;     
        default:
            return STATE_FAIL;
    }

    // 设置停止位
    switch (stopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
        break;
        case 2:
            options.c_cflag |= CSTOPB;
        break;
        default:
            return STATE_FAIL;
    }

    //修改输出模式，原始数据输出
    options.c_oflag &= ~OPOST;
      
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //options.c_lflag &= ~(ISIG | ICANON);
       
    //设置等待时间和最小接收字符
    options.c_cc[VTIME] = 40; /* 读取一个字符等待1*(1/10)s */
    options.c_cc[VMIN] = 0;

    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读
    tcflush(fd,TCIFLUSH);

    //激活配置 (将修改后的termios数据设置到串口中）
    if (tcsetattr(fd,TCSANOW,&options) != 0)
        return STATE_FAIL;
    return STATE_OK;
}

type_int32 uart_read(type_int32 fd, type_char *rcv_buf,type_int32 data_len)
{
    return read(fd,rcv_buf,data_len);
}

type_int32 uart_write(type_int32 fd, type_char *send_buf,type_int32 data_len)
{
    return write(fd,send_buf,data_len);
}
 
#ifdef __cplusplus
}
#endif