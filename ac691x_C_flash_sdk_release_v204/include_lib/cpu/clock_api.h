#ifndef _CLOCK_API_H_
#define _CLOCK_API_H_

#include "typedef.h"
#include "clock_interface.h"

/********************************************************************************/
/*
 *      API
 */
void clock_init(SYS_CLOCK_INPUT sys_in, u32 input_freq, u32 out_freq);

u32 clock_get_usb_freq(void);

u32 clock_get_dac_freq(void);

u32 clock_get_apc_freq(void);

u32 clock_get_uart_freq(void);

u32 clock_get_bt_freq(void);

u32 clock_get_sfc_freq(void);

u32 clock_get_hsb_freq(void);

u32 clock_get_lsb_freq(void);

u32 clock_get_otp_freq(void);

u32 clock_get_iosc_freq(void);

u32 clock_get_osc_freq(void);

void clock_dump(void);

#define USB_CLK         clock_get_usb_freq()
#define DAC_CLK         clock_get_dac_freq()
#define APC_CLK         clock_get_apc_freq()
#define UART_CLK        clock_get_uart_freq()
#define BT_CLK          clock_get_bt_freq()


#define SFC_CLK         clock_get_sfc_freq()
#define SYS_CLK         clock_get_hsb_freq()
#define LSB_CLK         clock_get_lsb_freq()
#define OTP_CLK         clock_get_otp_freq()

#define IOSC_CLK        clock_get_iosc_freq()
#define OSC_CLK         clock_get_osc_freq()

void set_spi_speed_auto(void);
void set_auto_dvdd_lev_cab(void (*auto_match_dvdd_lev_callback)(u32 out_freq));
#endif
