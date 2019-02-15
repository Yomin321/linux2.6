
#include "main.h"

/*
	eeprom的变量初始化
*/
void HAL_eeprom_ValueInit(void)
{
}


void HAL_eeprom_Init(void)
{
  FLASH_DeInit();
}

void HAL_eeprom_ReadData(u32 Address,u8 *pdata,u16 DataLength)
{
	u8 data;
	u16 cnt;
	u32 Addr;
	//地址不在范围内，直接跳出
	if(Address > EEP_END_ADDR || Address < EEP_BASE_ADDR)return;

	for(cnt=0;cnt<DataLength;cnt++)
	{
		Addr = Address + cnt;
		data = FLASH_ReadByte(Addr);
		*pdata++ = data;
	}	
}

void HAL_eeprom_WriteData(u32 Address,u8 *pdata,u16 DataLength)
{
	u8 data;
	u16 cnt;
	u32 Addr;
	//地址不在范围内，直接跳出
	if(Address > EEP_END_ADDR || Address < EEP_BASE_ADDR)return;
	
	FLASH_Unlock(FLASH_MemType_Data); 

	while(FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET);// 等待写保护解除

	for(cnt=0;cnt<DataLength;cnt++)
	{
		Addr = Address + cnt;
		data = *pdata++;
		FLASH_ProgramByte(Addr,data);
		while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET);
	}

	FLASH_Lock(FLASH_MemType_Data);
	while(FLASH_GetFlagStatus(FLASH_FLAG_DUL) == SET);
	
}


void HAL_eeprom_EraseAllBlock(void)
{
	u8 i, dat[16];
    u32 addr;
    u32 tmp = EEP_END_ADDR - EEP_BASE_ADDR + 1 ;
    
    for(i=0; i<16; i++)  dat[i] = 0x00;

    for (addr=0; addr<tmp; addr += 16)
    {
        HAL_eeprom_WriteData(EEP_BASE_ADDR+addr,dat,16);
    }
}




