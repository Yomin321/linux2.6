/***********************************************************************************************
**************************************PRO_spider.h************************************
***********************************************************************************************/
#ifndef _PRO_SPIDER__H_
#define _PRO_SPIDER__H_


// spider命令开头和结束符
#define SPIDER_CMD_START 0X59
#define SPIDER_CMD_END   0X47


//------------------------- 配置类
#define  CMD_TYPE_CONFIG    0X01 
	#define CONFIG_CMD_SETWORKPARAMETER			0X01  	// 设置工作参数
	#define CONFIG_CMD_SETMODULECONFIG  0X02	// 设置模块形态
//------------------------ 控制类	
#define  CMD_TYPE_CTRL		0X03 
	#define CTRL_CMD_RESTART_MOD			0X01	//复位模块
	#define CTRL_CMD_RESTART_RF			0X02	//复位模块无线部分
	#define CTRL_CMD_RESETDEFAULT  	0X03	//恢复出厂
	#define CTRL_CMD_LOGOUTNETFILE	0X04	//注销组网档案
	#define CTRL_CMD_CLOSESLEEP       0X10	//关闭休眠功能
	#define CTRL_CMD_SWITCHWORKMODE   0X11	//切换工作模式

//------------------------ 查询类	
#define  CMD_TYPE_QUERY		0X04 
	#define QUERY_CMD_GETMODULESTATUS		0X01	//查询模块状态
	#define QUERY_CMD_GETWORKPARAMETER		0X02	//查询工作参数
	#define QUERY_CMD_GETNETFILE				0X06	//查询组网档案
	#define QUERY_CMD_GETMODULESHAPE			0X0A	//查询模块形态

	#define QUERY_CMD_GETDOWNSTAIRFALST  0X31	//查询下行快捷令
	#define QUERY_CMD_GETDOWNUPACK		0X32	// 查询下行U包
	#define QUERY_CMD_GETDOWNMPACK		0X33	// 查询下行M包
	
#define  CMD_TYPE_TRFER		0X05 // 传输类
	#define TRFER_CMD_SENDSTATUS     		0X01	//更新上行状态
	#define TRFER_CMD_SENDUPACK			0X02	//发送上行 U包
	#define TRFER_CMD_SENDMPACK			0X03	//发送上行 M 包
	#define TRFER_CMD_STARTPPACK			0X04	//启动下行 P 包数据流
	
	
	#define TRFER_CMD_SENDPPACK			0X05	//发送上行 P 包
	#define TRFER_CMD_STOPPPACK			0X06	//关闭上行 P 包数据流


void PRO_spider_BuildCMDForNoPar(u8 cmd,u8 cmd1);
void PRO_spider_BuildCMDForPar(u8 cmd,u8 cmd1,u8* pdata,u16 dataLength);
void PRO_spider_BuildMParkForPar(u8 *SendDate,u8 serve,u8* pdata,u16 dataLength);


#endif

