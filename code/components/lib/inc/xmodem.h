#ifndef __XMODEM_H__
#define __XMODEM_H__

#include "type.h"
#include "state.h"
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************
 *                Constants
 *******************************************************/

/*
XMODEM数据包格式
---------------------------------------------------------------------------------------
|      BYTE1      |     BYTE2     |      BYTE3       | BYTE4~BYTE131 | BYTE132~BYTE133 |
---------------------------------------------------------------------------------------
| Start Of Header | Packet Number | ~(Packet Number) |  Packet Data  |    16Bit CRC    |
---------------------------------------------------------------------------------------
*/

//XMODEM协议校验和回显标志
#define XMODEM_FRM_FLAG_ADD_ECHO        0x15
//XMODEM协议CRC16回显标志
#define XMODEM_FRM_FLAG_CRC_ECHO        'c'
//XMODEM协议128字节头标志
#define XMODEM_FRM_FLAG_SOH             0x01
//XMODEM协议1K字节头标志
#define XMODEM_FRM_FLAG_STX             0x02
//XMODEM协议发送结束标志
#define XMODEM_FRM_FLAG_EOT             0x04
//XMODEM应答标志
#define XMODEM_FRM_FLAG_ACK             0x06
//XMODEM非应答标志
#define XMODEM_FRM_FLAG_NAK             0x15
//XMODEM取消发送标志
#define XMODEM_FRM_FLAG_CAN             0x18
//XMODEM使用CRC16校验标志
#define XMODEM_FRM_FLAG_CRC16           0x43
//XMODEM填充数据包标志
#define XMODEM_FRM_FLAG_CTRLZ           0x1A


//XMODEM数据长度
#define XMODEM_DATA_LEN_128             128
#define XMODEM_DATA_LEN_1K              1024

//XMODEM字段关键字长度
#define XMODEM_FRM_FLAG_LEN_ADD         4
#define XMODEM_FRM_FLAG_LEN_CRC         5

//XMODEM缓冲区长度配置
#define XMODEM_BUF_LEN_128_ADD          (XMODEM_DATA_LEN_128+XMODEM_FRM_FLAG_LEN_ADD)
#define XMODEM_BUF_LEN_128_CRC          (XMODEM_DATA_LEN_128+XMODEM_FRM_FLAG_LEN_CRC)
#define XMODEM_BUF_LEN_1K_ADD           (XMODEM_DATA_LEN_1K+XMODEM_FRM_FLAG_LEN_ADD)
#define XMODEM_BUF_LEN_1K_CRC           (XMODEM_DATA_LEN_1K+XMODEM_FRM_FLAG_LEN_CRC)
#define XMODEM_BUF_LEN_MAX              XMODEM_BUF_LEN_1K_CRC

//XMODEM接收超时时间
#define XMODEM_RX_TIMEOUT_MS            1000

//XMODEM等待次数
#define XMODEM_WAIT_TIMES               80

//XMODEM接收尝试次数
#define XMODEM_RX_RETRY_TIMES           5

/*******************************************************
 *                Type Definitions
 *******************************************************/
typedef enum xmodem_rx_st       xmodem_rx_st_t;
typedef enum xmodem_chk_md      xmodem_chk_md_t;
typedef enum xmodem_data_md     xmodem_data_md_t;
typedef enum xmodem_req         xmodem_req_t;
typedef enum xmodem_state       xmodem_state_t;

/*======================================================
函数名:xmodem_tx_cb_t
参数:
    buf:缓冲区
    len:发送长度
返回值:
    STATE_FALSE失败
    STATE_TRUE成功
说明:
    XMODEM发送回调函数类型，XMODEM数据发送过程
=======================================================*/
typedef type_err (*xmodem_tx_cb_t)(type_uint8 *buf, type_uint16 len);

/*======================================================
函数名:xmodem_rx_st_t
参数:
    buf:缓冲区
    buf_len:需要接收长度
    len:实际长度
    timeout:超时时间，毫秒
返回值:
    XMODEM_RX_ST_OK表示接收成功
    XMODEM_RX_ST_TIMEOUT表示接收超时
    XMODEM_RX_ST_WAIT表示接收等待
说明:
    XMODEM接收回调函数类型，XMODEM数据接收过程，支持阻塞和非阻塞
=======================================================*/
typedef xmodem_rx_st_t (*xmodem_rx_cb_t)(type_uint8 *buf, type_uint16 buf_len, type_uint16 *len, type_uint16 timeout);

/*======================================================
函数名:xmodem_cb_t
参数:
    buf:缓冲区
    len:实际长度
返回值:无
说明:
    XMODEM数据回调函数类型，XMODEM调用此函数向上层通知接收到数据
    回调函数通过以下参数告知当前数据状态：
    buf为空     len为0------>表示传输故障，所有传输数据无效
    buf不为空   len不为0----->表示当前数据有效
    buf不为空   len为0------>表示当前传输完成
=======================================================*/
typedef void (*xmodem_cb_t)(type_uint8 *buf, type_uint16 len);

//定义XMODEM对象类型
typedef struct xmodem       xmodem_t;
 /*******************************************************
 *                Structures
 *******************************************************/

//数据接收状态
enum xmodem_rx_st
{
    XMODEM_RX_ST_IDLE,          //接收空闲
    XMODEM_RX_ST_WAIT,          //接收等待，只用于非阻塞接收
    XMODEM_RX_ST_TIMEOUT,       //接收超时
    XMODEM_RX_ST_OK,            //接收完成
};

//XMODEM数据模式配置
enum xmodem_data_md
{
    XMODEM_DATA_128,            //128字节数据
    XMODEM_DATA_1024            //1024字节数据
};

//XMODEM校验模式配置
enum xmodem_chk_md
{
    XMODEM_CHK_MD_ADD,          //累加和校验
    XMODEM_CHK_MD_CRC,          //CRC校验
    XMODEM_CHK_MD_MAX
};

//XMODEM请求
enum xmodem_req
{
    XMODEM_REQ_NONE,            //无请求
    XMODEM_REQ_START,           //开始请求
    XMODEM_REQ_STOP,            //停止请求
    XMODEM_REQ_MAX
};

//XMMODE工作状态
enum xmodem_state
{
    XMODEM_STATE_IDLE,              //XMODEM空闲状态
    XMODEM_STATE_STANDBY,           //XMODEM就绪状态
    XMODEM_STATE_RECEIVING,         //XMODEM接收状态
    XMODEM_STATE_FINISH,            //XMODEM完成状态
    XMODEM_STATE_CANCEL,            //XMODEM取消状态
    XMODEM_STATE_STOP,              //XMODEM结束状态
    XMODEM_STATE_UNKNOW,
};

struct xmodem
{
    //XMODEM数据模式
    xmodem_data_md_t data_md;
    //XMODEM校验模式
    xmodem_chk_md_t chk_md;
    //XMODEM请求
    xmodem_req_t req;
    //保存当前XMODE状态
    xmodem_state_t state;
    //保存传输等待次数
    type_uint16 wait_times;
    //保存当前传输等待计数
    type_uint16 wait_cnt;
    //保存当前接收等待状态
    xmodem_rx_st_t rx_state;
    //保存接收超时次数
    type_uint16 rx_retry;
    //保存接收超时次数计数
    type_uint16 rx_retry_cnt;
    //XMODE接口发送回调函数
    xmodem_tx_cb_t tx_cb;
    //XMODEM接口接收回调函数
    xmodem_rx_cb_t rx_cb;
    //XMODEM数据接收回调函数
    xmodem_cb_t callback;
    //XMODEM接收数据缓冲区
    type_uint8 buf[XMODEM_BUF_LEN_MAX];
    //XMODEM当前接收数据长度
    type_uint16 length;
    //XMODEM帧计数
    type_uint8 frm_cnt;
};
/*******************************************************
 *                Variables Declarations
 *******************************************************/

/*******************************************************
 *                Function Definitions
 *******************************************************/
//XMODEM初始化
type_err xmodem_init(xmodem_t *obj);
//XMODEM发送回调函数注册
type_err xmodem_tx_cb_reg(xmodem_t *obj, xmodem_tx_cb_t cb);
//XMODEM接收回调函数注册
type_err xmodem_rx_cb_reg(xmodem_t *obj, xmodem_rx_cb_t cb);
//XMODEM数据接收回调注册函数
type_err xmodem_cb_reg(xmodem_t *obj, xmodem_cb_t cb);
//XMODEM请求
type_err xmodem_req(xmodem_t *obj, xmodem_req_t req);
//XMODEM处理过程
void xmodem_proc(xmodem_t *obj);
//XMODEM获取当前状态
xmodem_state_t xmodem_state_get(xmodem_t *obj);
//XMODEM卸载
void xmodem_deinit(xmodem_t *obj);

#ifdef __cplusplus
}
#endif

#endif
