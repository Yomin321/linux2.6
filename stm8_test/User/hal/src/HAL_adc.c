
#include "main.h"

ADC_STRUCT  HAL_adc_StrPar;



void HAL_adc_ValueInit(void)
{
	HAL_adc_StrPar.DataTmp = 0;
	HAL_adc_StrPar.Datacnt = 0;
	HAL_adc_StrPar.WorkGrade = 0;
}

void HAL_ADC_Init(void)
{
  u8 idx;
	 
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);//使能ADC时钟 
 // CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);//使能DMA1时钟

  ADC_DeInit(ADC1);
  
  GPIO_Init(GPIOA,GPIO_Pin_4,GPIO_Mode_In_FL_No_IT);//浮空输入
  
  for (idx = 0;idx < 50;idx++);//adc上电需要一段时间
       
  ADC_VrefintCmd(ENABLE);//使能内部参考电压
  
 // ADC_DMACmd(ADC1,DISABLE);//使用ADC的DMA功能
  
  //连续转换模式
  ADC_Init(ADC1, ADC_ConversionMode_Continuous,ADC_Resolution_12Bit, ADC_Prescaler_2);
  
 // ADC_SamplingTimeConfig(ADC1, ADC_Group_FastChannels, ADC_SamplingTime_192Cycles);
  ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels,ADC_SamplingTime_192Cycles);
                           
                            
 // ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint, ENABLE);//内部参考电压
  ADC_ChannelCmd(ADC1, ADC_Channel_2, ENABLE);//通道2   PA4

  // disable SchmittTrigger for ADC_Channel_1, to save power 
  ADC_SchmittTriggerConfig(ADC1, ADC_Channel_2, DISABLE);//禁止施密特触发器

  /* Enable Analog watchdog ADC1 Interrupt */
  //ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  
  ADC_Cmd(ADC1,ENABLE);//打开ADC
  ADC_SoftwareStartConv(ADC1);//software trigger
  //ADC_SoftwareStartConv(ADC1);//software trigger*/
}




/*
	获得电池电压
*/
void APP_ADC_GetWorkVolGrade(void)
{
	u8 s_tmp[18];
	u8 cnt;
	HAL_adc_StrPar.DataTmp += ADC_GetConversionValue(ADC1);
	HAL_adc_StrPar.Datacnt ++;
		
	if(HAL_adc_StrPar.Datacnt == 5)
	{
		HAL_adc_StrPar.AverDataTmp = HAL_adc_StrPar.DataTmp/5;
		HAL_adc_StrPar.DataTmp = 0;
		HAL_adc_StrPar.Datacnt = 0;
		if(HAL_adc_StrPar.AverDataTmp > 1150 )//电压高于6.5v
		{
			HAL_adc_StrPar.WorkGrade = 1;
			//高压告警 ，数据准备
			s_tmp[0] = 0x01;
			for(cnt=0;cnt<16;cnt++)
			{
				s_tmp[cnt+1] = 0x00;
			}
			s_tmp[3] = HAL_adc_StrPar.WorkGrade;
			s_tmp[16] = DEV_TypeClass;
			
			PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER,TRFER_CMD_SENDSTATUS,s_tmp,17);
			
		}
		else if(HAL_adc_StrPar.AverDataTmp > 800) //电池电压高于 5v
		{
			HAL_adc_StrPar.WorkGrade = 2;
		}
		else 		//电机不再工作,spider能工作
		{
			HAL_adc_StrPar.WorkGrade = 3;
			//低压告警 ，数据准备
			s_tmp[0] = 0x01;
			for(cnt=0;cnt<16;cnt++)
			{
				s_tmp[cnt+1] = 0x00;
			}
			s_tmp[3] = HAL_adc_StrPar.WorkGrade;
			s_tmp[16] = DEV_TypeClass;
			
			PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER,TRFER_CMD_SENDSTATUS,s_tmp,17);
		}
	}
		
}





























