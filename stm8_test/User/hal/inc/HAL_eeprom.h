
#ifndef _HAL_EEPROM_H_
#define _HAL_EEPROM_H_






//-------------------------------------------------------µÿ÷∑∑∂Œß
// stm8L151c8 ------------ 2K Byte
// #define EEP_BASE_ADDR       0x1000 
// #define EEP_END_ADDR        0x13FF

#define EEP_BASE_ADDR       0x1000 
#define EEP_END_ADDR        0x17FF






void HAL_eeprom_Init(void);
void HAL_eeprom_ReadData(u32 Address,u8 *pdata,u16 DataLength);
void HAL_eeprom_WriteData(u32 Address,u8 *pdata,u16 DataLength);
void HAL_eeprom_EraseAllBlock(void);


#endif





















