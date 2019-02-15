#ifndef _SRC_H_
#define _SRC_H_

#include "typedef.h"
#include "audio/src_api.h"

#define SRC_FLTB_MAX		(2)
#define SRC_IDAT_LEN_MAX	(512)
#define SRC_ODAT_LEN_MAX	(512)
#define SRC_IDAT_LEN		(128)
#define SRC_ODAT_LEN		(128)
#define SRC_KICK_START_LEN	(10)
#define SRC_CBUFF_SIZE      (4)

extern u8 src_buffer[SRC_IDAT_LEN_MAX + SRC_ODAT_LEN_MAX + SRC_FLTB_MAX * 48];
extern u8 src_cbuffer[SRC_CBUFF_SIZE];

void src_disable();
void src_enable(src_param_t *arg);
u32 src_run(u8 *buf, u16 len);
void src_clear();
void src_kick_start(u8 start_flag);
void src_demo();

#endif
