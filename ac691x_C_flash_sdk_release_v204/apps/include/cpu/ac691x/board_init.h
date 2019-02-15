#ifndef __BOARD_INIT_H__
#define __BOARD_INIT_H__

#include "typedef.h"


void board_init();
void set_port_init();
void lowpwr_setup_init(void);


typedef void (*initcall_t)(void);


#define no_sequence_initcall(fn)  \
    const initcall_t __initcall_##fn \
        sec_used(.sys.initcall) = fn

#endif
