#ifndef _DESCRIPTOR_H_
#define _DESCRIPTOR_H_

#include "typedef.h"


extern const u8 gabSCSIModeSense[4];

extern const u8 SCSIInquiryData[36];

extern const u8 SCSIInquiryData1[36];

extern const u8 bAudioVol[];

extern const u8 bMicAudioVol[];

//---------------------------------------------------------------------------------------------------------------------
// DEVICE DESCRIPTOR
//---------------------------------------------------------------------------------------------------------------------
extern const u8 sDeviceDescriptor[0x12];


extern const u8 sConfigDescriptor[0x9];

/*********************************************************/
/*
			   Mass Storage Class
*/
/*********************************************************/
extern const u8 sMassDescriptor[0x17];


/*********************************************************/
/*
			   Audio Class
*/
/*********************************************************/
extern const u8 sAduioDescriptor[0xb8];


extern const u8 sSpeakerDescriptor[0x65];

extern const u8 sMicDescriptor[0x64];

/*********************************************************/
/*
			   HID Class
*/
/*********************************************************/
extern const u8 sHIDDescriptor[0x19];

extern const u8 sHIDReport[51];

extern const u8 LANGUAGE_STR[0x4];

extern const u8 IMANUFACTURE_STR[0x30];

extern const u8 IPRODUCT_STR[0x1A];

extern const u8 ISERIAL_NUMBER[0x12];

extern const u8 IOTHER_STRING[0x1A];

#endif  /*  _DESCRIPTOR_H_  */
