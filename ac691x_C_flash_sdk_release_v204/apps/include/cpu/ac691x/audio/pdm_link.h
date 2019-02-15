#ifndef _PDM_LINK_H
#define _PDM_LINK_H

#include "typedef.h"


void pdm_link_enable(void);

void pdm_link_disable(void);

void set_plink_data_callback(void (*callback)(s16 *, u16));

void pdm_link_demo(void);

#endif  //_PDM_LINK_H
