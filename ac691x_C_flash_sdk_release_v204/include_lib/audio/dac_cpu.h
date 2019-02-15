#ifndef __DAC_CPU_H__
#define __DAC_CPU_H__

// include "hw_cpu.h"

/*---JL_AUDIO->CON sfr Setting----*/
#define DAC_SR(x)           SFR(JL_AUDIO->DAC_CON, 0, 4, x)
#define DAC_DIGITAL_EN(x)   SFR(JL_AUDIO->DAC_CON, 4, 1, x)
#define DAC_IE(x)           SFR(JL_AUDIO->DAC_CON, 5, 1, x)
#define DAC_CPND(x)         JL_AUDIO->DAC_CON |= BIT(6)
#define ADC_UDE(x)			SFR(JL_AUDIO->DAC_CON, 9, 1, x)
#define DAC_DCCS(x)         SFR(JL_AUDIO->DAC_CON, 12, 4, x)

/*---JL_AUDIO->CON1 sfr Setting----*/
#define DAC_DSMC(x)			SFR(JL_AUDIO->DAC_CON1,0,4,x)
#define DAC_PDMOE(x)		SFR(JL_AUDIO->DAC_CON1,4,1,x)
#define DAC_PDMRV(x)		SFR(JL_AUDIO->DAC_CON1,5,1,x)

/*---JL_AUDIO->LADC_CON sfr Setting----*/
#define LADC_SR(x)          SFR(JL_AUDIO->LADC_CON, 0, 4, x)
#define LADC_DIGITAL_EN(x)  SFR(JL_AUDIO->LADC_CON, 4, 1, x)
#define LADC_IE(x)          SFR(JL_AUDIO->LADC_CON, 5, 1, x)
#define LADC_CPND(x)        JL_AUDIO->LADC_CON |= BIT(6)
#define LADC_CHE(x)			SFR(JL_AUDIO->LADC_CON, 9, 1, x)
#define LADC_DCCS(x)        SFR(JL_AUDIO->LADC_CON, 12, 4, x)

/*---JL_AUDIO->LADC_CON1 sfr Setting----*/
#define LADC_EXS(x)         SFR(JL_AUDIO->LADC_CON1, 0,  5, x)
#define LADC_CLK_DIV(x)     SFR(JL_AUDIO->LADC_CON1, 5,  3, x)
#define LADC_OVER_SAPMLE(x) SFR(JL_AUDIO->LADC_CON1, 8,  1, x)
#define LADC_CTL_CPU(x)		SFR(JL_AUDIO->LADC_CON1, 11, 1, x)
#define ADC_ANALOG_EN(x)    SFR(JL_AUDIO->LADC_CON1, 12, 1, x)
#define ADC_RESET(x)       	SFR(JL_AUDIO->LADC_CON1, 13, 1, x)
#define ADC_CURRENT(x)		SFR(JL_AUDIO->LADC_CON1, 14, 2, x)

/*---JL_AUDIO->LADC_CON2 sfr Setting----*/
#define LADC_CH0SS(x)		SFR(JL_AUDIO->LADC_CON2,  0, 3, x)
#define LADC_CH1SS(x)		SFR(JL_AUDIO->LADC_CON2,  4, 3, x)
#define LADC_CH2SS(x)		SFR(JL_AUDIO->LADC_CON2,  8, 3, x)
#define LADC_CH3SS(x)		SFR(JL_AUDIO->LADC_CON2, 12, 3, x)

/*---JL_AUDIO->DAA_CON0 sfr Setting----*/
#define DAC_ANALOG_EN(x)    SFR(JL_AUDIO->DAA_CON0, 0, 1, x)
//BIT(1):reserved
#define LDO1_EN(x)          SFR(JL_AUDIO->DAA_CON0, 2, 1, x)
#define LDO2_EN(x)          SFR(JL_AUDIO->DAA_CON0, 3, 1, x)
#define HP_L_EN(x)          SFR(JL_AUDIO->DAA_CON0, 4, 1, x)
#define HP_R_EN(x)          SFR(JL_AUDIO->DAA_CON0, 5, 1, x)
#define PNS_EN(x)           SFR(JL_AUDIO->DAA_CON0, 6, 1, x)
#define PNS10k_EN(x)        SFR(JL_AUDIO->DAA_CON0, 7, 1, x)
#define LDO_ISEL(x)			SFR(JL_AUDIO->DAA_CON0, 8, 3, x)
#define DAC_DTSEL(x)		SFR(JL_AUDIO->DAA_CON0, 11, 2, x)
#define TRIM_EN(x)          SFR(JL_AUDIO->DAA_CON0, 13, 1, x)
#define TRIM_SEL(x)         SFR(JL_AUDIO->DAA_CON0, 14, 1, x)
#define TRIM_SW(x)          SFR(JL_AUDIO->DAA_CON0, 15, 1, x)
#define CHANNEL_L_GAIN(x)   SFR(JL_AUDIO->DAA_CON0, 16, 5, x)
#define LR_2_L(x)           SFR(JL_AUDIO->DAA_CON0, 21, 1, x)
#define LR_2_R(x)           SFR(JL_AUDIO->DAA_CON0, 22, 1, x)
#define MUTE_EN(x)          SFR(JL_AUDIO->DAA_CON0, 23, 1, x)
#define CHANNEL_R_GAIN(x)   SFR(JL_AUDIO->DAA_CON0, 24, 5, x)
#define MIC_2_L(x)          SFR(JL_AUDIO->DAA_CON0, 29, 1, x)
#define MIC_2_R(x)          SFR(JL_AUDIO->DAA_CON0, 30, 1, x)
#define TRIM_OUT(x)         SFR(JL_AUDIO->DAA_CON0, 31, 1, x)
#define TRIM_OUT_RD      	(JL_AUDIO->DAA_CON0 & BIT(31))

/*---JL_AUDIO->DAA_CON1 sfr Setting----*/
#define LIN0_CHANNL_L_EN(x) SFR(JL_AUDIO->DAA_CON1, 0, 1, x)
#define LIN0_CHANNL_R_EN(x) SFR(JL_AUDIO->DAA_CON1, 1, 1, x)
#define LIN0L_BIAS_EN(x) 	SFR(JL_AUDIO->DAA_CON1, 2, 1, x)
#define LIN0R_BIAS_EN(x) 	SFR(JL_AUDIO->DAA_CON1, 3, 1, x)
#define LIN1_CHANNL_L_EN(x) SFR(JL_AUDIO->DAA_CON1, 4, 1, x)
#define LIN1_CHANNL_R_EN(x) SFR(JL_AUDIO->DAA_CON1, 5, 1, x)
#define LIN1L_BIAS_EN(x)    SFR(JL_AUDIO->DAA_CON1, 6, 1, x)
#define LIN1R_BIAS_EN(x)    SFR(JL_AUDIO->DAA_CON1, 7, 1, x)
#define LINL_2_ADC(x) 		SFR(JL_AUDIO->DAA_CON1, 8, 1, x)
#define LINR_2_ADC(x) 		SFR(JL_AUDIO->DAA_CON1, 9, 1, x)
#define LPF_ISEL(x) 		SFR(JL_AUDIO->DAA_CON1, 10, 3, x)
#define VCM_EN(x)           SFR(JL_AUDIO->DAA_CON1, 13, 1, x)
#define VCM_OUT_EN(x)       SFR(JL_AUDIO->DAA_CON1, 14, 1, x)
#define VCM_OUT_PD(x)       SFR(JL_AUDIO->DAA_CON1, 15, 1, x)
#define MIC_EN(x)           SFR(JL_AUDIO->DAA_CON1, 16, 1, x)
#define MIC_ISEL(x)         SFR(JL_AUDIO->DAA_CON1, 17, 1, x)
#define MIC_MUTE(x)         SFR(JL_AUDIO->DAA_CON1, 18, 1, x)
#define MIC_NEG12(x)  		SFR(JL_AUDIO->DAA_CON1, 19, 1, x)
#define AMUX_BIAS_EN(x)     SFR(JL_AUDIO->DAA_CON1, 20, 1, x)
#define AMUX_EN(x)          SFR(JL_AUDIO->DAA_CON1, 21, 1, x)
#define AMUX_G(x)           SFR(JL_AUDIO->DAA_CON1, 22, 1, x)
#define AMUX_MUTE(x)        SFR(JL_AUDIO->DAA_CON1, 23, 1, x)
#define MIC_GAIN(x)         SFR(JL_AUDIO->DAA_CON1, 24, 6, x)
#define MIC_GX2(x)       	SFR(JL_AUDIO->DAA_CON1, 30, 1, x)
#define VCM_RSEL(x)         SFR(JL_AUDIO->DAA_CON1, 31, 1, x)

#define LIN2_CHANNL_L_EN(x)
#define LIN2_CHANNL_R_EN(x)
#define LIN2L_BIAS_EN(x)
#define LIN2R_BIAS_EN(x)

/*---JL_AUDIO->DAA_CON2 reserved----*/
/*---JL_AUDIO->DAA_CON3 sfr Setting----*/
#define DAC_CLKEN(x)        SFR(JL_AUDIO->DAA_CON3, 0, 1, x)
#define DAC_DATEN(x)		SFR(JL_AUDIO->DAA_CON3, 1, 1, x)
#define DAC_EXT(x)          SFR(JL_AUDIO->DAA_CON3, 2, 1, x)
#define ADC_COE(x)          SFR(JL_AUDIO->DAA_CON3, 3, 1, x)
#define ADC_DOE(x)          SFR(JL_AUDIO->DAA_CON3, 4, 1, x)
#define ADC_DIT(x)          SFR(JL_AUDIO->DAA_CON3, 5, 1, x)

#endif
