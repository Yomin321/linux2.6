#ifndef _APP_FLASH_H_
#define _APP_FLASH_H_

#define EQUIPMENTCHAR1	0x55	//设备项目字符1"U"
#define EQUIPMENTCHAR2	0x4C	//设备项目字符2"L"

#define SNFLAGADD	0x13E7	//SN数据标志位的地址
#define SNFLAGDATA	0x79	//SN的标志位数据

#define SNHAVEADD	0X13E5



/*---------------------终端产品序列号V1.0.0----------------------*/
/*
N地址范围：0x17F0~0x17F5
SN格式： 业务类型(1byte)+编号(4byte)+校验码(1byte)
业务类型： 
		超声波 		C1
		车位视频	C8
		车位锁		C5
		引导屏		D1
		收费屏		C3
		地磁		C4
		道闸		C7
编号：4byte 从 0x00 0x00 0x00 0x01 开始,结束于 0x99 0x99 0x99 0x99
	  编号遇9进位，与十进制相仿，例如 0x09 进位 0x10
	  注：编号不为0
校验码： 校验码 = 业务类型^编号 + 0x47
*/
#define SN_START_ADD	0x17F0	//SN数据的开始地址
#define SN_LENG		6			//SN的数据长度
#define SN_TYPE		0xAF		//业务类型
#define SN_CRC_ADD		0x17F5	//校验码地址




typedef struct 
{
	u8 SNdata[SN_LENG];
	u8 SNflag;
}SN_STRUCT;

extern SN_STRUCT APP_SN_StrPar;		   

u8 APP_SN_CRC(u8 *pdata,u8 sn_type);
void APP_SN_Write(void);
void APP_SN_Init(void);



#endif