#ifndef _USB_SCSI_H_
#define _USB_SCSI_H_

#include "typedef.h"
#include "cpu.h"
#include "irq_api.h"


#ifdef BIG_ENDIAN
#define CBW_SIGNATURE   0x55534243L
#define CSW_SIGNATURE   0x55534253L
#define CBW_TAG         0x13248658L
#endif

#ifdef LITTLE_ENDIAN
#define CBW_SIGNATURE   0x43425355L
#define CSW_SIGNATURE   0x53425355L
#define CBW_TAG         0x13248658L
#endif



#define	min(a,b) (((a)<(b))?(a):(b))
//+=============================================================================+
//|           D - DIRECT ACCESS DEVICE                       Device Column Key  |
//|           .T - SEQUENTIAL ACCESS DEVICE                  M = Mandatory      |
//|           . L - PRINTER DEVICE                           O = Optional       |
//|           .  P - PROCESSOR DEVICE                        V = Vendor Specific|
//|           .  .W - WRITE ONCE READ MULTIPLE DEVICE        R = Reserved       |
//|           .  . R - READ ONLY (CD-ROM) DEVICE                                |
//|           .  .  S - SCANNER DEVICE                                          |
//|           .  .  .O - OPTICAL MEMORY DEVICE                                  |
//|           .  .  . M - MEDIA CHANGER DEVICE                                  |
//|           .  .  .  C - COMMUNICATION DEVICE                                 |
//|           .  .  .  .                                                        |
//|        OP DTLPWRSOMC Description                                            |
//|----------+----------+-------------------------------------------------------|
//|        00 MMMMMMMMMM TEST UNIT READY                                        |
#define TEST_UNIT_READY                                        0x00
//|        01  M         REWIND                                                 |
//#define REWIND                                                 0x01
//|        01 O V OO OO  REZERO UNIT                                            |
#define REZERO_UNIT                                            0x01
//|        02 VVVVVV  V                                                         |
//|        03 MMMMMMMMMM REQUEST SENSE                                          |
#define REQUEST_SENSE                                          0x03
//|        04   O        FORMAT                                                 |
#define FORMAT                                                 0x04
//|        04 M      O   FORMAT UNIT                                            |
#define FORMAT_UNIT                                            0x04
//|        05 VMVVVV  V  READ BLOCK LIMITS                                      |
#define READ_BLOCK_LIMITS                                      0x05
//|        06 VVVVVV  V                                                         |
//|        07         O  INITIALIZE ELEMENT STATUS                              |
#define INITIALIZE_ELEMENT_STATUS                              0x07
//|        07 OVV O  OV  REASSIGN BLOCKS                                        |
#define REASSIGN_BLOCKS                                        0x07
//|        08          M GET MESSAGE(06)                                        |
#define GET_MESSAGE_06                                        0x08
//|        08 OMV OO OV  READ(06)                                               |
#define READ_06                                               0x08
//|        08    O       RECEIVE                                                |
#define RECEIVE                                                0x08
//|        09 VVVVVV  V                                                         |
//|        0A   M        PRINT                                                  |
#define PRINT                                                  0x0A
//|        0A          M SEND MESSAGE(06)                                       |
#define SEND_MESSAGE_06                                       0x0A
//|        0A    M       SEND(06)                                               |
#define SEND_06                                               0x0A
//|        0A OM  O  OV  WRITE(06)                                              |
#define WRITE_06                                              0x0A
//|        0B O   OO OV  SEEK(06)                                               |
#define SEEK_06                                               0x0B
//|        0B   O        SLEW AND PRINT                                         |
#define SLEW_AND_PRINT                                         0x0B
//|        0C VVVVVV  V                                                         |
//|        0D VVVVVV  V                                                         |
//|        0E VVVVVV  V                                                         |
//|        0F VOVVVV  V  READ REVERSE                                           |
#define READ_REVERSE                                           0x0F
//|        10   O O      SYNCHRONIZE BUFFER                                     |
#define SYNCHRONIZE_BUFFER                                     0x10
//|        10 VM VVV     WRITE FILEMARKS                                        |
#define WRITE_FILEMARKS                                        0x10
//|        11 VMVVVV     SPACE                                                  |
#define SPACE                                                  0x11
//|        12 MMMMMMMMMM INQUIRY                                                |
#define INQUIRY                                                0x12
//|        13 VOVVVV     VERIFY(06)                                             |
#define VERIFY_06                                             0x13
//|        14 VOOVVV     RECOVER BUFFERED DATA                                  |
#define RECOVER_BUFFERED_DATA                                  0x14
//|        15 OMO OOOOOO MODE SELECT(06)                                        |
#define MODE_SELECT_06                                        0x15
//|        16 M   MM MO  RESERVE                                                |
#define RESERVE_CMD                                           0x16
//|        16  MM   M    RESERVE UNIT                                           |
#define RESERVE_UNIT                                           0x16
//|        17 M   MM MO  RELEASE                                                |
#define RELEASE                                                0x17
//|        17  MM   M    RELEASE UNIT                                           |
#define RELEASE_UNIT                                           0x17
//|        18 OOOOOOOO   COPY                                                   |
#define COPY                                                   0x18
//|        19 VMVVVV     ERASE                                                  |
#define ERASE                                                  0x19
//|        1A OMO OOOOOO MODE SENSE(06)                                         |
#define MODE_SENSE_06                                         0x1A
//|        1B  O         LOAD UNLOAD                                            |
#define LOAD_UNLOAD                                            0x1B
//|        1B       O    SCAN                                                   |
#define SCAN                                                   0x1B
//|        1B   O        STOP PRINT                                             |
#define STOP_PRINT                                             0x1B
//|        1B O   OO O   STOP START UNIT                                        |
#define STOP_START_UNIT                                        0x1B
//+=============================================================================+
//
//
//Table 365: (continued)
//+=============================================================================+
//|           D - DIRECT ACCESS DEVICE                       Device Column Key  |
//|           .T - SEQUENTIAL ACCESS DEVICE                  M = Mandatory      |
//|           . L - PRINTER DEVICE                           O = Optional       |
//|           .  P - PROCESSOR DEVICE                        V = Vendor Specific|
//|           .  .W - WRITE ONCE READ MULTIPLE DEVICE        R = Reserved       |
//|           .  . R - READ ONLY (CD-ROM) DEVICE                                |
//|           .  .  S - SCANNER DEVICE                                          |
//|           .  .  .O - OPTICAL MEMORY DEVICE                                  |
//|           .  .  . M - MEDIA CHANGER DEVICE                                  |
//|           .  .  .  C - COMMUNICATION DEVICE                                 |
//|           .  .  .  .                                                        |
//|        OP DTLPWRSOMC Description                                            |
//|----------+----------+-------------------------------------------------------|
//|        1C OOOOOOOOOO RECEIVE DIAGNOSTIC RESULTS                             |
#define RECEIVE_DIAGNOSTIC_RESULTS                             0x1C
//|        1D MMMMMMMMMM SEND DIAGNOSTIC                                        |
#define SEND_DIAGNOSTIC                                        0x1D
//|        1E OO  OO OO  PREVENT ALLOW MEDIUM REMOVAL                           |
#define PREVENT_ALLOW_MEDIUM_REMOVAL                           0x1E
//|        1F                                                                   |
//|        20 V   VV V                                                          |
//|        21 V   VV V                                                          |
//|        22 V   VV V                                                          |
//|        23 V   VV V                                                          |
//|        24 V   VVM    SET WINDOW
#define READ_FORMAT_CAPACITY                                   0x23
#define SET_WINDOW                                             0x24
//|        25       O    GET WINDOW                                             |
#define GET_WINDOW                                             0x25
//|        25 M   M  M   READ CAPACITY                                          |
#define READ_CAPACITY                                          0x25
//|        25      M     READ CD-ROM CAPACITY                                   |
#define READ_CD_ROM_CAPACITY                                   0x25
#define SCSI_CAPACITY_SECTOR_COUNT_MSB  2
#define SCSI_CAPACITY_SECTOR_COUNT_LSB  3
#define SCSI_CAPACITY_SECTOR_SIZE_MSB  6
#define SCSI_CAPACITY_SECTOR_SIZE_LSB  7
//|        26 V   VV                                                            |
//|        27 V   VV                                                            |
//|        28          O GET MESSAGE(10)                                        |
#define GET_MESSAGE_10                                        0x28
//|        28 M   MMMM   READ(10)                                               |
#define READ_10                                               0x28
//|        29 V   VV O   READ GENERATION                                        |
#define READ_GENERATION                                        0x29
//|        2A          O SEND MESSAGE(10)                                       |
#define SEND_MESSAGE_10                                       0x2A
//|        2A       O    SEND(10)                                               |
#define SEND_10                                               0x2A
//|        2A M   M  M   WRITE(10)                                              |
#define WRITE_10                                              0x2A
//|        2B  O         LOCATE                                                 |
#define LOCATE                                                 0x2B
//|        2B         O  POSITION TO ELEMENT                                    |
#define POSITION_TO_ELEMENT                                    0x2B
//|        2B O   OO O   SEEK(10)                                               |
#define SEEK_10                                               0x2B
//|        2C V      O   ERASE(10)                                              |
#define ERASE_10                                              0x2C
//|        2D V   O  O   READ UPDATED BLOCK                                     |
#define READ_UPDATED_BLOCK                                     0x2D
//|        2E O   O  O   WRITE AND VERIFY(10)                                   |
#define WRITE_AND_VERIFY_10                                   0x2E
//|        2F O   OO O   VERIFY(10)                                             |
#define VERIFY_10                                             0x2F
//|        30 O   OO O   SEARCH DATA HIGH(10)                                   |
#define SEARCH_DATA_HIGH_10                                   0x30
//|        31       O    OBJECT POSITION                                        |
#define OBJECT_POSITION                                        0x31
//|        31 O   OO O   SEARCH DATA EQUAL(10)                                  |
#define SEARCH_DATA_EQUAL_10                                  0x31
//|        32 O   OO O   SEARCH DATA LOW(10)                                    |
#define SEARCH_DATA_LOW_10                                    0x32
//|        33 O   OO O   SET LIMITS(10)                                         |
#define SET_LIMITS_10                                         0x33
//|        34       O    GET DATA BUFFER STATUS                                 |
#define GET_DATA_BUFFER_STATUS                                 0x34
//|        34 O   OO O   PRE-FETCH                                              |
//#define PRE-FETCH                                              0x34
//|        34  O         READ POSITION                                          |
#define READ_POSITION                                          0x34
//|        35 O   OO O   SYNCHRONIZE CACHE                                      |
#define SYNCHRONIZE_CACHE                                      0x35
//|        36 O   OO O   LOCK UNLOCK CACHE                                      |
#define LOCK_UNLOCK_CACHE                                      0x36
//|        37 O      O   READ DEFECT DATA(10)                                   |
#define READ_DEFECT_DATA_10                                   0x37
//|        38     O  O   MEDIUM SCAN                                            |
#define MEDIUM_SCAN                                            0x38
//|        39 OOOOOOOO   COMPARE                                                |
#define COMPARE                                                0x39
//|        3A OOOOOOOO   COPY AND VERIFY                                        |
#define COPY_AND_VERIFY                                        0x3A
//|        3B OOOOOOOOOO WRITE BUFFER                                           |
#define WRITE_BUFFER                                           0x3B
//|        3C OOOOOOOOOO READ BUFFER                                            |
#define READ_BUFFER                                            0x3C
//|        3D     O  O   UPDATE BLOCK                                           |
#define UPDATE_BLOCK                                           0x3D
//|        3E O   OO O   READ LONG                                              |
#define READ_LONG                                              0x3E
//|        3F O   O  O   WRITE LONG                                             |
#define WRITE_LONG                                             0x3F
//+=============================================================================+
//
//
//Table 365: (continued)
//+=============================================================================+
//|           D - DIRECT ACCESS DEVICE                       Device Column Key  |
//|           .T - SEQUENTIAL ACCESS DEVICE                  M = Mandatory      |
//|           . L - PRINTER DEVICE                           O = Optional       |
//|           .  P - PROCESSOR DEVICE                        V = Vendor Specific|
//|           .  .W - WRITE ONCE READ MULTIPLE DEVICE        R = Reserved       |
//|           .  . R - READ ONLY (CD-ROM) DEVICE                                |
//|           .  .  S - SCANNER DEVICE                                          |
//|           .  .  .O - OPTICAL MEMORY DEVICE                                  |
//|           .  .  . M - MEDIA CHANGER DEVICE                                  |
//|           .  .  .  C - COMMUNICATION DEVICE                                 |
//|           .  .  .  .                                                        |
//|        OP DTLPWRSOMC Description                                            |
//|----------+----------+-------------------------------------------------------|
//|        40 OOOOOOOOOO CHANGE DEFINITION                                      |
#define CHANGE_DEFINITION                                      0x40
//|        41 O          WRITE SAME                                             |
#define WRITE_SAME                                             0x41
//|        42      O     READ SUB-CHANNEL                                       |
#define READ_SUB_CHANNEL                                       0x42
//|        43      O     READ TOC                                               |
#define READ_TOC                                               0x43
//|        44      O     READ HEADER                                            |
#define READ_HEADER                                            0x44
//|        45      O     PLAY AUDIO(10)                                         |
#define PLAY_AUDIO_10                                         0x45
//|        46                                                                   |
//|        47      O     PLAY AUDIO MSF                                         |
#define PLAY_AUDIO_MSF                                         0x47
//|        48      O     PLAY AUDIO TRACK INDEX                                 |
#define PLAY_AUDIO_TRACK_INDEX                                 0x48
//|        49      O     PLAY TRACK RELATIVE(10)                                |
#define PLAY_TRACK_RELATIVE_10                                0x49
//|        4A                                                                   |
//|        4B      O     PAUSE RESUME                                           |
#define PAUSE_RESUME                                           0x4B
//|        4C OOOOOOOOOO LOG SELECT                                             |
#define LOG_SELECT                                             0x4C
//|        4D OOOOOOOOOO LOG SENSE                                              |
#define LOG_SENSE                                              0x4D
//|        4E                                                                   |
//|        4F                                                                   |
//|        50                                                                   |
//|        51                                                                   |
//|        52                                                                   |
//|        53                                                                   |
//|        54                                                                   |
//|        55 OOO OOOOOO MODE SELECT(10)                                        |
#define MODE_SELECT_10                                        0x55
//|        56                                                                   |
//|        57                                                                   |
//|        58                                                                   |
//|        59                                                                   |
//|        5A OOO OOOOOO MODE SENSE(10)                                         |
#define MODE_SENSE_10                                         0x5A
//|        5B                                                                   |
//|        5C                                                                   |
//|        5D                                                                   |
//|        5E                                                                   |
//|        5F                                                                   |
//+=============================================================================+
//
//
//Table 365: (concluded)
//+=============================================================================+
//|           D - DIRECT ACCESS DEVICE                       Device Column Key  |
//|           .T - SEQUENTIAL ACCESS DEVICE                  M = Mandatory      |
//|           . L - PRINTER DEVICE                           O = Optional       |
//|           .  P - PROCESSOR DEVICE                        V = Vendor Specific|
//|           .  .W - WRITE ONCE READ MULTIPLE DEVICE        R = Reserved       |
//|           .  . R - READ ONLY (CD-ROM) DEVICE                                |
//|           .  .  S - SCANNER DEVICE                                          |
//|           .  .  .O - OPTICAL MEMORY DEVICE                                  |
//|           .  .  . M - MEDIA CHANGER DEVICE                                  |
//|           .  .  .  C - COMMUNICATION DEVICE                                 |
//|           .  .  .  .                                                        |
//|        OP DTLPWRSOMC Description                                            |
//|----------+----------+-------------------------------------------------------|
//|        A0                                                                   |
//|        A1                                                                   |
//|        A2                                                                   |
//|        A3                                                                   |
//|        A4                                                                   |
//|        A5         M  MOVE MEDIUM                                            |
#define MOVE_MEDIUM                                            0xA5
//|        A5      O     PLAY AUDIO(12)                                         |
#define PLAY_AUDIO_12                                         0xA5
//|        A6         O  EXCHANGE MEDIUM                                        |
#define EXCHANGE_MEDIUM                                        0xA6
//|        A7                                                                   |
//|        A8          O GET MESSAGE(12)                                        |
#define GET_MESSAGE_12                                        0xA8
//|        A8     OO O   READ(12)                                               |
#define READ_12                                               0xA8
//|        A9      O     PLAY TRACK RELATIVE(12)                                |
#define PLAY_TRACK_RELATIVE_12                                0xA9
//|        AA          O SEND MESSAGE(12)                                       |
#define SEND_MESSAGE_12                                       0xAA
//|        AA     O  O   WRITE(12)                                              |
#define WRITE_12                                              0xAA
//|        AB                                                                   |
//|        AC        O   ERASE(12)                                              |
#define ERASE_12                                              0xAC
//|        AD                                                                   |
//|        AE     O  O   WRITE AND VERIFY(12)                                   |
#define WRITE_AND_VERIFY_12                                   0xAE
//|        AF     OO O   VERIFY(12)                                             |
#define VERIFY_12                                             0xAF
//|        B0     OO O   SEARCH DATA HIGH(12)                                   |
#define SEARCH_DATA_HIGH_12                                   0xB0
//|        B1     OO O   SEARCH DATA EQUAL(12)                                  |
#define SEARCH_DATA_EQUAL_12                                  0xB1
//|        B2     OO O   SEARCH DATA LOW(12)                                    |
#define SEARCH_DATA_LOW_12                                    0xB2
//|        B3     OO O   SET LIMITS(12)                                         |
#define SET_LIMITS_12                                         0xB3
//|        B4                                                                   |
//|        B5                                                                   |
//|        B5         O  REQUEST VOLUME ELEMENT ADDRESS                         |
#define REQUEST_VOLUME_ELEMENT_ADDRESS                         0xB5
//|        B6                                                                   |
//|        B6         O  SEND VOLUME TAG                                        |
#define SEND_VOLUME_TAG                                        0xB6
//|        B7        O   READ DEFECT DATA(12)                                   |
#define READ_DEFECT_DATA_12                                   0xB7
//|        B8                                                                   |
//|        B8         O  READ ELEMENT STATUS
#define READ_ELEMENT_STATUS                                   0xB8


/* Sense Key definition*/
#define SENSE_KEY_NO_SENSE					0x00
#define SENSE_KEY_RECOVERED_ERROR			0x01
#define SENSE_KEY_NOT_READY					0x02
#define SENSE_KEY_MEDIUM_ERROR				0x03
#define SENSE_KEY_HARDWARE_ERROR			0x04
#define SENSE_KEY_ILLEGAL_REQUEST			0x05
#define SENSE_KEY_UNIT_ATTENTION			0x06
#define SENSE_KEY_DATA_PROTECT				0x07
#define SENSE_KEY_BLANK_CHECK				0x08
#define SENSE_KEY_VENDOR_SPECIFIC			0x09
#define SENSE_KEY_ABORTED_COMMAND			0x0B
#define SENSE_KEY_VOLUME_OVERFLOW			0x0D
#define SENSE_KEY_MIS_COMPARE				0x0E

/* Additional Sense code definition*/
#define ASC_NO_ADDITIONAL_SENSE_INFORMATION		0x00
#define ASC_RECOVERED_DATA_WITH_RETRIES			0x17
#define ASC_RECOVERED_DATA_WITH_ECC				0x18
#define ASC_MEDIUM_PRESENT						0x3A
#define ASC_LOGICAL_DRIVE_NOT_READY_BEING_READY	0x04
#define ASC_LOGICAL_DRIVE_NOT_READY_FMT_IN_PRGS	0x04
#define ASC_NO_REFERENCE_POSITION_FOUND			0x06
#define ASC_NO_SEEK_COMPLETE					0x02
#define ASC_WRITE_FAULT							0x03
#define ASC_ID_CRC_ERROR						0x10
#define ASC_UNRECOVERED_READ_ERROR				0x11
#define ASC_ADDRESS_MARK_NOT_FOUND_FOR_ID_FIELD	0x12
#define ASC_RECORDED_ENTITY_NOT_FOUND			0x14
#define ASC_INCOMPATIBLE_MEDIUM_INSTALLED		0x30
#define ASC_CANNOT_READ_MEDIUM_INCOMPATIBLE_FMT	0x30
#define ASC_CANNOT_READ_MEDIUM_UNKNOWN_FORMAT	0x30
#define ASC_FORMAT_COMMAND_FAILED				0x31
#define ASC_INVALID_COMMAND_OPERATION_CODE		0x20
#define ASC_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE	0x21
#define ASC_INVALID_FIELD_IN_COMMAND_PACKET		0x24
#define ASC_LOGICAL_UNIT_NOT_SUPPORTED			0x25
#define ASC_INVALID_FIELD_IN_PARAMETER_LIST		0x26
#define ASC_MEDIUM_REMOVAL_PREVENTED			0x53
#define ASC_NOT_READY_TO_READY_TRANSIT_MDI_CHNG	0x28
#define ASC_POWER_ON_OR_BUS_DEVICE_RESET		0x29
#define ASC_WRITE_PROTECTED_MEDIA				0x27
#define ASC_OVERLAPPED_COMMAND_ATTEMPTED		0x4E

/* Definition of additional sense code qualifier*/
/* Additional Sense code definition */
#define ASCQ_NO_ADDITIONAL_SENSE_INFORMATION		0x00
#define ASCQ_RECOVERED_DATA_WITH_RETRIES			0x01
#define ASCQ_RECOVERED_DATA_WITH_ECC				0x00
#define ASCQ_MEDIUM_PRESENT							0x00
#define ASCQ_LOGICAL_DRIVE_NOT_READY_BEING_READY	0x01
#define ASCQ_LOGICAL_DRIVE_NOT_READY_FMT_IN_PRGS	0x04
#define ASCQ_NO_REFERENCE_POSITION_FOUND			0x00
#define ASCQ_NO_SEEK_COMPLETE						0x00
#define ASCQ_WRITE_FAULT							0x00
#define ASCQ_ID_CRC_ERROR							0x00
#define ASCQ_UNRECOVERED_READ_ERROR					0x00
#define ASCQ_ADDRESS_MARK_NOT_FOUND_FOR_ID_FIELD	0x00
#define ASCQ_RECORDED_ENTITY_NOT_FOUND				0x00
#define ASCQ_INCOMPATIBLE_MEDIUM_INSTALLED			0x00
#define ASCQ_CANNOT_READ_MEDIUM_INCOMPATIBLE_FMT	0x02
#define ASCQ_CANNOT_READ_MEDIUM_UNKNOWN_FORMAT		0x01
#define ASCQ_FORMAT_COMMAND_FAILED					0x01
#define ASCQ_INVALID_COMMAND_OPERATION_CODE			0x00
#define ASCQ_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE		0x00
#define ASCQ_INVALID_FIELD_IN_COMMAND_PACKET		0x00
#define ASCQ_LOGICAL_UNIT_NOT_SUPPORTED				0x00
#define ASCQ_INVALID_FIELD_IN_PARAMETER_LIST		0x00
#define ASCQ_MEDIUM_REMOVAL_PREVENTED				0x02
#define ASCQ_NOT_READY_TO_READY_TRANSIT_MDI_CHNG	0x00
#define ASCQ_POWER_ON_OR_BUS_DEVICE_RESET			0x00
#define ASCQ_WRITE_PROTECTED_MEDIA					0x00
#define ASCQ_OVERLAPPED_COMMAND_ATTEMPTED			0x00


#define	RBC_SDPASS			1
#define	RBC_SDFAIL			0
#define ATA_SECTOR_SIZE 	0x200

//USB Protocol
#define     ILLEGAL              (0 << 4)
#define     ISOCHRONOUS          (1 << 4)
#define     BULK                 (2 << 4)
#define     INTERRUPT            (3 << 4)
//#define USB_DATA_END()      USB_CON0 |= BIT(6);
/*USB Host Control FSM*/
enum {
    FSM_Attached = 0,   /*连接状态*/
    FSM_Powered,        /*上电状态*/
    FSM_Default,        /*缺省状态*/
    FSM_Address,        /*地址状态*/
    FSM_Configured,     /*配置状态*/
    FSM_GetDescriptor_Device,
    FSM_GetDescriptor_Config,
    FSM_GetDescriptor_Config1,
    FSM_Set_Config,
    FSM_Get_Max_LUN,
    FSM_CTL_END,
};

/*USB Host MassStorage FSM*/
enum {
    FSM_INQUIRY = 0x0,
    FSM_TEST_UNIT_READY,
    FSM_READ_CAPACITY,
    FSM_END,
};


typedef struct _COMMAND_BLOCK_FORMAT {
    u8  operationCode;
    u8  LUN;                //<Logical Unit Number
    u8  LBA[4];             //<Logical Block Address[7:31]
    u8  Reserved;
    u8  LengthL;            //<Transfer or Parameter List or Allocation Length
    u8  LengthH;
    u8  XLength;            //<
    u8  Null[6];
}/*__attribute__((packed))*/ COMMAND_BLOCK_FORMAT;

typedef struct _INQUIRY_DATA {
    u8  PeripheralDeviceType;
    u8  RMB;
    u8  ISO;
    u8  ResponseDataFormat;
    u8  AdditionalLength;
    u8  Reserved[3];
    u8  VendorInfo[8];
    u8  ProductInfo[16];
    u8  ProductRevisionLevel[4];
}/*__attribute__((packed))*/ INQUIRY_DATA;

typedef struct _READ_CAPACITY_DATA {
    u8  LastLBA[4];
    u8  BlockLengthBytes[4];
}/*__attribute__((packed))*/ READ_CAPACITY_DATA;

typedef struct _REQUEST_SENSE_DATA {
    u8  ErrorCode;
    u8  Reserved;
    u8  SenseKey;
    u8  Info[4];
    u8  ASL;
    u8  Reserved1[4];
    u8  ASC;
    u8  ASCQ;
    u8  Reserved2[4];
}/*__attribute__((packed))*/ REQUEST_SENSE_DATA;


#endif  /*  _USB_SCSI_H_    */
