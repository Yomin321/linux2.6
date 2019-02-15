#ifndef _USB_Host_Ctl_H_
#define _USB_Host_Ctl_H_

#include "typedef.h"

//#define DeviceDescriptor        ((DEVICE_DESCRIPTOR *)g_usb_var_p->control_trans.pBuf)//((DEVICE_DESCRIPTOR *)Xfer_buf)
//#define ConfigDescriptor        ((CONFIG_DESCRIPTOR *)g_usb_var_p->control_trans.pBuf)//((CONFIG_DESCRIPTOR *)Xfer_buf)
//#define InterfaceDescriptor     ((INTERFACE_DESCRIPTOR *)(g_usb_var_p->control_trans.pBuf + CONFIG_DESCRIPTOR_LEN))//((INTERFACE_DESCRIPTOR *)(Xfer_buf + CONFIG_DESCRIPTOR_LEN))
//#define EndpointDescriptor(x)   ((ENDPOINT_DESCRITOR *)(g_usb_var_p->control_trans.pBuf + CONFIG_DESCRIPTOR_LEN + INTERFACE_DESCRIPTOR_LEN + ENDPOINT_DESCRITOR_LEN*x))//((ENDPOINT_DESCRITOR *)(Xfer_buf + CONFIG_DESCRIPTOR_LEN + INTERFACE_DESCRIPTOR_LEN + ENDPOINT_DESCRITOR_LEN*x))

#define DEVICE_DESCRIPTOR_SIZE      0x12
#define CONFIG_DESCRIPTOR_LEN       0x9
#define INTERFACE_DESCRIPTOR_LEN    0x9
#define ENDPOINT_DESCRIPTOR_LEN     0x7

#define EP_BULKRX   0x1
#define EP_BULKTX   0x1

#define EP_HID_TX   0x2
#define EP_HID_RX   0x2

#define EP_ISO_TX   0x3

typedef struct _DEVICE_DESCRIPTOR {
    u8	bLength;			// Descriptor length ( = sizeof(DEVICEDSCR) )
    u8	bDescriptorType;	// Decriptor type (Device = 1)
    u16	bcdUSB;     	    // Specification Version (BCD)
    u8	bDeviceClass;		// Device class
    u8	bDeviceSubClass;	// Device sub-class
    u8	bDeviceProtocol;	// Device sub-sub-class
    u8	bMaxPacketSize0;	// Maximum packet size
    u16	idVendor;		    // Vendor ID
    u16	idProduct;		    // Product ID
    u16	bcdDevice;		    // Product version ID
    u8	iManufacturer;		// Manufacturer string index
    u8	iProduct;		    // Product string index
    u8	iSerialNumber;	    // Serial number string index
    u8	bNumConfigurations;		// Numder of configurations
}/*__attribute__((packed))*/ DEVICE_DESCRIPTOR;


typedef struct _CONFIG_DESCRIPTOR {
    u8  bLength;
    u8  bDescriptorType;
    u8  wTotalLength[2];
    u8  bNumInterfaces;
    u8  bConfigurationValue;
    u8  iConfiguration;
    u8  bmAttributes;
    u8  bMaxPower;
}/*__attribute__((packed))*/ CONFIG_DESCRIPTOR;


typedef struct _INTERFACE_DESCRIPTOR {
    u8  bLength;
    u8  bDescriptorType;
    u8  bInterfaceNumber;
    u8  bAlternateSetting;
    u8  bNumEndpoints;
    u8  bInterfaceClass;
    u8  bInterfaceSubClass;
    u8  bInterfaceProtocol;
    u8  iInterface;
}/*__attribute__((packed))*/ INTERFACE_DESCRIPTOR;


typedef struct _ENDPOINT_DESCRIPTOR {
    u8  bLength;
    u8  bDescriptorType;
    u8  bEndpointAddress;
    u8  bmAttributes;
    u16 wMaxPacketSize;
    u8  bInterval;
}/*__attribute__((packed))*/ ENDPOINT_DESCRITOR;



/*****************************
        Function Declare
*****************************/
u8 USB_Host_dev_enum(void *buf, u16 delay_max);
u8 usb_host_clear_feature(u8 index, void *buf);
u8 usb_control_transfers(void *buf);
void usb_descriptor_parser(void *buf);
u8 usb_ctlXfer(void *buf);
u8 usb_host_timeout(u32 jiffies);


#endif	/*	_USB_Host_Ctl_H_	*/
