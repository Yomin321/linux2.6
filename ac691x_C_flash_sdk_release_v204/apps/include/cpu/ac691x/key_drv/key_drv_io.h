#ifndef __KEY_DRV_IO_H__
#define __KEY_DRV_IO_H__

#include "sdk_cfg.h"
#include "key.h"
#include "sys_detect.h"
#include "rtc/rtc_api.h"
// *INDENT-OFF*
////PP
#if (IO_KEY_PP_POART_SLECT&KEY_POARA)
  #define KEY_PP_PORT_DIE(...)
  #define KEY_PP_PORT_PU(x,y)   (JL_PORTA->PU |= x)
  #define KEY_PP_PORT_PD(x,y)   (JL_PORTA->PD &= ~x)
  #define KEY_PP_PORT_DIR(x,y)  (lJL_PORTA->DIR |= x)
  #define KEY_PP_PORT_IN(x)     (!(JL_PORTA->IN&x))
#elif (IO_KEY_PP_POART_SLECT&KEY_POARB)
  #define KEY_PP_PORT_DIE(...)
  #define KEY_PP_PORT_PU(x,y)   (JL_PORTB->PU |= x)
  #define KEY_PP_PORT_PD(x,y)   (JL_PORTB->PD &= ~x)
  #define KEY_PP_PORT_DIR(x,y)  (lJL_PORTB->DIR |= x)
  #define KEY_PP_PORT_IN(x)     (!(JL_PORTB->IN&x))
#elif (IO_KEY_PP_POART_SLECT&KEY_POARC)
  #define KEY_PP_PORT_DIE(...)
  #define KEY_PP_PORT_PU(x,y)   (JL_PORTC->PU |= x)
  #define KEY_PP_PORT_PD(x,y)   (JL_PORTC->PD &= ~x)
  #define KEY_PP_PORT_DIR(x,y)  (lJL_PORTC->DIR |= x)
  #define KEY_PP_PORT_IN(x)     (!(JL_PORTC->IN&x))
#elif (IO_KEY_PP_POART_SLECT&KEY_POARD)
  #define KEY_PP_PORT_DIE(...)
  #define KEY_PP_PORT_PU(x,y)   (JL_PORTD->PU |= x)
  #define KEY_PP_PORT_PD(x,y)   (JL_PORTD->PD &= ~x)
  #define KEY_PP_PORT_DIR(x,y)  (lJL_PORTD->DIR |= x)
  #define KEY_PP_PORT_IN(x)     (!(JL_PORTD->IN&x))

#elif (IO_KEY_PP_POART_SLECT&KEY_PORT_PR)
  #define KEY_PP_PORT_DIE(x,y)  PORTR_DIE(x,y)
  #define KEY_PP_PORT_PU(x,y)   PORTR_PU(x,y)
  #define KEY_PP_PORT_PD(x,y)   PORTR_PD(x,y)
  #define KEY_PP_PORT_DIR(x,y)  PORTR_DIR(x,y)
  #define KEY_PP_PORT_IN(x)   (!PORTR_IN(x))

#elif (IO_KEY_PP_POART_SLECT&KEY_PORT_DM)
  #define KEY_PP_PORT_DIE(...)  //
  #define KEY_PP_PORT_PU(x,y)   USB_DM_PU(1)
  #define KEY_PP_PORT_PD(x,y)   USB_DM_PD(0)
  #define KEY_PP_PORT_DIR(x,y)  USB_DM_DIR(1)
  #define KEY_PP_PORT_IN(x,y)   (!(USB_DM_IN()))

#elif (IO_KEY_PP_POART_SLECT&KEY_PORT_DP)
  #define KEY_PP_PORT_DIE(...)  //
  #define KEY_PP_PORT_PU(x,y)   USB_DP_PU(y)
  #define KEY_PP_PORT_PD(x,y)   USB_DP_PD(y)
  #define KEY_PP_PORT_DIR(x,y)  USB_DP_DIR(y)
  #define KEY_PP_PORT_IN(x)   (!(USB_DP_IN()))
#else
  #define KEY_PP_PORT_DIE(...)
  #define KEY_PP_PORT_PU(...)
  #define KEY_PP_PORT_PD(...)
  #define KEY_PP_PORT_DIR(...)
  #define KEY_PP_PORT_IN(x)     0
#endif

///NEXT
#if (IO_KEY_NEXT_POART_SLECT&KEY_POARA)
  #define KEY_NEXT_PORT_DIE(...)
  #define KEY_NEXT_PORT_PU(x,y)   (JL_PORTA->PU |= x)
  #define KEY_NEXT_PORT_PD(x,y)   (JL_PORTA->PD &= ~x)
  #define KEY_NEXT_PORT_DIR(x,y)  (JL_PORTA->DIR |= x)
  #define KEY_NEXT_PORT_IN(x)     (!(JL_PORTA->IN&x))
#elif (IO_KEY_NEXT_POART_SLECT&KEY_POARB)
  #define KEY_NEXT_PORT_DIE(...)
  #define KEY_NEXT_PORT_PU(x,y)   (JL_PORTB->PU |= x)
  #define KEY_NEXT_PORT_PD(x,y)   (JL_PORTB->PD &= x)
  #define KEY_NEXT_PORT_DIR(x,y)  (lJL_PORTB->DIR |= x)
  #define KEY_NEXT_PORT_IN(x)     (!(JL_PORTB->IN&x))
#elif (IO_KEY_NEXT_POART_SLECT&KEY_POARC)
  #define KEY_NEXT_PORT_DIE(...)
  #define KEY_NEXT_PORT_PU(x,y)   (JL_PORTC->PU |= x)
  #define KEY_NEXT_PORT_PD(x,y)   (JL_PORTC->PD &= ~x)
  #define KEY_NEXT_PORT_DIR(x,y)  (JL_PORTC->DIR |= x)
  #define KEY_NEXT_PORT_IN(x)     (!(JL_PORTC->IN&x))
#elif (IO_KEY_NEXT_POART_SLECT&KEY_POARD)
  #define KEY_NEXT_PORT_DIE(...)
  #define KEY_NEXT_PORT_PU(x,y)   (JL_PORTD->PU |= x)
  #define KEY_NEXT_PORT_PD(x,y)   (JL_PORTD->PD &= ~x)
  #define KEY_NEXT_PORT_DIR(x,y)  (JL_PORTD->DIR |= x)
  #define KEY_NEXT_PORT_IN(x)     (!(JL_PORTD->IN&x))

#elif (IO_KEY_NEXT_POART_SLECT&KEY_PORT_PR)
  #define KEY_NEXT_PORT_DIE(x,y)  PORTR_DIE(x,y)
  #define KEY_NEXT_PORT_PU(x,y)   PORTR_PU(x,y)
  #define KEY_NEXT_PORT_PD(x,y)   PORTR_PD(x,y)
  #define KEY_NEXT_PORT_DIR(x,y)  PORTR_DIR(x,y)
  #define KEY_NEXT_PORT_IN(x)     (!PORTR_IN(x))

#elif (IO_KEY_NEXT_POART_SLECT&KEY_PORT_DM)
  #define KEY_NEXT_PORT_DIE(...)  //
  #define KEY_NEXT_PORT_PU(x,y)   USB_DM_PU(1)
  #define KEY_NEXT_PORT_PD(x,y)   USB_DM_PD(0)
  #define KEY_NEXT_PORT_DIR(x,y)  USB_DM_DIR(1)
  #define KEY_NEXT_PORT_IN(x)   (!(USB_DM_IN()))

#elif (IO_KEY_NEXT_POART_SLECT&KEY_PORT_DP)
  #define KEY_NEXT_PORT_DIE(...)  //
  #define KEY_NEXT_PORT_PU(x,y)   USB_DP_PU(y)
  #define KEY_NEXT_PORT_PD(x,y)   USB_DP_PD(y)
  #define KEY_NEXT_PORT_DIR(x,y)  USB_DP_DIR(y)
  #define KEY_NEXT_PORT_IN(x)   (!(USB_DP_IN()))
#else
  #define KEY_NEXT_PORT_DIE(...)
  #define KEY_NEXT_PORT_PU(...)
  #define KEY_NEXT_PORT_PD(...)
  #define KEY_NEXT_PORT_DIR(...)
  #define KEY_NEXT_PORT_IN(x)     0
#endif


///PREV
#if (IO_KEY_PREV_POART_SLECT&KEY_POARA)
  #define KEY_PREV_PORT_DIE(...)
  #define KEY_PREV_PORT_PU(x,y)   (JL_PORTA->PU |= x)
  #define KEY_PREV_PORT_PD(x,y)   (JL_PORTA->PD &= ~x)
  #define KEY_PREV_PORT_DIR(x,y)  (JL_PORTA->DIR |= x)
  #define KEY_PREV_PORT_IN(x)     (!(JL_PORTA->IN&x))
#elif (IO_KEY_PREV_POART_SLECT&KEY_POARB)
  #define KEY_PREV_PORT_DIE(...)
  #define KEY_PREV_PORT_PU(x,y)   (JL_PORTB->PU |= x)
  #define KEY_PREV_PORT_PD(x,y)   (JL_PORTB->PD &= ~x)
  #define KEY_PREV_PORT_DIR(x,y)  (JL_PORTB->DIR |= x)
  #define KEY_PREV_PORT_IN(x)     (!(JL_PORTB->IN&x))
#elif (IO_KEY_PREV_POART_SLECT&KEY_POARC)
  #define KEY_PREV_PORT_DIE(...)
  #define KEY_PREV_PORT_PU(x,y)   (JL_PORTC->PU |= x)
  #define KEY_PREV_PORT_PD(x,y)   (JL_PORTC->PD &= ~x)
  #define KEY_PREV_PORT_DIR(x,y)  (JL_PORTC->DIR |= x)
  #define KEY_PREV_PORT_IN(x)     (!(JL_PORTC->IN&x))
#elif (IO_KEY_PREV_POART_SLECT&KEY_POARD)
  #define KEY_PREV_PORT_DIE(...)
  #define KEY_PREV_PORT_PU(x,y)   (JL_PORTD->PU |= x)
  #define KEY_PREV_PORT_PD(x,y)   (JL_PORTD->PD &= ~x)
  #define KEY_PREV_PORT_DIR(x,y)  (JL_PORTD->DIR |= x)
  #define KEY_PREV_PORT_IN(x)     (!(JL_PORTD->IN&x))

#elif (IO_KEY_PREV_POART_SLECT&KEY_PORT_PR)
  #define KEY_PREV_PORT_DIE(x,y)  PORTR_DIE(x,y)
  #define KEY_PREV_PORT_PU(x,y)   PORTR_PU(x,y)
  #define KEY_PREV_PORT_PD(x,y)   PORTR_PD(x,y)
  #define KEY_PREV_PORT_DIR(x,y)  PORTR_DIR(x,y)
  #define KEY_PREV_PORT_IN(x)     (!PORTR_IN(x))

#elif (IO_KEY_PREV_POART_SLECT&KEY_PORT_DM)
  #define KEY_PREV_PORT_DIE(...)  //
  #define KEY_PREV_PORT_PU(x,y)   USB_DM_PU(1)
  #define KEY_PREV_PORT_PD(x,y)   USB_DM_PD(0)
  #define KEY_PREV_PORT_DIR(x,y)  USB_DM_DIR(1)
  #define KEY_PREV_PORT_IN(x)   (!(USB_DM_IN()))

#elif (IO_KEY_PREV_POART_SLECT&KEY_PORT_DP)
  #define KEY_PREV_PORT_DIE(...)  //
  #define KEY_PREV_PORT_PU(x,y)   USB_DP_PU(y)
  #define KEY_PREV_PORT_PD(x,y)   USB_DP_PD(y)
  #define KEY_PREV_PORT_DIR(x,y)  USB_DP_DIR(y)
  #define KEY_PREV_PORT_IN(x)     (!(USB_DP_IN()))
#else
  #define KEY_PREV_PORT_DIE(...)
  #define KEY_PREV_PORT_PU(...)
  #define KEY_PREV_PORT_PD(...)
  #define KEY_PREV_PORT_DIR(...)
  #define KEY_PREV_PORT_IN(x)     0
#endif

#define IS_KEY0_DOWN()    KEY_NEXT_PORT_IN(IO_KEY_NEXT_N)
#define IS_KEY1_DOWN()    KEY_PREV_PORT_IN(IO_KEY_PREV_N)
#define IS_KEY2_DOWN()    KEY_PP_PORT_IN(IO_KEY_PP_N)
#define IS_KEY3_DOWN()    0


#define KEY_INIT()        do{\
                             KEY_PP_PORT_PU(IO_KEY_PP_N,1);KEY_PP_PORT_PD(IO_KEY_PP_N,0);KEY_PP_PORT_DIR(IO_KEY_PP_N,1);KEY_PP_PORT_DIE(IO_KEY_PP_N,1);\
                             KEY_NEXT_PORT_PU(IO_KEY_NEXT_N,1);KEY_NEXT_PORT_PD(IO_KEY_NEXT_N,0);KEY_NEXT_PORT_DIR(IO_KEY_NEXT_N,1);KEY_NEXT_PORT_DIE(IO_KEY_NEXT_N,1);\
                             KEY_PREV_PORT_PU(IO_KEY_PREV_N,1);KEY_PREV_PORT_PD(IO_KEY_PREV_N,0);KEY_PREV_PORT_DIR(IO_KEY_PREV_N,1);KEY_PREV_PORT_DIE(IO_KEY_PREV_N,1);\
					    	}while(0)

// *INDENT-ON*

extern const key_interface_t key_io_info;

#endif/*__KEY_DRV_IO_H__*/
