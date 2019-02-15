#ifndef TIMER_H
#define TIMER_H

#include "typedef.h"

void timer0_isr_callback_fun(void);
void timer_init(void (*isr_fun)(void));
void timer_delay_2ms(volatile u32 delay_time);

enum {
    TIMER_CLK_SRC_SYSCLK          = 0,
    TIMER_CLK_SRC_IOSIGN,
    TIMER_CLK_SRC_OSC,
    TIMER_CLK_SRC_RC,
};

#endif

