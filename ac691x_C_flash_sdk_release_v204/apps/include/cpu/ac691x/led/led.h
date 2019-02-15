#ifndef _LED_H_
#define _LED_H_

#include "includes.h"
#include "sys_detect.h"
#include "sdk_cfg.h"

void led_pwm_init(void);
void led_pwm_close(void);

// *INDENT-OFF*
#if (USE_USB_IO_LED&LED_DM_TWO_LED)  ////用usb 的DM同时推两个灯R_LED, B_LED,只能用普通IO口推
  #undef  USE_PWM_CONTROL_LED
  #define USE_PWM_CONTROL_LED        0
#endif
#if LED_EN

  #if (LEDB_PORT&LED_POARA)
    #define LEDB_PORT_DIE  JL_PORTA->DIE
    #define LEDB_PORT_PU   JL_PORTA->PU
    #define LEDB_PORT_PD   JL_PORTA->PD
    #define LEDB_PORT_DIR  JL_PORTA->DIR
    #define LEDB_PORT_OUT  JL_PORTA->OUT
  #elif (LEDB_PORT&LED_POARB)
    #define LEDB_PORT_DIE  JL_PORTB->DIE
    #define LEDB_PORT_PU   JL_PORTB->PU
    #define LEDB_PORT_PD   JL_PORTB->PD
    #define LEDB_PORT_DIR  JL_PORTB->DIR
    #define LEDB_PORT_OUT  JL_PORTB->OUT
  #elif (LEDB_PORT&LED_POARC)
    #define LEDB_PORT_DIE  JL_PORTC->DIE
    #define LEDB_PORT_PU   JL_PORTC->PU
    #define LEDB_PORT_PD   JL_PORTC->PD
    #define LEDB_PORT_DIR  JL_PORTC->DIR
    #define LEDB_PORT_OUT  JL_PORTC->OUT
  #elif (LEDB_PORT&LED_POARD)
    #define LEDB_PORT_DIE   JL_PORTD->DIE
    #define LEDB_PORT_PU   JL_PORTD->PU
    #define LEDB_PORT_PD   JL_PORTD->PD
    #define LEDB_PORT_DIR  JL_PORTD->DIR
    #define LEDB_PORT_OUT  JL_PORTD->OUT
  #endif

  #if (LEDR_PORT&LED_POARA)
    #define LEDR_PORT_DIE  JL_PORTA->DIE
    #define LEDR_PORT_PU   JL_PORTA->PU
    #define LEDR_PORT_PD   JL_PORTA->PD
    #define LEDR_PORT_DIR  JL_PORTA->DIR
    #define LEDR_PORT_OUT  JL_PORTA->OUT
  #elif (LEDR_PORT&LED_POARB)
    #define LEDR_PORT_DIE  JL_PORTB->DIE
    #define LEDR_PORT_PU   JL_PORTB->PU
    #define LEDR_PORT_PD   JL_PORTB->PD
    #define LEDR_PORT_DIR  JL_PORTB->DIR
    #define LEDR_PORT_OUT  JL_PORTB->OUT
  #elif (LEDR_PORT&LED_POARC)
    #define LEDR_PORT_DIE  JL_PORTC->DIE
    #define LEDR_PORT_PU   JL_PORTC->PU
    #define LEDR_PORT_PD   JL_PORTC->PD
    #define LEDR_PORT_DIR  JL_PORTC->DIR
    #define LEDR_PORT_OUT  JL_PORTC->OUT
  #elif (LEDR_PORT&LED_POARD)
    #define LEDR_PORT_DIE  JL_PORTD->DIE
    #define LEDR_PORT_PU   JL_PORTD->PU
    #define LEDR_PORT_PD   JL_PORTD->PD
    #define LEDR_PORT_DIR  JL_PORTD->DIR
    #define LEDR_PORT_OUT  JL_PORTD->OUT
  #endif

  #if (USE_PWM_CONTROL_LED==1)//用pwm推灯设置
      #define LED_INIT_EN()     	  do{led_pwm_init();}while(0)
      #define LED_INIT_DIS()		  do{led_pwm_close();}while(0)

      #if (USE_USB_IO_LED & LED_DM_DP)////用usb dm dp推灯
         #define R_LED_ON()           do{USB_DP_PU(1);USB_DP_PD(1);USB_DP_DIR(0);}while(0)
         #define R_LED_OFF()          do{USB_DP_PU(0);USB_DP_PD(0);USB_DP_DIR(1);}while(0)
         #define B_LED_ON()           do{USB_DM_PU(1);USB_DM_PD(1);USB_DM_DIR(0);}while(0)
         #define B_LED_OFF()          do{USB_DM_PU(0);USB_DM_PD(0);USB_DM_DIR(1);}while(0)

      #elif (USE_USB_IO_LED & LED_DM_ONE_LED)//用usb 的DM B_LED
         #define R_LED_ON(...)
         #define R_LED_OFF(...)
         #define B_LED_ON()           do{USB_DM_PU(1);USB_DM_PD(1);USB_DM_DIR(0);}while(0)
         #define B_LED_OFF()          do{USB_DM_PU(0);USB_DM_PD(0);USB_DM_DIR(1);}while(0)

      #else//用非usb口 推灯设置
        #if LEDB_PORT
            #define B_LED_ON()		  do{LEDB_PORT_PU |= LED_B;LEDB_PORT_PD |= LED_B;LEDB_PORT_DIR &= ~LED_B;\
                                                              LEDB_PORT_OUT &= ~LED_B;LEDB_PORT_DIE &= ~LED_B;}while(0)
            #define B_LED_OFF()		  do{LEDB_PORT_PU &= ~LED_B;LEDB_PORT_PD &= ~LED_B;LEDB_PORT_DIR |= LED_B;}while(0)
        #else
            #define B_LED_ON(...)
            #define B_LED_OFF(...)
        #endif

        #if LEDR_PORT
           #define R_LED_ON()		  do{LEDR_PORT_PU |= LED_R;LEDR_PORT_PD |= LED_R;LEDR_PORT_DIR &= ~LED_R;\
                                                              LEDR_PORT_OUT &= ~LED_R;LEDR_PORT_DIE &= ~LED_R;}while(0)
           #define R_LED_OFF()		  do{LEDR_PORT_PU &= ~LED_R;LEDR_PORT_PD &= ~LED_R;LEDR_PORT_DIR |= LED_R;}while(0)
        #else
           #define R_LED_ON(...)
           #define R_LED_OFF(...)
        #endif
      #endif

  #else//用非PWM,普通IO方式推灯设置

     #if (USE_USB_IO_LED & LED_DM_DP)////用usb dm dp推灯
        #define LED_INIT_EN()           do{USB_DP_PU(0);USB_DP_PD(0);USB_DP_DIR(0);\
                                                   USB_DM_PU(0);USB_DM_PD(0);USB_DM_DIR(0);}while(0)
        #define LED_INIT_DIS()          do{USB_DP_PU(0);USB_DP_PD(0);USB_DP_DIR(1);\
                                                   USB_DM_PU(0);USB_DM_PD(0);USB_DM_DIR(1);}while(0)
        #define R_LED_ON()              do{USB_DP_OUT(1);}while(0)
        #define R_LED_OFF()             do{USB_DP_OUT(0);}while(0)
        #define B_LED_ON()              do{USB_DM_OUT(1);}while(0)
        #define B_LED_OFF()             do{USB_DM_OUT(0);}while(0)

     #elif (USE_USB_IO_LED & LED_DM_TWO_LED)//用usb 的DM同时推两个灯R_LED, B_LED
        #define LED_INIT_EN(...)        //do{USB_DM_PU(0);USB_DM_PD(0);USB_DM_DIR(0);}while(0)
        #define LED_INIT_DIS()          do{USB_DM_PU(0);USB_DM_PD(0);USB_DM_DIR(1);}while(0)
        #define B_LED_ON()              do{USB_DM_PU(0);USB_DM_PD(0);USB_DM_DIR(0);USB_DM_OUT(0);}while(0)
        #define B_LED_OFF()             do{USB_DM_PU(0);USB_DM_PD(0);USB_DM_DIR(1);}while(0)
        #define R_LED_ON()              do{USB_DM_PU(0);USB_DM_PD(0);USB_DM_DIR(0);USB_DM_OUT(1);}while(0)
        #define R_LED_OFF()             do{USB_DM_PU(0);USB_DM_PD(0);USB_DM_DIR(1);}while(0)

     #elif (USE_USB_IO_LED & LED_DM_ONE_LED)//用usb 的DM B_LED
        #define LED_INIT_EN()           do{USB_DM_PU(0);USB_DM_PD(0);USB_DM_DIR(0);}while(0)
        #define LED_INIT_DIS()          do{USB_DM_PU(0);USB_DM_PD(0);USB_DM_DIR(1);}while(0)
        #define R_LED_ON(...)
        #define R_LED_OFF(...)
        #define B_LED_ON()              do{USB_DM_OUT(1);}while(0)
        #define B_LED_OFF()             do{USB_DM_OUT(0);}while(0)

     #else//用非usb口 ,普通IO口推灯设置
        #define LED_INIT_EN(...)
        #define LED_INIT_DIS(...)
        #if LEDB_PORT
            #define B_LED_ON()			do{LEDB_PORT_PU &= ~LED_B;LEDB_PORT_PD &= ~LED_B;LEDB_PORT_DIR &= ~LED_B;LEDB_PORT_OUT |= LED_B;}while(0)
            #define B_LED_OFF()		    do{LEDB_PORT_PU &= ~LED_B;LEDB_PORT_PD &= ~LED_B;LEDB_PORT_DIR &= ~LED_B;LEDB_PORT_OUT &= ~LED_B;}while(0)
        #else
            #define B_LED_ON(...)
            #define B_LED_OFF(...)
        #endif

        #if LEDR_PORT
            #define R_LED_ON()		   do{LEDR_PORT_PU &= ~LED_R;LEDR_PORT_PD &= ~LED_R;LEDR_PORT_DIR &= ~LED_R;LEDR_PORT_OUT |= LED_R;}while(0)
            #define R_LED_OFF()		   do{LEDR_PORT_PU &= ~LED_R;LEDR_PORT_PD &= ~LED_R;LEDR_PORT_DIR &= ~LED_R;LEDR_PORT_OUT &= ~LED_R;}while(0)
        #else
            #define R_LED_ON(...)
            #define R_LED_OFF(...)
        #endif

     #endif

 #endif //USE_PWM_CONTROL_LED

#else
     #define LED_INIT_EN(...)
     #define LED_INIT_DIS(...)
     #define R_LED_ON(...)
     #define R_LED_OFF(...)
     #define B_LED_ON(...)
     #define B_LED_OFF(...)

#endif//end LED_EN
// *INDENT-ON*

enum {
    C_ALL_OFF = 1,         ///全灭
    C_ALL_ON,              ///全亮

    C_BLED_ON,             ///蓝亮
    C_BLED_OFF,            ///蓝灭
    C_BLED_SLOW,           ///蓝慢闪
    C_BLED_FAST,           ///蓝快闪
    C_BLED_FAST_DOBLE_5S,  ///蓝灯5秒闪连闪两下
    C_BLED_FAST_ONE_5S,    ///蓝灯5秒闪连闪1下


    C_RLED_ON,             ///红亮
    C_RLED_OFF,            ///红灭
    C_RLED_SLOW,           ///红慢闪
    C_RLED_FAST,           ///红快闪
    C_RLED_FAST_DOBLE_5S,  //////红灯5秒闪连闪两下
    C_RLED_FAST_ONE_5S,    //////红灯5秒闪连闪1下

    C_RB_FAST,              ///红蓝交替闪（快闪）
    C_RB_SLOW,              ///红蓝交替闪（慢闪）
};

#define C_ALL_OFF_MODE               ((0<<8)|(C_ALL_OFF))             ///全灭
#define C_ALL_ON_MODE                ((0<<8)|(C_ALL_ON))              ///全亮

#define C_BLED_ON_MODE               ((0<<8)|(C_BLED_ON))             ///蓝亮
#define C_BLED_OFF_MODE              ((0<<8)|(C_BLED_OFF))            ///蓝灭
#define C_BLED_SLOW_MODE             ((50<<8)|(C_BLED_SLOW))          ///蓝慢闪
#define C_BLED_FAST_MODE             ((25<<8)|(C_BLED_FAST))          ///蓝快闪
#define C_BLED_FAST_DOBLE_5S_MODE    ((500<<8)|(C_BLED_FAST_DOBLE_5S))   ///蓝灯5秒闪连闪两下
#define C_BLED_FAST_ONE_5S_MODE      ((500<<8)|(C_BLED_FAST_ONE_5S))   ///蓝灯5秒闪连闪两下


#define C_RLED_ON_MODE               ((0<<8)|(C_RLED_ON))             ///红亮
#define C_RLED_OFF_MODE              ((0<<8)|(C_RLED_OFF))            ///红灭
#define C_RLED_SLOW_MODE             ((50<<8)|(C_RLED_SLOW))          ///红慢闪
#define C_RLED_FAST_MODE             ((25<<8)|(C_RLED_FAST))          ///红快闪
#define C_RLED_FAST_DOBLE_5S_MODE    ((500<<8)|(C_RLED_FAST_DOBLE_5S))///红灯5秒闪连闪两下
#define C_RLED_FAST_ONE_5S_MODE      ((500<<8)|(C_RLED_FAST_ONE_5S))  ///红灯5秒闪连闪1下

#define C_RB_FAST_MODE               ((80<<8)|(C_RB_FAST))          ///红蓝交替闪（快闪）
#define C_RB_SLOW_MODE               ((160<<8)|(C_RB_SLOW))          ///红蓝交替闪（慢闪）

#define C_RLED_LOWER_POWER_MODE        C_RLED_SLOW_MODE               ///低电红慢闪

void led_fre_set(u32 fre_type);
void led_scan();

void led_bt_sniff_init(void);

void set_led_scan(u8 en);

void clear_led_rb_flag(void);
void led_test(void);
void set_r_led_on_cnt(u8 cnt);
void lower_power_led_flash(u8 control, u32 led_flash);
#endif/*_LED_H_*/

