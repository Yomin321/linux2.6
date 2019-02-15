#include "main.h"

SN_STRUCT APP_SN_StrPar;	
/*
void APP_SN_Write(void){
	// u8 SNData[SNLENG] = {0};
	u8 SNFlag = SNFLAGDATA;
	u8 Code = 0;
	// u8 s_tmp[7] = {0};
	HAL_eeprom_WriteData(SNFLAGADD, &SNFlag, 1);					//写入SN加密修改标志位
	//HAL_eeprom_ReadData(SNSTARTADD, SNData, SNLENG);				//读取SN原始数据
	Code = ALG_SN_Code(APP_SN_StrPar.SNdata);						//获取SN加密字
	ALG_SN_Encrypt(APP_SN_StrPar.SNdata, Code);						//对SN设备号加密
	HAL_eeprom_WriteData(SNSTARTADD, APP_SN_StrPar.SNdata, SNLENG);	//写入SN加密后的数据
#ifdef DEBUG_MCU_STATUS
	printf("\r\n【MCU status】:encrypted SN: %02X_%02X_%02X_%02X_%02X\r\n",APP_SN_StrPar.SNdata[0],
		APP_SN_StrPar.SNdata[1],APP_SN_StrPar.SNdata[2],APP_SN_StrPar.SNdata[3],APP_SN_StrPar.SNdata[4]);
#endif
	// s_tmp[0] = 5;
	// s_tmp[1] = 0;
	// s_tmp[2] = SNData[0];
	// s_tmp[3] = SNData[1];
	// s_tmp[4] = SNData[2];
	// s_tmp[5] = SNData[3];
	// s_tmp[6] = SNData[4];
	//PRO_spider_BuildCMDForPar(CMD_TYPE_CONFIG,CONFIG_CMD_SETWORKPARAMETER,s_tmp,7);
	//HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);         
} 
*/
/*---------------------终端产品序列号V1.0.0----------------------*/
void APP_SN_Init(void){
	u32 num_sum=0;
	u8 SN_TYPE_DATA=SN_TYPE;		
	u8 SN_Crc_Data=0;

#if 1
	//SN号写入测试
	APP_SN_StrPar.SNdata[1]=0x09;			//写入测试SN号
	HAL_eeprom_WriteData(SN_START_ADD+4, &APP_SN_StrPar.SNdata[1], 1);		//写入SN
	HAL_eeprom_WriteData(SN_START_ADD, &SN_TYPE_DATA, 1);					//写入业务类型
	SN_Crc_Data=APP_SN_CRC(APP_SN_StrPar.SNdata,SN_TYPE);					//计算校验码
	HAL_eeprom_WriteData(SN_CRC_ADD, &SN_Crc_Data, 1);						//写入校验码
	//SN号写入测试*/
#endif

 
	HAL_eeprom_ReadData(SN_START_ADD, APP_SN_StrPar.SNdata, SN_LENG);	//读取SN原始数据
#ifdef DEBUG_MCU_STATUS
	printf("\r\n【MCU status】:Read From Eeprom SN: %02X_%02X_%02X_%02X_%02X_%02X\r\n",APP_SN_StrPar.SNdata[0],
		APP_SN_StrPar.SNdata[1],APP_SN_StrPar.SNdata[2],APP_SN_StrPar.SNdata[3],APP_SN_StrPar.SNdata[4],APP_SN_StrPar.SNdata[5]);
#endif
	//数据有效性判断
	for(int i=1; i<5; i++){
		num_sum+=APP_SN_StrPar.SNdata[i];
		if((APP_SN_StrPar.SNdata[i]&0xF0) > 0x90 || (APP_SN_StrPar.SNdata[i]&0x0F) > 0x09){	//判断数据是否非法
			while(1){
				printf("_%02x_%02x_\n",APP_SN_StrPar.SNdata[i]<<4,APP_SN_StrPar.SNdata[i]>>4);
				printf("\r\nIllegal SN !\r\n");	
				for(;;) feed_IWDG();				//喂狗	
			}
		}

	}					
	while(!num_sum){	//编号为0
		printf("\r\nIllegal SN !\r\n");	
		for(;;) feed_IWDG();				//喂狗	
	}	//判断业务类型
	if(APP_SN_StrPar.SNdata[0]==0x00 || APP_SN_StrPar.SNdata[0]==0xFF){	//初次烧录
		HAL_eeprom_WriteData(SN_START_ADD, &SN_TYPE_DATA, 1);					//写入业务类型
		SN_Crc_Data=APP_SN_CRC(APP_SN_StrPar.SNdata,SN_TYPE);					//计算校验码
		HAL_eeprom_WriteData(SN_CRC_ADD, &SN_Crc_Data, 1);					//写入校验码
	}else if(APP_SN_StrPar.SNdata[0]==SN_TYPE)	//判断业务类型
		{
			SN_Crc_Data=APP_SN_CRC(APP_SN_StrPar.SNdata,APP_SN_StrPar.SNdata[0]);	//计算校验码
			if(SN_Crc_Data != APP_SN_StrPar.SNdata[5]){			//读取的校验码和计算得到的不相同
				while(1){
					printf("\r\nSN CRC Fault !\r\n");	
					for(;;) feed_IWDG();				//喂狗	
				}
			}else{	//直接将其写入工作参数
					for (int i = 0; i < 4; ++i)	//写入4位编号
					{
						SPIDER_WORK_DATA[i]=APP_SN_StrPar.SNdata[i+1];
					}
					SPIDER_WORK_DATA[8]=APP_SN_StrPar.SNdata[0];	//写入业务类型
				}

		}else	//业务类型不对
			{
				while(1){
					printf("\r\nSN TYPE Fault !\r\n");	
					for(;;) feed_IWDG();				//喂狗	
				}
			}
}

/*
 * 函    数: APP_SN_CRC
 * 功    能: 计算SN编号校验值
 * 输入参数: pdata: SN号起始地址
 *			 sn_type: 业务类型
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他:
 */
u8 APP_SN_CRC(u8 *pdata,u8 sn_type){
	u8 crc_data=0;
	for (int i = 1; i < 5; ++i)
		{
			crc_data+=sn_type ^ pdata[i];
		}	
	crc_data+=0x47;
	return crc_data;
}

/*
void APP_SN_Init(void){
	HAL_eeprom_ReadData(SNSTARTADD, APP_SN_StrPar.SNdata, SNLENG);	//读取SN原始数据
	HAL_eeprom_ReadData(SNFLAGADD, &APP_SN_StrPar.SNflag, 1);		//读取SN加密标志位数据
	if(APP_SN_StrPar.SNflag == SNFLAGDATA){//SN号已加密
		if(APP_SN_StrPar.SNdata[0]||APP_SN_StrPar.SNdata[1]||APP_SN_StrPar.SNdata[2]||APP_SN_StrPar.SNdata[3]||APP_SN_StrPar.SNdata[4]){
			u8 Code = 0;
			Code = ALG_SN_Code(APP_SN_StrPar.SNdata);	//获取SN加密字
			if((APP_SN_StrPar.SNdata[3] == EQUIPMENTCHAR2^Code) && (APP_SN_StrPar.SNdata[4] == EQUIPMENTCHAR1^Code) ){	//判断加密是否正确
#ifdef DEBUG_MCU_STATUS
				printf("\r\n【MCU status】:SN: %02X_%02X_%02X_%02X_%02X\r\n",APP_SN_StrPar.SNdata[0],
				APP_SN_StrPar.SNdata[1],APP_SN_StrPar.SNdata[2],APP_SN_StrPar.SNdata[3],APP_SN_StrPar.SNdata[4]);
#endif
				for (int i = 0; i < 5; ++i)	//写入工作参数区
				{
					SPIDER_WORK_DATA[i]=APP_SN_StrPar.SNdata[i];
				}
			}else{	//加密不正确
				while(1)
				{
					printf("\r\n【MCU Status】SN falut !\r\n" );
					feed_IWDG();				//喂狗	
				}
			}
		}
		else{
			//while(1);//添加LED闪烁指示，和喂狗过程
			while(1)
			{
				printf("\r\n【MCU Status】SN falut !\r\n" );
				feed_IWDG();				//喂狗	
			}
		}
	}
	else{
		if(APP_SN_StrPar.SNdata[4] == EQUIPMENTCHAR1 && APP_SN_StrPar.SNdata[3] == EQUIPMENTCHAR2){//SN号未加密
#ifdef DEBUG_MCU_STATUS
	printf("\r\n【MCU status】:The is SN unencrypted\r\n");
#endif
#ifdef DEBUG_MCU_STATUS
	printf("\r\n【MCU status】:unencrypted SN: %02X_%02X_%02X_%02X_%02X\r\n",APP_SN_StrPar.SNdata[0],
		APP_SN_StrPar.SNdata[1],APP_SN_StrPar.SNdata[2],APP_SN_StrPar.SNdata[3],APP_SN_StrPar.SNdata[4]);
#endif
			APP_SN_Write();	//对SN数据进行加密
		}
		else if(APP_SN_StrPar.SNdata[4] == 0x00 && APP_SN_StrPar.SNdata[3] == 0x00){//无SN号
#ifdef DEBUG_MCU_STATUS
	printf("\r\n【MCU status】:The is no SN\r\n");
#endif
			//while(1);//添加LED闪烁指示，和喂狗过程
		}
		else{//SN有误
#ifdef DEBUG_MCU_STATUS
	printf("\r\n【MCU status】:SN error\r\n");
#endif
			//while(1);//添加LED闪烁指示，和喂狗过程
		}
	}

// 	u8 SNFlag = 0;
// 	u8 SNData[SNLENG] = {0};
// 	HAL_eeprom_ReadData(SNHAVEADD, &SNFlag, 1);
// 	if(SNFlag != 0xF1){
// #ifdef DEBUG
// 		printf("\r\nnot have SN, init failure\r\n");
// #endif
// 		return 0;
// 	}
// 	HAL_eeprom_ReadData(SNFLAGADD, &SNFlag, 1);	//读取SN修改标志位
// 	if(SNFlag == 0){		//判断SN数据是否加密
// #ifdef DEBUG
// 	printf("\r\nstart init SN...\r\n");
// #endif
// 	APP_SN_Write();	//对SN数据进行加密
// #ifdef DEBUG
// 	printf("\r\nSN init succeed\r\n");
// #endif
// 	}
// 	HAL_eeprom_ReadData(SNSTARTADD, SNData, SNLENG);	//读取SN数据
// #ifdef DEBUG
// 	printf("\r\nSN: %X-%X-%X-%X-%X\r\n",SNData[0],SNData[1],SNData[2],SNData[3],SNData[4]);
// #endif
	// return 1;
}

*/
