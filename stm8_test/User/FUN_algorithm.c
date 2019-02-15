


#include "main.h"


//  公司字符
#define COMPANY1_STRING		0X59	// 'Y'
#define COMPANY2_STRING		0X47	// 'G'
// 项目字符
#define PROJECT1_STRING		0X44	// 'D'
#define PROJECT2_STRING		0X57	// 'W'
#define CAL_STRING			0X53	// 'S'
//SN字符数组编号
#define SERIALNUM1 0
#define SERIALNUM2 1
#define SERIALNUM3 2
#define EQUIPMENT1 3
#define EQUIPMENT2 4

typedef union  
{  
    float fdata;  
    unsigned long ldata;  
}FloatLongType; 


/*===============================================
				比较常数数据	
data1 :比较的数据
leng1:比较数据的长度
data2 :比较的常数
leng2:比较常数的长度

返回0，则比较失败
返回1，则比较成功
=================================================*/
unsigned char ALG_CmpsConst( unsigned char *data1,unsigned int leng1,const unsigned char *data2,unsigned int leng2)
{
	while(leng1)
	    {
	    if(*data1!=*data2) return 0;
	    data1++;data2++;leng1--;leng2--;
	    if(!leng2) return 1;
	    }
	return 0;  
}	

/*===============================================
				比较常数数据	
data1 :比较的数据
leng1:比较数据的长度
data2 :比较的数据
leng2:比较数据的长度

返回0，则比较失败
返回1，则比较成功
=================================================*/
unsigned char ALG_CmpsData( unsigned char *data1,unsigned int leng1,unsigned char *data2,unsigned int leng2)
{
	while(leng1)
	    {
	    if(*data1!=*data2) return 0;
	    data1++;data2++;leng1--;leng2--;
	    if(!leng2) return 1;
	    }
	return 0;  
}	


//*****************************************************************拷贝RAM数据//
void ALG_CopyRam(unsigned char *data1,unsigned int leng1,unsigned char *data2,unsigned int *leng2)
    {
	*leng2=leng1;
    while(leng1)
	    { 
	    *data2=*data1;data2++;data1++;leng1--;
	    }
	}

/************************************************************************/
/*                  ASCII转换为十六进制                                 */
/************************************************************************/
unsigned char ALG_AsciitoHex(unsigned char asc)
{
	unsigned char tmp=0xff;
	
	if ((asc>=0x30)&&(asc<=0x39))	// 0-9
	{
		tmp=asc-0x30;
	}
	else if ((asc>=0x41)&&(asc<=0x46))		// A-F
	{
		tmp=asc-0x37;
	}
	else if ((asc>=0x61)&&(asc<=0x66))		// a-f
	{
		tmp=asc-0x57;
	}
	return tmp;
}

/************************************************************************/
/*              十六进制转换为ASCII码                                */
/************************************************************************/

unsigned char ALG_HextoAscii(unsigned char tmp)
{
  unsigned char asc;
  asc= tmp;
  return asc;
}


char ALG_HexToASCII(unsigned char  data_hex)
{ 
   char  ASCII_Data;
   ASCII_Data=data_hex & 0x0F;
   if(ASCII_Data<10) 
    ASCII_Data=ASCII_Data;//+0x30; //‘0--9’
   else  
    ASCII_Data=ASCII_Data;//;+0x37;       //‘A--F’
   return ASCII_Data;
}

void ALG_HexGroupToString(char *OutStrBuffer, unsigned char *InHexBuffer, unsigned int HexLength)
{
   unsigned int i, k=0;
   for(i=0;i<HexLength;i++)
   {
      OutStrBuffer[k++]=ALG_HexToASCII((InHexBuffer[i]>>4)&0x0F);
      OutStrBuffer[k++]=ALG_HexToASCII(InHexBuffer[i]&0x0F);
   }
   OutStrBuffer[k]='\0';
}



/************************************************************************/
/*                           信息ASCII转换为可用数据                     */
/************************************************************************/
void ALG_ASCIItoDATA(unsigned char *pdata1,unsigned char *pdata2,unsigned char *leng2)
{
	unsigned char cnt=0;
	unsigned char tmp1,tmp2;
	
	tmp1=*pdata1++;
	tmp2=*pdata1++;
	*leng2=(ALG_AsciitoHex(tmp1)<<4)+ALG_AsciitoHex(tmp2);
	for (;cnt<(*leng2);cnt++)
	{
		tmp1=*pdata1++;
		tmp2=*pdata1++;
		*pdata2=(ALG_AsciitoHex(tmp1)<<4)+ALG_AsciitoHex(tmp2);
		pdata2++;
	}
}
/* 
将4个字节数据byte[4]转化为浮点数存放在*f中 
*/  
void Byte_to_Float(float *f,unsigned char byte[])  
{  
    FloatLongType fl;  
    fl.ldata=0;  
    fl.ldata=byte[3];  
    fl.ldata=(fl.ldata<<8)|byte[2];  
    fl.ldata=(fl.ldata<<8)|byte[1];  
    fl.ldata=(fl.ldata<<8)|byte[0];  
    *f=fl.fdata;  
}  


/************************************************************************/
/*                           主要是针对手机号码转为PDU格式                  */
/************************************************************************/
void ALG_ConvPDU(unsigned char *pdata1,unsigned char *pdata2)
{
	unsigned char cnt=0;
	unsigned char tmp1,tmp2;
	
	for (;cnt<6;cnt++)
	{
		if(cnt<5)
		{
			tmp1	=	*pdata1++;
			tmp2	= 	*pdata1++;
		}
		else
		{
			tmp1 	=	*pdata1;
			tmp2	= 	0x46; // 'F'	
		}
		*pdata2	=	tmp2;
		pdata2++;
		*pdata2 =	tmp1;
		pdata2++;
	}
	
}


/*
	计算CRC

注:返回值为CRC
*/
unsigned char ALG_CrcCal(unsigned int length, unsigned char cmd)
{
	u8 len1,len2;

	len2 = (u8)(length>>8);
	len1 = ((u8)length)^cmd;
	len1 ^=len2;
	if( len1 %2 )
	{
		len1^=COMPANY2_STRING;  // ‘G’
	}
	else
	{
		len1^=COMPANY1_STRING;  // ‘Y’
	}
	return (len1+1);

}






unsigned char ALG_CmdCrcCal(unsigned char *pdata, unsigned int len)
{
	unsigned int i;
	unsigned char xorByte = pdata[0];
    for(i=1;i<len;i++)
	{
		xorByte ^= pdata[i];
	}
	xorByte++;
    return xorByte; 
}



/************************************************************************/
/*              十进制显示                                              */
/************************************************************************/

unsigned char ALG_HextoAlg(unsigned char tmp1)
{
  unsigned char tmp;

  unsigned char cnt,cnt1;
  tmp = tmp1;
  cnt = tmp / 10;
  cnt1 = tmp % 10;
  
  tmp = (cnt<<4) + cnt1;
  
  return tmp;
}

unsigned char ALG_CmdCrcCal1(unsigned char *pdata, unsigned int len)
{
	unsigned int i;
	unsigned char xorByte = pdata[0];
    for(i=1;i<len;i++)
	{
		xorByte ^= pdata[i];
	}
    return xorByte; 
}

/*
 * 函    数: ALG_SN_Code
 * 功    能: 获取SN设备号的加密字
 * 输入参数: pdata: SN原始数据
 * 输出参数: 无
 * 返 回 值: 加密字
 * 其    他:
 */
u8 ALG_SN_Code(u8 *pdata){
	u8 Code = 0;
	Code = pdata[SERIALNUM1]^pdata[SERIALNUM2];	
	Code ^= pdata[SERIALNUM3];
	return ++Code;
}

/*
 * 函    数: ALG_SN_Encrypt
 * 功    能: 对SN设备号进行加密
 * 输入参数: pdata: SN原始数据
 *			 Code: 加密字
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他:
 */
void ALG_SN_Encrypt(u8 *pdata, u8 Code){
	pdata[EQUIPMENT1] ^= Code;
	pdata[EQUIPMENT2] ^= Code;
}





