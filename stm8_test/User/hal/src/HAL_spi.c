
#include "main.h"



/*===============================================
	SPI配置函数
=================================================*/
void SPI_Configuration(void)
{

  /*!< Configure FLASH_CS_PIN pin: FLASH CS pin */
  GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_Out_PP_High_Fast);  //初始化PC端口，设为推挽输出
  
  /*!< Configure FLASH_SPI pins: SCK MOSI  MISO*/
  GPIO_Init(GPIOD, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);//上拉输入
    
  /*!< SPI configuration */
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI2, ENABLE);//使能SPI2时钟
  
  SPI_Init(SPI2,SPI_FirstBit_MSB,SPI_BaudRatePrescaler_2,SPI_Mode_Master,\
    SPI_CPOL_High,SPI_CPHA_2Edge,SPI_Direction_2Lines_FullDuplex,SPI_NSS_Soft,0x07);
  
  SPI_Cmd(SPI2,ENABLE);
}


/*===============================================
	SPI发送一个字节
=================================================*/
u8  SPI_SendByte(SPI_TypeDef *SPIx, u8 byte)
{

    /*!< Loop while DR register in not empty */
    while (SPI_GetFlagStatus(SPIx, SPI_FLAG_TXE) == RESET);

    /*!< Send byte through the SPI1 peripheral */
    SPI_SendData(SPIx, byte);

    /*!< Wait to receive a byte */
    while (SPI_GetFlagStatus(SPIx, SPI_FLAG_RXNE) == RESET);

    /*!< Return the byte read from the SPI bus */
    return SPI_ReceiveData(SPIx);
}

































