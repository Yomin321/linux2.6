#ifndef	__USB_DEVICE_STATUS_H__
#define __USB_DEVICE_STATUS_H__

#include "typedef.h"
// #include "usb_global_var.h"

/*----------------------------------------------------------------------------*/
/**@brief   USB Slave 状态设置，触发系统事件(函数实现有判断保护)
   @param   status：在线状态
   @return  无
   @note    USB_ONLINE_STATUS usb_device_status(void)
*/
/*----------------------------------------------------------------------------*/
USB_ONLINE_STATUS usb_device_status(void);

u8 usb_slave_is_online(void);
u32 usb_slave_online_check(u32 cnt);
u32 usb_slave_online_check_simple(u32 cnt);

#endif/*__USB_DEVICE_STATUS_H__*/
