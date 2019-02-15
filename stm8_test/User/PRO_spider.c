#include "main.h"



/*
	生成命令(针对无参数)
	cmd:命令类别
	cmd1:具体命令
*/
void PRO_spider_BuildCMDForNoPar(u8 cmd,u8 cmd1)
{
	u16 cnt;
	APP_SPIDER.TxUsartLeng =0 ;
        
	APP_SPIDER.TxUsartData[APP_SPIDER.TxUsartLeng++]= SPIDER_CMD_START;// 头
	APP_SPIDER.TxUsartData[APP_SPIDER.TxUsartLeng++]= 0x02;// 长度低字节
	APP_SPIDER.TxUsartData[APP_SPIDER.TxUsartLeng++]= 0x00;
	APP_SPIDER.TxUsartData[APP_SPIDER.TxUsartLeng++]= cmd;// 命令类别
	APP_SPIDER.TxUsartData[APP_SPIDER.TxUsartLeng++]= cmd1;// 具体命令
	cnt = APP_SPIDER.TxUsartLeng;
	APP_SPIDER.TxUsartData[APP_SPIDER.TxUsartLeng++]= ALG_CmdCrcCal(&APP_SPIDER.TxUsartData[1],cnt-1);
	APP_SPIDER.TxUsartData[APP_SPIDER.TxUsartLeng++]= SPIDER_CMD_END;
}



/*
	生成命令(针对有参数)
	cmd:命令类别
	cmd1:具体命令
	pdata:参数指针
	dataLength:参数的长度
*/
void PRO_spider_BuildCMDForPar(u8 cmd,u8 cmd1,u8* pdata,u16 dataLength)
{
	u16 cnt;
	
	APP_SPIDER.TxUsartLeng =0 ;
	APP_SPIDER.TxUsartData[APP_SPIDER.TxUsartLeng++]= SPIDER_CMD_START;// 头(开始符)
	APP_SPIDER.TxUsartLeng+=2; // 跳过长度2个字节
        
	APP_SPIDER.TxUsartData[APP_SPIDER.TxUsartLeng++]= cmd;// 命令类别
	APP_SPIDER.TxUsartData[APP_SPIDER.TxUsartLeng++]= cmd1;// 具体命令
        
	for(cnt=0;cnt<dataLength;cnt++)
	{
		APP_SPIDER.TxUsartData[APP_SPIDER.TxUsartLeng++]=*(pdata+cnt);
	}
	//  重新赋值长度字节
	APP_SPIDER.TxUsartData[1]= (APP_SPIDER.TxUsartLeng-3)&0xFF;
	APP_SPIDER.TxUsartData[2]= ((APP_SPIDER.TxUsartLeng-3)>>8)&0xFF;
        
	cnt = APP_SPIDER.TxUsartLeng;
	APP_SPIDER.TxUsartData[APP_SPIDER.TxUsartLeng++]= ALG_CmdCrcCal(&APP_SPIDER.TxUsartData[1],cnt-1);
	APP_SPIDER.TxUsartData[APP_SPIDER.TxUsartLeng++]= SPIDER_CMD_END;
}
/*
 * 函 数 名: main
 * 功    能: 主函数
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
 
void PRO_spider_BuildMParkForPar(u8 *SendDate,u8 serve,u8* pdata,u16 dataLength){
	int i = 0;
	if(dataLength > 128)	dataLength = 128;
	SendDate[0] = serve;
	for(i = 0;i < dataLength; i++){
		SendDate[i+1] = pdata[i];
	}
	if(dataLength%16 != 0)	dataLength = dataLength/16+16;
	PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER, TRFER_CMD_SENDMPACK, SendDate, dataLength+1);
}


