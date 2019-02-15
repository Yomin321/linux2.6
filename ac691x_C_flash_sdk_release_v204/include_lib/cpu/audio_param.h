#ifndef __DAC_PARAM_H__
#define __DAC_PARAM_H__

#include "typedef.h"

//=============================================================
//
//					MAX VOL DEFINE
//		 used by libs,application can't use.
//
//=============================================================
#define ANALOG_VOL_LIMIT      31      ///<DAC模块模拟通道最大音量级数

//=============================================================
//
//					LADC PARAM DEFINE
//
//=============================================================
#define LADC_LINE_CHANNEL      		1      	//linein adc
#define LADC_MIC_CHANNEL            2		//mic adc

//ladc_support channel
#define LADC_MAX_CHANNEL			1

//ISEL
#define LADC_EN			BIT(0)
#define LADC_S1_ISEL	BIT(4)//[5:4]
#define LADC_ISEL		BIT(6)
#define LADC_PGA_EN		BIT(7)
#define LADC_PGA_G		BIT(11)//[11:8]:8(default)
#define LADC_PGA_ISEL	BIT(12)//mic set 1 , linein set 0
#define LADC_MIC_ISEL	BIT(17)
#define LADC_CFG	(LADC_EN | LADC_S1_ISEL | LADC_PGA_EN | LADC_PGA_G | LADC_PGA_ISEL) //reduce
//#define LADC_CFG	(LADC_EN | LADC_S1_ISEL | LADC_ISEL | LADC_PGA_EN | LADC_PGA_G | LADC_PGA_ISEL | LADC_MIC_ISEL)//normal

//=============================================================
//
//					DAC PARAM DEFINE
//
//=============================================================
//audio samplerate define
#define SR44100    	44100
#define SR48000		48000
#define SR32000  	32000
#define SR22050    	22050
#define SR24000    	24000
#define SR16000    	16000
#define SR11025    	11025
#define SR12000    	12000
#define SR8000     	8000

enum {
    SR_441K = 0x0,
    SR_48K,
    SR_32K,
    SR_2205K = 0x4,
    SR_24K,
    SR_16K,
    SR_11025K = 0x8,
    SR_12K = 0x9,
    SR_8K = 0xA,
};

enum {
    DAC_NO_ERR                     = 0,
    DAC_DRV_ISRFUN_RDEF_ERR        = -1000,
};

//通道最大值不能超出0x00ff，(channel_mode & 0xFF00)
enum {
    //digital channel
    DAC_DIGITAL_CH = 1,       ///<数字通道
    //analog channel
    DAC_AMUX0,
    DAC_AMUX1,
    DAC_AMUX2,
    DAC_AMUX0_L_ONLY,
    DAC_AMUX0_R_ONLY,
    DAC_AMUX1_L_ONLY,
    DAC_AMUX1_R_ONLY,
    DAC_AMUX2_L_ONLY,
    DAC_AMUX2_R_ONLY,
    DAC_AMUX_DAC_L,             ///<DAC_L input,DAC_R output
    DAC_AMUX_DAC_R,             ///<DAC_R input,DAC_L output
};

/*
*DAC MONO
*i.e. aux_ch = DAC_AMUX0,dac_ch = DAC_L
*linein_channel_open(DAC_AMUX0|DAC_MONO_L,1);
*/
#define DAC_MONO_L	1<<8
#define DAC_MONO_R	2<<8

#define LDO_1   1 //LDO1(default)
#define LDO_2   2 //LDO2

#define DAC_SAMPLE_CON      (0)
#define DAC_SAMPLE_POINT    (32 * (1 << DAC_SAMPLE_CON))
#define DAC_BUF_LEN         (DAC_SAMPLE_POINT*2*2)
#define DAC_DUAL_BUF_LEN    (DAC_SAMPLE_POINT * 2)

#define OUTPUT_BUF_SIZE  (576*2*2*3)      ///dac_cbuf size
#define OUTPUT_BUF_LIMIT (OUTPUT_BUF_SIZE/2)

//DAC_HP
#define DAC_L_CHANNEL     1    ///Left Channel
#define DAC_R_CHANNEL     2    ///Right Channel
#define DAC_L_R_CHANNEL   3    ///Left and Right Channel

//DAC_ISEL
#define DAC_ISEL5U      BIT(2)//4mA
#define DAC_ISEL_HALF	BIT(1)//2mA
#define DAC_ISEL_THIRD  BIT(0)//1.2mA
#define DAC_VSEL		(3<<4)//0~7(max),0xF means close AUDIO_LDO

//DAC_VCM_RSEL
#define VCM_RSEL_0		0	//power on slowly(BIG)
#define VCM_RSEL_1		1	//power on quickly(SMALL)

//=============================================================
//
//					AUDIO ERROR CODES
//
//=============================================================
typedef enum audio_err {
    AUDIO_ERR_NONE				= 0u,
    AUDIO_ERR_REPEAT				,
    AUDIO_ERR_CHANNEL				,
} AUDIO_ERR;

#endif
