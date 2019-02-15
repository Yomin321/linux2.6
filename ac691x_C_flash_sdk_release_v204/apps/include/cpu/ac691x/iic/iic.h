#ifndef	_IIC_H_
#define _IIC_H_

#include "typedef.h"
#include "common/common.h"

void iic_write(u8 chip_id, u8 iic_addr, u8 *iic_dat, u8 n);
void iic_readn(u8 chip_id, u8 iic_addr, u8 *iic_dat, u8 n);
void iic_init(void);
void iic_init_io(void);
void write_info(u8 addr, u8 dat);
tu8 read_info(u8 addr);

void write_eerom(u8 addr, u8 dat);
tu8 read_eerom(u8 iic_addr);
void eeprom_page_write_stop(void);
void eeprom_page_write_start(void);
u8 eeprom_verify(void);

void iic_start(void);
void iic_stop(void);
void iic_sendbyte_io(u8 byte);
void iic_sendbyte(u8 byte);
tu8 iic_revbyte(u8 para);

void set_memory(u8 addr, u8 dat);
u8 get_memory(u8 addr);

extern bool g_iic_busy;

#define iic_delay()      delay(15)

#define _24C01_ADD_Read        0xa1
#define _24c01_ADD_Write       0xa0
#define BT_VOL_ADD            0xF8
#define BT_NAME_ADD            0x08

#define IIC_PORT			JL_PORTA
#define IIC_DAT				BIT(12)
#define IIC_CLK				BIT(11)


#define iic_data_out()   do{IIC_PORT->DIR &= ~IIC_DAT;IIC_PORT->PU |= IIC_DAT;}while(0)
#define iic_data_in()    do{IIC_PORT->DIR |=  IIC_DAT;IIC_PORT->PU |= IIC_DAT;}while(0)
#define iic_data_r()     (IIC_PORT->IN & IIC_DAT)
#define iic_data_h()     do{IIC_PORT->OUT |= IIC_DAT;}while(0)
#define iic_data_l()     do{IIC_PORT->OUT &=~IIC_DAT;}while(0)

#define iic_clk_out()    do{IIC_PORT->DIR &=~IIC_CLK;IIC_PORT->PU |= IIC_CLK;}while(0)
#define iic_clk_h()      do{IIC_PORT->OUT |= IIC_CLK;}while(0)
#define iic_clk_l()      do{IIC_PORT->OUT &=~IIC_CLK;}while(0)

/*
#define iic_data_out()   do{USB_DM_DIR(0);USB_DM_PU(1);}while(0)
#define iic_data_in()    do{USB_DM_DIR(1);USB_DM_PU(1);}while(0)
#define iic_data_r()     (USB_DM_IN())
#define iic_data_h()     do{USB_DM_OUT(1);}while(0)
#define iic_data_l()     do{USB_DM_OUT(0);}while(0)

#define iic_clk_out()    do{USB_DP_DIR(0);USB_DP_PU(1);}while(0)
#define iic_clk_h()      do{USB_DP_OUT(1);}while(0)
#define iic_clk_l()      do{USB_DP_OUT(0);}while(0)
*/
#define IIC_DATA_PORT       3
#define IIC_CLK_PORT        4
/*
#define iic_clk_out()    //do{JL_PORTA->DIR |=  BIT(IIC_CLK_PORT);JL_PORTA->PU |= BIT(IIC_CLK_PORT);}while(0)
#define iic_clk_h()      do{JL_PORTA->DIR |=  BIT(IIC_CLK_PORT);JL_PORTA->PU |= BIT(IIC_CLK_PORT);}while(0)
#define iic_clk_l()      do{JL_PORTA->OUT &=~BIT(IIC_CLK_PORT);JL_PORTA->DIR &=~BIT(IIC_CLK_PORT);}while(0)


#define iic_data_out()   //do{JL_PORTA->DIR |=  BIT(IIC_DATA_PORT);JL_PORTA->PU |= BIT(IIC_DATA_PORT);}while(0)
#define iic_data_in()    do{JL_PORTA->DIR |=  BIT(IIC_DATA_PORT);JL_PORTA->PU |= BIT(IIC_DATA_PORT);}while(0)
#define iic_data_r()     (JL_PORTA->IN&BIT(IIC_DATA_PORT))
#define iic_data_h()     do{JL_PORTA->DIR |=  BIT(IIC_DATA_PORT);JL_PORTA->PU |= BIT(IIC_DATA_PORT);}while(0)
#define iic_data_l()     do{JL_PORTA->OUT &=~BIT(IIC_DATA_PORT);JL_PORTA->DIR &= ~BIT(IIC_DATA_PORT);}while(0)

*/
/*
#define app_IIC_write(a, b, c, d) \
  iic_write(a, b, c, d)
#define app_IIC_readn(a, b, c, d)  \
  iic_readn(a, b, c, d)
#define app_E2PROM_write(a, b)  \
  write_eerom(a, b)
#define app_E2PROM_read(a)   \
  read_eerom(a)
*/

#endif






