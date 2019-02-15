#ifndef __RTC_API_H__
#define __RTC_API_H__

#include "typedef.h"

//RTC IE
#define IRTC_X2IE(x)		    JL_IRTC->CON = ((JL_IRTC->CON & ~(BIT(1))) | (x & 0x01)<<1)
#define IRTC_X512IE(x)	        JL_IRTC->CON = ((JL_IRTC->CON & ~(BIT(2))) | (x & 0x01)<<2)
#define IRTC_WKIE(x)	        JL_IRTC->CON = ((JL_IRTC->CON & ~(BIT(3))) | (x & 0x01)<<3)

#define IRTC_WKCLRPND           JL_IRTC->CON |= BIT(15)
#define IRTC_X512CLRPND         JL_IRTC->CON |= BIT(14)
#define IRTC_X2CLRPND           JL_IRTC->CON |= BIT(13)

#define IRTC_CS(x)              SFR(JL_IRTC->CON, 8,  1, x)

//RTC OP ADDR
#define WRITE_RTC               0x40
#define READ_RTC                0xC0
#define WRITE_ALM               0x10
#define READ_ALM                0x90

#define W_RTC_REG            	0x20 //WRITE REG
#define R_RTC_REG           	0xA0 //READ  REG

//PR PORT
#define PORTR0          	    0
#define PORTR1             		1
#define PORTR2              	2
#define PORTR3              	3

//WAKEUP SOURCE
#define WAKE_UP_PR0        		BIT(0)
#define WAKE_UP_PR1         	BIT(1)
#define WAKE_UP_PR2         	BIT(2)
#define WAKE_UP_PR3         	BIT(3)
#define EDGE_PR0            	BIT(4)
#define EDGE_PR1            	BIT(5)
#define EDGE_PR2            	BIT(6)
#define EDGE_PR3            	BIT(7)

#define WKUP_TYOE_NORMAL        0
#define WKUP_TYPE_PCNT_OVF      1
#define WKUP_TYPE_EDGE          2
#define WKUP_TYPE_ALMOUT        3
#define WKUP_TYPE_LVD5V         4

enum {
    LONG_4S_RESET = 0,
    LONG_8S_RESET,
};

enum {
    RTC_ISR_ALARM_ON = 0,
    RTC_ISR_PR_WAKE,
    RTC_ISR_X512,
    RTC_ISR_X2,
    RTC_ISR_PCNT,
    RTC_ISR_LDO5V,
};

//RTC REG ADDR

/* PDFLAG             1bit RW  */ //BIT 7
/* ALMOUT             1bit R   */ //BIT 6
/* ALMEN              1bit RW  */ //BIT 5
/* PINR_8SEN      	  1bit R   */ //BIT 4
/* RESERVED           1bit RW  */ //BIT 3
/* RESERVED           1bit RW  */ //BIT 2
/* XCKS               2bit RW  */ //BIT 0-1
#define RTC_CON_ADDR00		0x00

/* PR_HD              4bit RW  */ //BIT 4-7
/* PR_IN              4bit R   */ //BIT 0-3
#define RTC_CON_ADDR01		0x01

/* PR_DIR             4bit RW  */ //BIT 4-7
/* PR_OUT             4bit RW  */ //BIT 0-3
#define RTC_CON_ADDR02		0x02

/* PR_PD              4bit RW  */ //BIT 4-7
/* PR_PU              4bit RW  */ //BIT 0-3
#define RTC_CON_ADDR03		0x03

/* WKUP_EDGE          4bit RW  */ //BIT 4-7  0:上升沿     1:下降沿
/* WAKEUP_EN          4bit RW  */ //BIT 0-3  0:不允许唤醒 1:允许唤醒
#define RTC_CON_ADDR04		0x04

/* WAKEUP_PND_CLR     4bit RW  */ //BIT 4-7
/* WAKEUP_PND         4bit R   */ //BIT 0-3
#define RTC_CON_ADDR05		0x05

/* RESERVED      	  1bit RW  */ //BIT 7
/* RESERVED      	  1bit RW  */ //BIT 6
/* X32XS			  2bit RW  */ //BIT 4-5
/* RESERVED      	  1bit RW  */ //BIT 3
/* X32TS			  1bit RW  */ //BIT 2
/* X32OE    		  1bit RW  */ //BIT 1
/* X32EN              1bit RW  */ //BIT 0
#define RTC_CON_ADDR06		0x06

/* LDO5V_PND          1bit RW  */ //BIT 7  LDO5V pending
/* LDO5V_PND_CLR      1bit RW  */ //BIT 6  clear LDO5V pending
/* PWR_EN[1]		  2bit RW  */ //BIT 5  0:PR1做普通IO口 1:输出内部Wake Up信号
/* PWR_EN[0]          1bit RW  */ //BIT 4  0:PR0做普通IO口 1:输出内部Wake Up信号
/* RESERVED      	  1bit RW  */ //BIT 3
/* LDO5V_DET		  1bit RW  */ //BIT 2  0:LDO5V没有插入 1:LDO5V插入
/* LDO5V_EDGE    	  1bit RW  */ //BIT 1  0:上升沿   1:下降沿
/* LDO5V_EN           1bit RW  */ //BIT 0
#define RTC_CON_ADDR07		0x07

/* PINR_EDGE          1bit RW  */ //BIT 7  0:低电平复位   1:高电平复位
/* RESET EN           1bit RW  */ //BIT 6
/* PR_SEL             2bit RW  */ //BIT 4-5
/* PR_DIE             4bit RW  */ //BIT 0-3
#define RTC_CON_ADDR08		0x08

/* RESERVED      	  5bit RW  */ //BIT 3-7
/* DIFF_XS            2bit RW  */ //BIT 1-2
/* DIFF_EN            1bit RW  */ //BIT 0
#define RTC_CON_ADDR09		0x09

/* X24XS      	      4bit RW  */ //BIT 4-7
/* RESERVED        	  1bit RW  */ //BIT 3
/* X24TS      	      1bit RW  */ //BIT 2
/* X24OE              1bit RW  */ //BIT 1
/* X24EN              1bit RW  */ //BIT 0
#define RTC_CON_ADDR0A		0x0A

/* PCNT_PND      	  1bit RW  */ //BIT 7
/* PCNT_PND_CLR       1bit RW  */ //BIT 6
/* RESERVED        	  1bit RW  */ //BIT 5
/* PCNT_WAKEUP_EN     1bit RW  */ //BIT 4
/* PCNT_PIN_SEL       2bit RW  */ //BIT 2-3
/* PCNT_EDGE          1bit RW  */ //BIT 1
/* PCNT_EN            1bit RW  */ //BIT 0
#define RTC_CON_ADDR11		0x11

/* PCNT_VALUE         7bit RW  */ //BIT 0-7
#define RTC_CON_ADDR12		0x12

/* RESERVED        	  4bit RW  */ //BIT 4-7
/* LVD5V WAKEUP       1bit RW  */ //BIT 3
/* ALMOUT      	      1bit RW  */ //BIT 2
/* EDGE_WAKEUP        1bit RW  */ //BIT 1
/* PCNT OVF           1bit RW  */ //BIT 0
#define RTC_CON_ADDR13		0x13

/* RESERVED        	  5bit RW  */ //BIT 3-7
/* ADC MUX            3bit RW  */ //BIT 0-2
#define RTC_CON_ADDR14		0x14

struct rtc_data {
    u16  days;
    u8   hour;
    u8   minute;
    u8   second;
};

void RTC_CLK_INIT(u8 div);

//RTC SFR OPERATIONS
void RTC_SFR_SET(u8 addr, u8 start, u8 len, u8 data);
u8 RTC_SFR_GET(u8 addr);

//PR PORT BACKUP
void RTC_PORT_BACKUP(void);
void RTC_PORT_RECOVER(void);
void RTC_PORT_CLOSE(void);

//PR PORT OPERATIONS
void PORTR_DIR(u8 port, u8 val);
void PORTR_OUT(u8 port, u8 val);
void PORTR_HD(u8 port, u8 val);
void PORTR_PU(u8 port, u8 val);
void PORTR_PD(u8 port, u8 val);
void PORTR_DIE(u8 port, u8 val);
u8 PORTR_IN(u8 port);

//PR PORT ADC CONTROL
void PORTR1_ADCEN_CTL(u8 val);
void PORTR2_ADCEN_CTL(u8 val);

void adc_mux_ch_set(u8 ch);
//RTC RESET SETTING mode 0:4S 1:8S
int rtc_port_reset(u8 mode, u8 port, u8  enable, u8 edge);
int rtc_port_4s_reset_close(void);

//RTC LDO5V
int rtc_ldo5v_detect(u8  enable, u8 edge);
int get_ldo5v_detect_flag(void);
void clr_ldo5v_pending(void);

//RTC 32K
void X32EN(u8 en);
void X32OE(u8 en);
void X32XS_LEV(u8 lev);
void close_32K(u8 keep_osci_flag);

//RTC 24M
void X24EN(u8 en);
void X24OE(u8 en);
void X24XS_LEV(u8 lev);


//RTC WAKEUP
u8 get_wake_up_type(void);    //wakeup source
u8 check_io_wakeup_pend();    //wakeup io

void soft_poweroff_wakeup_io(u8 wakeup_io, u8 wakeup_edge);

//测试demo
void rtc_test_demo(void);

//pcnt
void pcnt_init(u8 port, u8 edge);
void pcnt_kitstart(u8 cnt);
void pcnt_stop(void);

#endif // __RTC_API_H__
