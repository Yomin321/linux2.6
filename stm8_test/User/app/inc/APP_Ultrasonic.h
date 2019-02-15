#ifndef _APP_ULTRASONIC_H_
#define _APP_ULTRASONIC_H_


#include <string.h>


#define ULTRASONIC_MAX_BUFFER_LENGTH 50
#define ULTRASONIC_RESEND_MAX_TIME   30000//30s.

//------------------------------V1.0.0版本协议宏定义---------------------------

#define REPORT_PKG_CLASS_STA 0x60  //主动上报数据包类型——主动上报——状态变化
#define REPORT_PKG_CLASS_REL 0x20  //主动上报数据包类型——主动上报——建联数据

// #define SEND_PKG_CLASS_QUE  0x40   //发送数据包类型——主动上报——查询命令
// #define SEND_PKG_CLASS_CTR  0x30   //发送数据包类型——主动上报——控制命令

#define ACK_PKG_CLASS 1<<3     //应答数据包类型 

typedef enum
{
  ULTRASONIC_IDLE,
  ULTRASONIC_ACTIVE
}Ultrasonic_Perform_Status_enum;


//--------------------------变量
typedef struct{
  u8  InitOKFlag;		// 超声波 初始化成功标志
  bool StatuChaflag;
  u8  InitOverTimeCnt;		// 超声波 初始化超时计数
  
  u16 SendCmdOverTimeCnt;	//向超声波发送命令计时
  
  u8 InitDealAddr;		//初始化处理
  u16 UartRevOverCnt;           // 超时计数
  u8 RevBuffCashe;	        //缓存上次接收数据
  
  u8 LocalAddr;			//超声波本地地址
  u8 LedShakeFlag;		//指示灯闪烁标志 0:不闪烁  1: 闪烁
  u16 LedShakeTimCnt;		//指示灯闪烁计时
  u8 GW_Down_Cmd_Flag;	//网关下发查询命令标志
  u8  TxCnt;     //发送命令次数
}ULTRAS_STRUCT;


typedef struct
{
  uint8_t buf[ULTRASONIC_MAX_BUFFER_LENGTH];
  uint8_t len;
}Ultrasonic_Temporary_Buffer;


typedef struct
{
  uint8_t perform_status;
  volatile uint32_t timer;
}Ultrasonic_Control_Struct;


extern ULTRAS_STRUCT  APP_ULTRASONIC;


void APP_Ultrasonic_ValueInit(void);
void APP_Ultrasonic_ConfigInit(void);
void APP_Ultrasonic_UsartDealForInit(void);
void APP_Ultrasonic_UartRxDealforInit(void);
void APP_Ultrasonic_FixedPollState(u8 CmdFlag);
void APP_Ultrasonic_UartRxDealforWork(void);


void start_send_U_package(void);
void start_send_S_package(void);

void halt_send_U_package(void);
void ultrasonic_delay(void);



#endif

