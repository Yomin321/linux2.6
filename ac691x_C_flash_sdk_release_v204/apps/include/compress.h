#ifndef _COMPRESS_H_
#define _COMPRESS_H_

#include "typedef.h"

#define QLZ_DATA_EN	1

#if QLZ_DATA_EN
#define QLZ_DATA	0
#define QLZ_INIT	1
#define QLZ_COMM	2
#else
#define QLZ_INIT	0
#define QLZ_COMM	1
#endif

void bank_switch(u32 bank_num);

#endif
