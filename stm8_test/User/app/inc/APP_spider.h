#ifndef _APP_SPIDER_H_
#define _APP_SPIDER_H_


#define SPIDER_USART     USER_USART3


//-------------------------------------------------------业务类别
#define DEV_TYPE         0x00
//-------------------------------------------------------功耗服务类别
#define DEV_Power_DC     0x00    
#define DEV_Power_BTY    0x0F
//-------------------------------------------------------业务类型
#define DEV_TypeClass       0xAF   // 单向引导牌

//-------------------------------------------------------STU非敏感位
#define DEV_NonSensitiveByte1 0x00  // 0/1 : 敏感/非敏感
#define DEV_NonSensitiveByte2 0x00  // 0/1 : 敏感/非敏感



#define SPIDER_DLY_TIME 	        5  //spider启动延时
#define MAIN_SPIDER_INIT_TIME	10// SPIDER模块总初始化时间35S



//---------------------SPIDER终端模块外围硬件复位引脚
#define  APP_SPIDER_HARDWARE_DISABLE()    SPIDER_TM_RESET_DISABLE()
#define  APP_SPIDER_HARDWARE_ENABLE()	SPIDER_TM_RESET_ENABLE()

extern   u8 SPIDER_WORK_DATA[42]; //37to42
extern u8 U_Packet_CntCache; //网关下发的U包计数缓存(暂时以全局变量形式处理)

//-------------------------------变量
typedef struct{
  u8  InitOKFlag;				// spider 初始化成功标志  TRUE : 成功    FALSE :失败
  u8  InitOverTimeCnt;		// spider初始化超时计数
  u8  StartDly;				// 系统开机延时，3s.
  bool IEPINOK_Flag;		//中断事件标志
  bool MF_Flag;  //读取模块形态标志位
  bool WP_Flag;  //读取工作参数标志位
  bool SingleRegFlag;   //单终端组网标志
  bool MultRegFlag;   //多终端组网标志
  u8 InitDealAddr;
  bool UartRevOkFlag;        // ok
  bool UartRevOverFlag;        // 超时标志
  u16  UartRevOverCnt;        // 超时标志
  u8 SendToSpiderCmdCnt;		//发送给Spider命令计数
  u8 DealSendCmdToSpiderAddr;//主动向Spider发送命令地址
  u8 WakeupDealAddr;		//  唤醒处理地址
  u16 WakeupTimeCnt;		// 唤醒定时计数

  //----------uart
	u16	TxUsartLeng;
	u8  TxUsartData[256];

	// 快捷令标志
	u8 FastCmdFlag;
	u8 FastCmdLEDShakeFlag;

	u8 Judge_Is_OnNetFlag;		//是否组网标志  TRUE :已经组网    FALSE :没有组网 
	u8 Judge_ReadyOnNetFlag;	//是否继续进入组网模式    false:继续进入   true : 不再进入
	u8 Judge_SendOnNetCmdFlag;	//判断是否发送组网命令标志，true : 发送  false : 停止发送
}SPIDER_STRUCT;


extern SPIDER_STRUCT  APP_SPIDER;


void APP_spider_UsartDealForInit(void);
void APP_spider_ValueInit(void);
void APP_spider_IEPIN_deal(void);
void APP_spider_init(void);
void APP_spider_uartDeal(void);
void APP_spider_FastCmdLEDshake(void);
void APP_Spider_dealSendCmdToSpider(void);
void APP_Spider_ReadyOnNet(void);


#endif