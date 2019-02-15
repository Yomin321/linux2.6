#ifndef _HAL_SPI_FLASH_H_
#define _HAL_SPI_FLASH_H_


/* Select FLASH: Chip Select pin low */
#define SPI_FLASH_CS_LOW()  GPIO_ResetBits(GPIOD, GPIO_Pin_7)
/* Deselect FLASH: Chip Select pin high */
#define SPI_FLASH_CS_HIGH() GPIO_SetBits(GPIOD, GPIO_Pin_7)  

#define FLASH_WIP_FLAG           0x01  /*!< Write In Progress (WIP) flag */


#define FLASH_DUMMY_BYTE         0xA5



#define FLASH_W25X16             0xEF3015
#define FLASH_W25X40             0xEF3013
#define FLASH_W25Q16             0xEF4015

#define FLASH_W25X40_SETCOR		128	//	4Mbit=512Kbyte=128*4Kbyte
#define FLASH_W25X40_BLOCK		8	//	4Mbit=512Kbyte=8*64Kbyte
#define FLASH_W25X40_SIZE_BYTE	0x080000 // 


typedef struct 
{
    u8   flash_cmd_rdid;   /*!< Read flash id instruction */
    u32  flash_id; 
    char *flash_desc;
    u8   flash_cmd_write;  /*!< Page program instruction */
    u8   flash_cmd_wren;   /*!< Write enable instruction */  
    u8   flash_cmd_read;   /*!< Read from Memory instruction */
    u8   flash_cmd_rdsr;   /*!< Read Status Register instruction  */
    u8   flash_cmd_se;     /*!< Sector Erase instruction */
    u8   flash_cmd_be;     /*!< Block Erase instruction */
    u8   flash_cmd_ce;     /*!< Chip Erase instruction */
    u16  flash_spi_pagesize;
}SPI_FLASH_CMD;

//---------------------------------------------variable
extern u32 volatile spi_flash_index;
extern u8 volatile spi_flash_found;        
extern SPI_FLASH_CMD spi_flash_list[];

void  SPI_FLASH_Init(void); 
void SPI_FLASH_set_index(u32 flash_index, u8 flash_found);
u8 SPI_FLASH_get_index(u32 *flash_index);
void SPI_FLASH_WaitForWriteEnd(void);
void SPI_Flash_WriteEnable(void);
void SPI_FLASH_EraseSector(u32 FlashAddr);
void SPI_FLASH_EraseBlock(u32 FlashAddr);
void SPI_FLASH_WritePage(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_WriteBuffer(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_ReadBuffer(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);





#endif