#include "type.h"
#include "state.h"
#include "xmodem.h"
#include "crc.h"
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************
 *                Constants
 *******************************************************/
#define xmodem_debug       0
/*******************************************************
 *                Type Definitions
 *******************************************************/
typedef enum xmodem_rx_res        xmodem_rx_res_t;
/*******************************************************
 *                Structures
 *******************************************************/
//接收数据处理状态
enum xmodem_rx_res
{
    XMODEM_RX_RES_ERR,                  //接收数据错误
    XMODEM_RX_RES_OK,                   //接收数据成功
    XMODEM_RX_RES_FAULT,                //接收过程故障
    XMODEM_RX_RES_FINISH                //接收数据完成
};
/*******************************************************
 *                Variables Declarations
 *******************************************************/

/*******************************************************
 *                Function Definitions
 *******************************************************/

/*======================================================
函数名:xmodem_init
参数:
    obj:XMODEM传输对象
返回值:
    STATE_OK表示成功
    STATE_FAIL表示失败
说明:
    XMODEM对象初始化过程
=======================================================*/
type_err xmodem_init(xmodem_t *obj)
{
    if(!obj)
        return STATE_FAIL;
    //如果模式非法，则默认配置为CRC模式
    if(obj->chk_md>=XMODEM_CHK_MD_MAX)
        obj->chk_md=XMODEM_CHK_MD_CRC;
    //初始化状态
    obj->req=XMODEM_REQ_NONE;
    //初始化XMODEM状态
    obj->state=XMODEM_STATE_IDLE;
    //初始化当前传输等待计数
    obj->wait_cnt=1;
    //初始化XMODEM接收状态
    obj->rx_state=XMODEM_RX_ST_IDLE;
    //初始化接收超时次数计数
    obj->rx_retry_cnt=1;
    //检查回调函数
    if((!obj->tx_cb)||(!obj->rx_cb))
        return STATE_FAIL;
    //清除接收缓冲区
    for(type_uint16 i=0;i<XMODEM_BUF_LEN_MAX;i++)
        obj->buf[i]=0;
    //清除当前接收数据长度
    obj->length=0;
    //清除帧计数
    obj->frm_cnt=1;
    return STATE_OK;
}

/*======================================================
函数名:xmodem_tx_cb_reg
参数:
    obj:XMODEM传输对象
    cb:XMODEM数据发送回调函数
返回值:
    STATE_OK表示成功
    STATE_FAIL表示失败
说明:
    XMODEM通信数据发送回调函数注册，注册成功后XMODEM通过调用此
    函数向对方发送数据，需要注意的是，只有在XMODEM对象空闲，即
    XMODEM_STATE_IDLE状态时，才能注册发送回调函数
=======================================================*/
type_err xmodem_tx_cb_reg(xmodem_t *obj, xmodem_tx_cb_t cb)
{
    if((!obj)||(XMODEM_STATE_IDLE!=obj->state)||(!cb))
        return STATE_FAIL;
    obj->tx_cb=cb;
    return STATE_OK;
}

/*======================================================
函数名:xmodem_rx_cb_reg
参数:
    obj:XMODEM传输对象
    cb:XMODEM数据接收回调函数
返回值:
    STATE_OK表示成功
    STATE_FAIL表示失败
说明:
    XMODEM通信数据接收回调函数注册，注册成功后XMODEM通过调用此
    函数从对方接收数据，需要注意的是，只有在XMODEM对象空闲，即
    XMODEM_STATE_IDLE状态时，才能注册接收回调函数
=======================================================*/
type_err xmodem_rx_cb_reg(xmodem_t *obj, xmodem_rx_cb_t cb)
{
    if((!obj)||(XMODEM_STATE_IDLE!=obj->state)||(!cb))
        return STATE_FAIL;
    obj->rx_cb=cb;
    return STATE_OK;
}

/*======================================================
函数名:xmodem_cb_reg
参数:
    obj:XMODEM传输对象
    cb:XMODEM数据回调函数
返回值:
    STATE_OK表示成功
    STATE_FAIL表示失败
说明:
    XMODEM通信数据回调函数注册，注册成功后XMODEM通过调用此
    函数通知上层应用接收到数据，需要注意的是，只有在XMODEM
    对象空闲，即XMODEM_STATE_IDLE状态时，才能注册回调函数
=======================================================*/
type_err xmodem_cb_reg(xmodem_t *obj, xmodem_cb_t cb)
{
    if((!obj)||(XMODEM_STATE_IDLE!=obj->state)||(!cb))
        return STATE_FAIL;
    obj->callback=cb;
    return STATE_OK;
}

/*======================================================
函数名:xmodem_req
参数:
    obj:XMODEM传输对象
    req:XMODEM请求
        XMODEM_REQ_START表示请求一个新的传输过程
        XMODEM_REQ_STOP表示请求结束当前传输过程
返回值:
    STATE_OK表示成功
    STATE_FAIL表示失败
说明:
    XMODEM传输请求，系统在执行过程中会判断当前请求，从而执行相应
    的处理过程，如果在多线程中使用，需要注意请求冲突问题
=======================================================*/
type_err xmodem_req(xmodem_t *obj, xmodem_req_t req)
{
    if((!obj)||(XMODEM_REQ_MAX<=req))
        return STATE_FAIL;
    obj->req=req;
    return STATE_OK;
}

/*======================================================
函数名:reset
参数:
    obj:XMODEM传输对象
返回值:无
说明:
    内部函数，复位XMODEM对象状态，外部不可调用
=======================================================*/
static void reset(xmodem_t *obj)
{
    //清除当前请求
    obj->req=XMODEM_REQ_NONE;
    //将XMODEM恢复到空闲状态
    obj->state=XMODEM_STATE_IDLE;
    //将XMODEM接收状态恢复到空闲状态
    obj->rx_state=XMODEM_RX_ST_IDLE;
    //清除传输等待计数
    obj->wait_cnt=1;
    //清除接收超时次数计数
    obj->rx_retry_cnt=1;
    //清除接收缓冲区
    for(type_uint16 i=0;i<XMODEM_BUF_LEN_MAX;i++)
        obj->buf[i]=0;
    //清除当前接收数据长度
    obj->length=0;
    //清除帧计数
    obj->frm_cnt=1;
}

/*======================================================
函数名:chk_add8
参数:
    buf:缓冲区
    len:缓冲区长度
    chk_val:校验值
返回值:
    STATE_FALSE失败
    STATE_TRUE成功
说明:
    内部函数，单字节累加和校验计算，若校验匹配成功，则返回成功，
    否则返回失败
=======================================================*/
static type_bool chk_add8(type_uint8 *buf, type_uint16 len, type_uint8 chk_val)
{
    type_uint8 result=0;
    for(type_uint16 i=0;i<len;i++)
        result+=buf[i];
    if(chk_val==result)
        return STATE_TRUE;
#if xmodem_debug
    printf("chk_add8:chk_val=%d,cal_val=%d\n\r",chk_val,result);
#endif
    return STATE_FALSE;
}

/*======================================================
函数名:chk_crc16
参数:
    buf:缓冲区
    len:缓冲区长度
    chk_val:校验值
返回值:
    STATE_FALSE失败
    STATE_TRUE成功
说明:
    内部函数，CRC16校验计算，若校验匹配成功，则返回成功，否则返回
    失败
=======================================================*/
static type_bool chk_crc16(type_uint8 *buf, type_uint16 len, type_uint16 chk_val)
{
    return (chk_val==crc_161521_get(buf,len));
}

/*======================================================
函数名:rx_proc
参数:
    obj:XMODEM对象
返回值:
    接收处理结果
        XMODEM_RX_RES_ERR表示数据错误
        XMODEM_RX_RES_OK表示数据成功
        XMODEM_RX_RES_FAULT表示接收故障
        XMODEM_RX_RES_FINISH表示接收完成
说明:
    内部函数，数据接收处理过程，返回数据处理结果，详情见返回值
=======================================================*/
static xmodem_rx_res_t rx_proc(xmodem_t *obj)
{
    //非法对象则返回故障
    if((!obj)||(!obj->buf)||(!obj->callback))
    {
#if xmodem_debug
        printf("rx_proc:arg err\n\r");
#endif
        return XMODEM_RX_RES_FAULT;
    }
    //检查帧长是否合法
    if(0==obj->length)
    {
#if xmodem_debug
        printf("rx_proc:receive data length=0\n\r");
#endif
        return XMODEM_RX_RES_ERR;
    }
    //根据校验模式调整帧长度
    type_uint16 frm_len=(XMODEM_CHK_MD_ADD==obj->chk_md)?\
                         XMODEM_FRM_FLAG_LEN_ADD:XMODEM_FRM_FLAG_LEN_CRC;
    type_uint16 data_len=0;
    type_uint16 buf_len_add=0;
    type_uint16 buf_len_crc=0;
    //检查帧头标志
    switch(obj->buf[0])
    {
        //发送结束标志
        case XMODEM_FRM_FLAG_EOT:
#if xmodem_debug
            printf("rx_proc:get EOT=0\n\r");
#endif
            //返回接收完成
            return XMODEM_RX_RES_FINISH;
        break;
        //若为128字节数据
        case XMODEM_FRM_FLAG_SOH:
#if xmodem_debug
            printf("rx_proc:128 bytes data\n\r");
#endif
            data_len=XMODEM_DATA_LEN_128;
            buf_len_add=XMODEM_BUF_LEN_128_ADD;
            buf_len_crc=XMODEM_BUF_LEN_128_CRC;
            goto data_cb;
        break;
        //若为1024字节数据
        case XMODEM_FRM_FLAG_STX:
#if xmodem_debug
            printf("rx_proc:1024 bytes data\n\r");
#endif
            data_len=XMODEM_DATA_LEN_1K;
            buf_len_add=XMODEM_BUF_LEN_1K_ADD;
            buf_len_crc=XMODEM_BUF_LEN_1K_CRC;
            goto data_cb;
        break;
        default:
        break;
    }
    //非法帧头则返回错误
#if xmodem_debug
    printf("rx_proc:frame head err\n\r");
#endif
    return XMODEM_RX_RES_ERR;
data_cb:
    //设置正确帧长
    frm_len+=data_len;
    //检查帧长是否合法
    if(frm_len!=obj->length)
    {
#if xmodem_debug
        printf("rx_proc:frame length err,frm_len=%d,length=%d\n\t",frm_len,obj->length);
#endif
        return XMODEM_RX_RES_ERR;
    }
    //校验当接收包号
    if(0!=(obj->buf[1]&obj->buf[2]))
    {
#if xmodem_debug
        printf("rx_proc:frame num err,buf[1]=%d,buf[2]=%d\n\r",obj->buf[1],obj->buf[2]);
#endif
        return XMODEM_RX_RES_ERR;
    }
    //帧号合法性检测
    //首帧接收错误 或 接收帧号小于当前帧号-1 或 接收帧号大于当前帧号
    //则认为故障
    if(((1==obj->frm_cnt)&&(1!=obj->buf[1]))||\
       ((obj->frm_cnt-1)>obj->buf[1])||\
       (obj->frm_cnt<obj->buf[1]))
    {
#if xmodem_debug
        printf("rx_proc:frame num fault，cur_cnt=%d,buf frm=%d\n\r",obj->frm_cnt,obj->buf[1]);
#endif
        return XMODEM_RX_RES_FAULT;
    }
    //检查校验
    if(XMODEM_CHK_MD_ADD==obj->chk_md)
    {
        //校验失败，则返回错误
        if(!chk_add8(&obj->buf[3],data_len,obj->buf[buf_len_add-1]))
        {
#if xmodem_debug
            printf("rx_proc:add8 check err\n\r");
#endif
            return XMODEM_RX_RES_ERR;
        }
        //若接收的帧是新帧则产生回调
        if(obj->frm_cnt==obj->buf[1])
        {
#if xmodem_debug
            printf("rx_proc:data callback,data_len=%d\n\r",data_len);
#endif
            (*obj->callback)(&obj->buf[3],data_len);
            //包计数增加
            obj->frm_cnt++;
        }
#if xmodem_debug
        printf("rx_proc:data ok\n\r");
#endif
        return XMODEM_RX_RES_OK;
    }
    else if(XMODEM_CHK_MD_CRC==obj->chk_md)
    {
        //校验失败，则返回错误
        if(!chk_crc16(&obj->buf[3],data_len,(obj->buf[buf_len_crc-2]<<8)|(obj->buf[buf_len_crc-1])))
        {
#if xmodem_debug
            printf("rx_proc:crc check err\n\r");
#endif
            return XMODEM_RX_RES_ERR;
        }
        //若接收的帧是新帧则产生回调
        if(obj->frm_cnt==obj->buf[1])
        {
#if xmodem_debug
            printf("rx_proc:data callback\n\r");
#endif
            (*obj->callback)(&obj->buf[3],data_len);
            //包计数增加
            obj->frm_cnt++;
        }
#if xmodem_debug
        printf("rx_proc:data ok\n\r");
#endif
        return XMODEM_RX_RES_OK;
    }
    //非法模式则认为故障
#if xmodem_debug
    printf("rx_proc:mode fault\n\r");
#endif
    return XMODEM_RX_RES_FAULT;
}

type_uint16 rec_len_get(xmodem_t *obj)
{
    if(XMODEM_CHK_MD_ADD==obj->chk_md)
    {
        if(XMODEM_DATA_128==obj->data_md)
            return XMODEM_BUF_LEN_128_ADD;
        return XMODEM_BUF_LEN_1K_ADD;
    }
    if(XMODEM_DATA_128==obj->data_md)
        return XMODEM_BUF_LEN_128_CRC;
    return XMODEM_BUF_LEN_1K_CRC;
}

/*======================================================
函数名:xmodem_proc
参数:
    obj:XMODEM对象
返回值:无
说明:
    XMODEM接收系统主处理过程，在裸机中循环调用，或在多任务OS中创建
    线程循环调用，通过调用xmodem_req函数进行状态控制

    XMODEM接收系统主处理过程支持两种方式接收，阻塞模式和非阻塞模式，
    这两种方式通过xmodem_rx_cb_t类型接收过程实现。
    非阻塞模式:
        当使用非阻塞方式时，可以按照以下方式构造函数
        xmodem_rx_st_t xmodem_rx(type_uint8 *buf, \
                                 type_uint16 buf_len, \
                                 type_uint16 *len, \
                                 type_uint16 timeout)
        {
            非阻塞方式接收数据(buf,buf_len,len)
            if(收到数据)
                return XMODEM_RX_ST_OK;
            if(接收超时)
                return XMODEM_RX_ST_TIMEOUT;
            else
                return XMODEM_RX_ST_WAIT;
        }
    阻塞模式:
        当使用阻塞方式时，可以按照以下方式构造函数
        xmodem_rx_st_t xmodem_rx(type_uint8 *buf, \
                                 type_uint16 buf_len, \
                                 type_uint16 *len, \
                                 type_uint16 timeout)
        {
            阻塞方式接收数据(buf,buf_len,len,timeout)
            if(收到数据)
                return XMODEM_RX_ST_OK;
            else
                return XMODEM_RX_ST_TIMEOUT;
        }
=======================================================*/
void xmodem_proc(xmodem_t *obj)
{
    if(!obj)
        return;
    type_uint8 response;
    //首先判断等待状态，若处于等待状态则直接退出，用于非阻塞接收
    if(XMODEM_RX_ST_WAIT==obj->rx_state)
    {
#if xmodem_debug
        printf("xmodem_proc:XMODEM_RX_ST_WAIT\n\r");
#endif
        //再次更新获取读取接收状态,此处不进行超时设置
        obj->rx_state=(*obj->rx_cb)(obj->buf,rec_len_get(obj),&obj->length,0);
        //若仍需要等待，则直接返回
        if(XMODEM_RX_ST_WAIT==obj->rx_state)
            return;
    }
    //若接收空闲、接收完成或者已经超时，则根据XMODEM状态进一步处理
    switch(obj->state)
    {
        //IDLE状态则检查是否需要启动传输
        case XMODEM_STATE_IDLE:
            //若当前请求启动则切换到就绪模式
            if(XMODEM_REQ_START==obj->req)
            {
#if xmodem_debug
                printf("xmodem_proc:get start req.....change to XMODEM_STATE_STANDBY mode\n\r");
#endif
                //清除当前接收状态
                obj->rx_state=XMODEM_RX_ST_IDLE;
                obj->state=XMODEM_STATE_STANDBY;
                obj->req=XMODEM_REQ_NONE;
            }
        break;
        //STANDBY状态下开始准备接收数据
        case XMODEM_STATE_STANDBY:
#if xmodem_debug
            printf("xmodem_proc:XMODEM_STATE_STANDBY\n\r");
#endif
            //若当前请求停止
            if(XMODEM_REQ_STOP==obj->req)
            {
#if xmodem_debug
                printf("\tget stop req.....change to XMODEM_STATE_CANCEL mode\n\r");
#endif
                //清除当前接收状态
                obj->rx_state=XMODEM_RX_ST_IDLE;
                //切换XMODEM状态机为取消状态
                obj->state=XMODEM_STATE_CANCEL;
                obj->req=XMODEM_REQ_NONE;
            }
            //若接收处于空闲状态，则发送第一个ECHO
            else if(XMODEM_RX_ST_IDLE==obj->rx_state)
            {
#if xmodem_debug
                printf("\tsending echo & receving data\n\r");
#endif
                //根据模式设置ECHO类型
                type_uint8 echo=(XMODEM_CHK_MD_ADD==obj->chk_md?XMODEM_FRM_FLAG_ADD_ECHO:XMODEM_FRM_FLAG_CRC_ECHO);
                (*obj->tx_cb)(&echo,1);
                //设置超时，等待接收数据
                obj->rx_state=(*obj->rx_cb)(obj->buf,rec_len_get(obj),&obj->length,XMODEM_RX_TIMEOUT_MS);
            }
            //若当前接收到数据
            else if(XMODEM_RX_ST_OK==obj->rx_state)
            {
#if xmodem_debug
                printf("\treceiving data ok\n\r");
#endif
                //根据接收数据处理进行进一步操作
                switch(rx_proc(obj))
                {
                    //接收错误则按超时处理
                    case XMODEM_RX_RES_ERR:
#if xmodem_debug
                        printf("\tdata err\n\r");
#endif
                        obj->rx_state=XMODEM_RX_ST_TIMEOUT;
                    break;
                    //如果接收到第一帧数据，那么转入到接收模式
                    case XMODEM_RX_RES_OK:
#if xmodem_debug
                        printf("\tdata ok\n\r");
#endif
                        //发送一个ACK
                        response=XMODEM_FRM_FLAG_ACK;
                        (*obj->tx_cb)(&response,1);
                        //清除接收超时次数计数
                        obj->rx_retry_cnt=0;
                        //清除当前接收状态
                        obj->rx_state=XMODEM_RX_ST_IDLE;
                        //切换XMODEM状态机为接收状态
                        obj->state=XMODEM_STATE_RECEIVING;
                    break;
                    //传输完成
                    case XMODEM_RX_RES_FINISH:
#if xmodem_debug
                        printf("\tdata finish\n\r");
#endif
                        //发送一个ACK
                        response=XMODEM_FRM_FLAG_ACK;
                        (*obj->tx_cb)(&response,1);
                        //清除当前接收状态
                        obj->rx_state=XMODEM_RX_ST_IDLE;
                        //切换XMODEM状态机为停止状态
                        obj->state=XMODEM_STATE_STOP;
                    break;
                    //当前传输被从机中断
                    case XMODEM_RX_RES_FAULT:
                    //其他状态
                    default:
                        //清除当前接收状态
#if xmodem_debug
                        printf("\tdata fault\n\r");
#endif
                        obj->rx_state=XMODEM_RX_ST_IDLE;
                        //切换XMODEM状态机为取消状态
                        obj->state=XMODEM_STATE_CANCEL;
                    break;
                }
            }
            //若当前接收数据已经超时
            else if(XMODEM_RX_ST_TIMEOUT==obj->rx_state)
            {
#if xmodem_debug
                printf("\tdata timeout\n\r");
#endif
                //如果等待传输计数已经达到最大等待次数则直接停止
                if(obj->wait_cnt>=obj->wait_times)
                {
#if xmodem_debug
                    printf("\tend\n\r");
#endif
                    //清除当前接收状态
                    obj->rx_state=XMODEM_RX_ST_IDLE;
                    //切换XMODEM状态机为停止状态
                    obj->state=XMODEM_STATE_STOP;
                    break;
                }
                //等待计数增加
                obj->wait_cnt++;
                //重新发送一个ECHO
                type_uint8 echo=(XMODEM_CHK_MD_ADD==obj->chk_md?XMODEM_FRM_FLAG_ADD_ECHO:XMODEM_FRM_FLAG_CRC_ECHO);
                (*obj->tx_cb)(&echo,1);
                //设置超时，等待接收数据
                obj->rx_state=(*obj->rx_cb)(obj->buf,rec_len_get(obj),&obj->length,XMODEM_RX_TIMEOUT_MS);
#if xmodem_debug
                printf("\tsending echo & receving data\n\r");
#endif
            }
        break;
        //若当前处于接收数据状态
        case XMODEM_STATE_RECEIVING:
#if xmodem_debug
            printf("xmodem_proc:XMODEM_STATE_RECEIVING\n\r");
#endif
            //若当前请求停止
            if(XMODEM_REQ_STOP==obj->req)
            {
#if xmodem_debug
                printf("\tget stop req.....change to XMODEM_STATE_CANCEL mode\n\r");
#endif
                //清除当前接收状态
                obj->rx_state=XMODEM_RX_ST_IDLE;
                        //切换XMODEM状态机为取消状态
                obj->state=XMODEM_STATE_CANCEL;
                obj->req=XMODEM_REQ_NONE;
            }
            //如果当前接收空闲则开始接收数据
            else if(XMODEM_RX_ST_IDLE==obj->rx_state)
            {
#if xmodem_debug
                printf("\tsending echo & receving data\n\r");
#endif
                //设置超时，等待接收数据
                obj->rx_state=(*obj->rx_cb)(obj->buf,rec_len_get(obj),&obj->length,XMODEM_RX_TIMEOUT_MS);
            }
            //若当前接收到数据
            else if(XMODEM_RX_ST_OK==obj->rx_state)
            {
#if xmodem_debug
                printf("\treceiving data ok\n\r");
#endif
                //根据接收数据处理进行进一步操作
                switch(rx_proc(obj))
                {
                    //若接收错误发送NAK并按照超时处理
                    case XMODEM_RX_RES_ERR:
#if xmodem_debug
                        printf("\tdata err\n\r");
#endif
                        //发送一个NAK
                        response=XMODEM_FRM_FLAG_NAK;
                        (*obj->tx_cb)(&response,1);
                        obj->rx_state=XMODEM_RX_ST_TIMEOUT;
                    break;
                    //接收到数据则发送ACK并启动下一次接收
                    case XMODEM_RX_RES_OK:
#if xmodem_debug
                        printf("\tdata ok\n\r");
#endif
                        response=XMODEM_FRM_FLAG_ACK;
                        (*obj->tx_cb)(&response,1);
                        //清除接收超时次数计数
                        obj->rx_retry_cnt=0;
                        //设置超时，等待接收数据
                        obj->rx_state=(*obj->rx_cb)(obj->buf,rec_len_get(obj),&obj->length,XMODEM_RX_TIMEOUT_MS);
                    break;
                    //传输完成则发送ACK并切换到FINISH模式
                    case XMODEM_RX_RES_FINISH:
#if xmodem_debug
                        printf("\tdata finish\n\r");
#endif
                        response=XMODEM_FRM_FLAG_ACK;
                        (*obj->tx_cb)(&response,1);
                        //清除当前接收状态
                        obj->rx_state=XMODEM_RX_ST_IDLE;
                        //切换XMODEM状态机为完成状态
                        obj->state=XMODEM_STATE_FINISH;
                    break;
                    //当前传输被从机中断则发送CAN并切换到取消模式
                    case XMODEM_RX_RES_FAULT:
                    //其他状态
                    default:
#if xmodem_debug
                        printf("\tdata fault\n\r");
#endif
                        //清除当前接收状态
                        obj->rx_state=XMODEM_RX_ST_IDLE;
                        //切换XMODEM状态机为取消状态
                        obj->state=XMODEM_STATE_CANCEL;
                    break;
                }
            }
            //若当前接收数据已经超时
            else if(XMODEM_RX_ST_TIMEOUT==obj->rx_state)
            {
#if xmodem_debug
                printf("\tdata timeout\n\r");
#endif
                //如果接收超时次数计数已经达到最大接收超时次数
                if(obj->rx_retry_cnt>=obj->rx_retry)
                {
#if xmodem_debug
                    printf("\tend\n\r");
#endif
                    //清除当前接收状态
                    obj->rx_state=XMODEM_RX_ST_IDLE;
                    //切换XMODEM状态机为取消状态
                    obj->state=XMODEM_STATE_CANCEL;
                    break;
                }
                //超时次数计数增加
                obj->rx_retry_cnt++;
                //设置超时，等待接收数据
                obj->rx_state=(*obj->rx_cb)(obj->buf,rec_len_get(obj),&obj->length,XMODEM_RX_TIMEOUT_MS);
#if xmodem_debug
                printf("\treceving data\n\r");
#endif
            }
        break;
        //XMODEM传输完成处理
        case XMODEM_STATE_FINISH:
#if xmodem_debug
            printf("xmodem_proc:XMODEM_STATE_FINISH\n\r");
#endif
            //产生回调，告知上层当前传输已完成
            if(obj->callback)
                (*obj->callback)(obj->buf,0);
            //复位XMODEM
            reset(obj);
        break;
        //XMODEM传输取消处理
        case XMODEM_STATE_CANCEL:
#if xmodem_debug
            printf("xmodem_proc:XMODEM_STATE_CANCEL\n\r");
#endif
            response=XMODEM_FRM_FLAG_CAN;
            (*obj->tx_cb)(&response,1);
            obj->state=XMODEM_STATE_STOP;
        break;
        //若为停止状态
        case XMODEM_STATE_STOP:
#if xmodem_debug
            printf("xmodem_proc:XMODEM_STATE_STOP\n\r");
#endif
            //产生回调，告知上层当前传输已停止
            if(obj->callback)
                (*obj->callback)(TYPE_NULL,0);
            //复位XMODEM
            reset(obj);
        break;
        default:
        break;
    }
}

/*======================================================
函数名:xmodem_state_get
参数:
    obj:XMODEM对象
返回值:
    当前XMODEM状态
        XMODEM_STATE_IDLE表示空闲
        XMODEM_STATE_STANDBY表示当前可以传输
        XMODEM_STATE_RECEIVING表示当前正在传输
        XMODEM_STATE_FINISH表示当前已经传输完成
        XMODEM_STATE_CANCEL表示当前传输被取消
        XMODEM_STATE_STOP表示当前传输已经停止
说明:
    获取当前XMODEM的状态
=======================================================*/
xmodem_state_t xmodem_state_get(xmodem_t *obj)
{
    if(!obj)
        return XMODEM_STATE_UNKNOW;
    return obj->state;
}

/*======================================================
函数名:xmodem_deinit
参数:
    obj:XMODEM对象
返回值:无
说明:
    卸载XMODEM对象，在卸载XMODEM对象前，需要先调用此函数确
    保当前XMODEM处于空闲状态，随后再进行卸载，否则会出现异常状态
=======================================================*/
void xmodem_deinit(xmodem_t *obj)
{
    if(!obj)
        return;
    reset(obj);
}

#ifdef __cplusplus
}
#endif
