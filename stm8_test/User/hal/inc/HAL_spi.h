#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_

void SPI_Configuration(void);
u8  SPI_SendByte(SPI_TypeDef *SPIx, u8 byte);

#endif