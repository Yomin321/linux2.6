

#ifndef __FUN_ALGORITHM_H_
#define __FUN_ALGORITHM_H_



unsigned char ALG_CmpsConst( unsigned char *data1,unsigned int leng1,const unsigned char *data2,unsigned int leng2);
unsigned char ALG_CmpsData( unsigned char *data1,unsigned int leng1,unsigned char *data2,unsigned int leng2);

void ALG_CopyRam(unsigned char *data1,unsigned int leng1,unsigned char *data2,unsigned int *leng2);
unsigned char ALG_AsciitoHex(unsigned char asc);
unsigned char ALG_HextoAscii(unsigned char tmp);

char ALG_HexToASCII(unsigned char  data_hex);
void ALG_HexGroupToString(char *OutStrBuffer, unsigned char *InHexBuffer, unsigned int HexLength);

void ALG_ASCIItoDATA(unsigned char *pdata1,unsigned char *pdata2,unsigned char *leng2);
void ALG_ConvPDU(unsigned char *pdata1,unsigned char *pdata2);
unsigned char ALG_CrcCal(unsigned int length, unsigned char cmd);

unsigned char ALG_CmdCrcCal(unsigned char *pdata, unsigned int len);

unsigned char ALG_HextoAlg(unsigned char tmp1);
unsigned char ALG_CmdCrcCal1(unsigned char *pdata, unsigned int len);

u8 ALG_SN_Code(u8 *pdata);
void ALG_SN_Encrypt(u8 *pdata, u8 Code);
void Byte_to_Float(float *f,unsigned char byte[]);


#endif




