
#include "main.h"


/* SPI flash list, add the flash you want to support here */
SPI_FLASH_CMD spi_flash_list[]=
{
    {
        0x9f,  /*!< Read flash id instruction */
        FLASH_W25X40,
        "Winbond W25X40",
        0x02,  /*!< Page program instruction */
        0x06,  /*!< Write enable instruction */
        0x03,  /*!< Read from Memory instruction */
        0x05,  /*!< Read Status Register instruction  */
        0x20,  /*!< Sector Erase instruction */
        0xD8,  /*!< Block Erase instruction */
        0xC7,  /*!< Chip Erase instruction */
        0x100, /*!< Chip page size */
    },
    {
        0x9f,  /*!< Read flash id instruction */
        FLASH_W25Q16,
        "Winbond W25Q16",
        0x02,  /*!< Page program instruction */
        0x06,  /*!< Write enable instruction */
        0x03,  /*!< Read from Memory instruction */
        0x05,  /*!< Read Status Register instruction  */
        0x20,  /*!< Sector Erase instruction */
        0xD8,  /*!< Block Erase instruction */
        0xC7,  /*!< Chip Erase instruction */
        0x100, /*!< Chip page size */
    }
};
u32 volatile spi_flash_index;
u8 volatile spi_flash_found;




/*===============================================
		FLASH初始化
=================================================*/
void  SPI_FLASH_Init(void) 
{
	SPI_Configuration();
	SPI_FLASH_CS_HIGH();  //宏定义
}




/**
  * @brief  Set flash index and the found status.
  * @param  flash_index: the flash index.
  * @param  flash_found: the found status.
  * @retval None.
  */
void SPI_FLASH_set_index(u32 flash_index, u8 flash_found)
{
    spi_flash_index = flash_index;
    spi_flash_found = flash_found;
}

/**
  * @brief  get flash index and the found status.
  * @param  *flash_index: the flash index.
  * @retval the flash found status.
  */
u8 SPI_FLASH_get_index(u32 *flash_index)
{
    *flash_index = 0;
    if(spi_flash_found != 0)
        *flash_index = spi_flash_index;
    return spi_flash_found;
}



/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's
  *         status register and loop until write opertaion has completed.
  * @param  None
  * @retval None
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
    u8 flashstatus = 0;
    
    /*!< Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();

    /*!< Send "Read Status Register" instruction */
    SPI_SendByte(SPI2, spi_flash_list[spi_flash_index].flash_cmd_rdsr);

    /*!< Loop as long as the memory is busy with a write cycle */
    do
    {
        /*!< Send a dummy byte to generate the clock needed by the FLASH
        and put the value of the status register in FLASH_Status variable */
        flashstatus = SPI_SendByte(SPI2, FLASH_DUMMY_BYTE);
    }while ((flashstatus & FLASH_WIP_FLAG) == SET); /* Write in progress */

    /*!< Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();
}



/**
  * @brief  Enables the write access to the FLASH.
  * @param  None
  * @retval None
  */
void SPI_Flash_WriteEnable(void)
{
    /*!< Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();
    
    /*!< Send "Write Enable" instruction */
    SPI_SendByte(SPI2, spi_flash_list[spi_flash_index].flash_cmd_wren);

    /*!< Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();
}



/**
  * @brief  Erases the specified FLASH sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  */
void SPI_FLASH_EraseSector(u32 FlashAddr)
{
    /*!< Send write enable instruction */
    SPI_Flash_WriteEnable();
    
    /*!< Sector Erase */
    
    /*!< Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();
    /*!< Send Sector Erase instruction */
    SPI_SendByte(SPI2, spi_flash_list[spi_flash_index].flash_cmd_se);
    /*!< Send SectorAddr high nibble address byte */
    SPI_SendByte(SPI2, (FlashAddr & 0xFF0000) >> 16);
    /*!< Send SectorAddr medium nibble address byte */
    SPI_SendByte(SPI2, (FlashAddr & 0xFF00) >> 8);
    /*!< Send SectorAddr low nibble address byte */
    SPI_SendByte(SPI2, FlashAddr & 0xFF);
    /*!< Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();

    /*!< Wait the end of Flash writing */
    SPI_FLASH_WaitForWriteEnd();
}


/**
  * @brief  Erases the specified FLASH block.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  */
void SPI_FLASH_EraseBlock(u32 FlashAddr)
{
    /*!< Send write enable instruction */
    SPI_Flash_WriteEnable();
    
    /*!< Sector Erase */
    
    /*!< Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();
    /*!< Send Sector Erase instruction */
    SPI_SendByte(SPI2, spi_flash_list[spi_flash_index].flash_cmd_be);
    /*!< Send SectorAddr high nibble address byte */
    SPI_SendByte(SPI2, (FlashAddr & 0xFF0000) >> 16);
    /*!< Send SectorAddr medium nibble address byte */
    SPI_SendByte(SPI2, (FlashAddr & 0xFF00) >> 8);
    /*!< Send SectorAddr low nibble address byte */
    SPI_SendByte(SPI2, FlashAddr & 0xFF);
    /*!< Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();

    /*!< Wait the end of Flash writing */
    SPI_FLASH_WaitForWriteEnd();
}








/**
  * @brief  Writes more than one byte to the FLASH with a single WRITE cycle 
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the FLASH page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH, must be equal
  *         or less than "FLASH_PAGESIZE" value.
  * @retval None
  */
void SPI_FLASH_WritePage(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    /*!< Enable the write access to the FLASH */
    SPI_Flash_WriteEnable();
    /*!< Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();
    /*!< Send "Write to Memory " instruction */
    SPI_SendByte(SPI2, spi_flash_list[spi_flash_index].flash_cmd_write);
    /*!< Send WriteAddr high nibble address byte to write to */
    SPI_SendByte(SPI2, (WriteAddr & 0xFF0000) >> 16);
    /*!< Send WriteAddr medium nibble address byte to write to */
    SPI_SendByte(SPI2, (WriteAddr & 0xFF00) >> 8);
    /*!< Send WriteAddr low nibble address byte to write to */
    SPI_SendByte(SPI2, WriteAddr & 0xFF);

    /*!< while there is data to be written on the FLASH */
    while (NumByteToWrite--)
    {
        /*!< Send the current byte */
        SPI_SendByte(SPI2, *pBuffer);
        /*!< Point on the next byte to be written */
        pBuffer++;
    }

    /*!< Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();

    /*!< Wait the end of Flash writing */
    SPI_FLASH_WaitForWriteEnd();
}

/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH.
  * @retval None
  */
void SPI_FLASH_WriteBuffer(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u8 num_of_page = 0, num_of_single = 0, offset_in_page = 0, num_remained_in_1st_page = 0, temp = 0;
    u16 spi_flash_page_size = spi_flash_list[spi_flash_index].flash_spi_pagesize;
    
    offset_in_page = WriteAddr % spi_flash_page_size;
    num_remained_in_1st_page = spi_flash_page_size - offset_in_page;
    num_of_page =  NumByteToWrite / spi_flash_page_size;
    num_of_single = NumByteToWrite % spi_flash_page_size;

    if (offset_in_page == 0) /*!< WriteAddr is FLASH_PAGESIZE aligned  */
    {
        if (num_of_page == 0) /*!< NumByteToWrite < FLASH_PAGESIZE */
        {
            SPI_FLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
        }
        else /*!< NumByteToWrite > FLASH_PAGESIZE */
        {
            while (num_of_page--)
            {
                SPI_FLASH_WritePage(pBuffer, WriteAddr, spi_flash_page_size);
                WriteAddr +=  spi_flash_page_size;
                pBuffer += spi_flash_page_size;
            }

            if (num_of_single != 0)
            {
                SPI_FLASH_WritePage(pBuffer, WriteAddr, num_of_single);
            }
        }
    }
    else /*!< WriteAddr is not FLASH_PAGESIZE aligned  */
    {
        if (num_of_page == 0) /*!< NumByteToWrite < FLASH_PAGESIZE */
        {
            if (num_of_single > num_remained_in_1st_page) /*!< (NumByteToWrite + WriteAddr) > FLASH_PAGESIZE */
            {
                temp = num_of_single - num_remained_in_1st_page;
                SPI_FLASH_WritePage(pBuffer, WriteAddr, num_remained_in_1st_page);
                WriteAddr +=  num_remained_in_1st_page;
                pBuffer += num_remained_in_1st_page;

                SPI_FLASH_WritePage(pBuffer, WriteAddr, temp);
            }
            else
            {
                SPI_FLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
            }
        }
        else /*!< NumByteToWrite > FLASH_PAGESIZE */
        {
            NumByteToWrite -= num_remained_in_1st_page;
            num_of_page =  NumByteToWrite / spi_flash_page_size;
            num_of_single = NumByteToWrite % spi_flash_page_size;

            SPI_FLASH_WritePage(pBuffer, WriteAddr, num_remained_in_1st_page);
            WriteAddr +=  num_remained_in_1st_page;
            pBuffer += num_remained_in_1st_page;

            while (num_of_page--)
            {
                SPI_FLASH_WritePage(pBuffer, WriteAddr, spi_flash_page_size);
                WriteAddr +=  spi_flash_page_size;
                pBuffer += spi_flash_page_size;
            }

            if (num_of_single != 0)
            {
                SPI_FLASH_WritePage(pBuffer, WriteAddr, num_of_single);
            }
        }
    }
}

/**
  * @brief  Reads a block of data from the FLASH.
  * @param  pBuffer: pointer to the buffer that receives the data read from the FLASH.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the FLASH.
  * @retval None
  */
void SPI_FLASH_ReadBuffer(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    /*!< Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();

    /*!< Send "Read from Memory " instruction */
    SPI_SendByte(SPI2, spi_flash_list[spi_flash_index].flash_cmd_read);

    /*!< Send ReadAddr high nibble address byte to read from */
    SPI_SendByte(SPI2, (ReadAddr & 0xFF0000) >> 16);
    /*!< Send ReadAddr medium nibble address byte to read from */
    SPI_SendByte(SPI2, (ReadAddr& 0xFF00) >> 8);
    /*!< Send ReadAddr low nibble address byte to read from */
    SPI_SendByte(SPI2, ReadAddr & 0xFF);
    
    while (NumByteToRead--) /*!< while there is data to be read */
    {
        /*!< Read a byte from the FLASH */
        *pBuffer = SPI_SendByte(SPI2, FLASH_DUMMY_BYTE);
        /*!< Point to the next location where the byte read will be saved */
        pBuffer++;
    }

    /*!< Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();
}



























