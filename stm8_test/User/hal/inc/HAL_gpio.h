#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_


// ---------------------------SPIDER-TM ¸´Î»Òý½Å-------------
#define  SPIDER_TM_RESET_ENABLE()   GPIO_ResetBits(GPIOC,GPIO_Pin_4)
#define  SPIDER_TM_RESET_DISABLE()	 GPIO_SetBits(GPIOC,GPIO_Pin_4)


void HAL_GPIO_config(void);



#endif
