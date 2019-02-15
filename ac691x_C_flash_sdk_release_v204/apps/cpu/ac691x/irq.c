#include "irq_api.h"
#include "irq.h"
#include "uart.h"
#include "sdk_cfg.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".irq_api_bss")
#pragma data_seg(	".irq_api_data")
#pragma const_seg(	".irq_api_const")
#pragma code_seg(	".irq_api_code")
#endif

const u8 irq_index_2_prio_tab[][2] = {
    {IRQ_EXCEPTION_IDX, 	0},
    {IRQ_TIME0_IDX, 		1},
    {IRQ_TIME1_IDX, 		0},
    {IRQ_TIME2_IDX, 		0},
    {IRQ_TIME3_IDX, 		0},
    {IRQ_USB_SOF_IDX, 		2},
    {IRQ_USB_CTRL_IDX, 		2},
    {IRQ_RTC_IDX, 			0},
    {IRQ_ALINK_IDX, 		2},
    {IRQ_DAC_IDX, 			2},
    {IRQ_PORT_IDX, 			0},
    {IRQ_SPI0_IDX, 			0},
    {IRQ_SPI1_IDX, 			0},
    {IRQ_SD0_IDX, 			1},
    {IRQ_SD1_IDX, 			1},
    {IRQ_UART0_IDX, 		0},
    {IRQ_UART1_IDX, 		3},
    {IRQ_UART2_IDX, 		0},
    {IRQ_PAP_IDX, 			0},
    {IRQ_IIC_IDX, 			0},
    {IRQ_SARADC_IDX, 		0},
    {IRQ_PDM_LINK_IDX, 		1},
    {IRQ_LRC_IDX, 		    1},
    {IRQ_BREDR_IDX, 		2},
    {IRQ_BT_CLKN_IDX, 		2},
    {IRQ_BT_DBG_IDX, 		1},
    {IRQ_BT_PCM_IDX, 		2},
    {IRQ_SRC_IDX, 			2},
    {IRQ_EQ_IDX, 			2},
    {IRQ_BLE_RX_IDX,		2},
    {IRQ_BLE_EVENT_IDX,		2},
    {IRQ_SOFT_REC_IDX, 		0},
    {IRQ_SOFT0_IDX, 		1},
    {IRQ_SOFT_IDX, 			0},
};

/*----------------------------------------------------------------------------*/
/**@brief  异常错误中断服务程序
   @param
   @return
   @note
 */
/*----------------------------------------------------------------------------*/
void exception_isr(void)
{
    u32 tmp;
    u32 sp;

    __asm__ volatile("trigger");

    asm("mov %0, RETS " : "=r"(tmp));
    asm("mov %0, sp" :"=r"(sp));
    otp_printf("\nRETS=0x%x\n", tmp);

    otp_printf("DEBUG_MSG = 0x%x\n", JL_DEBUG->MSG);
    otp_printf("PRP MMU_MSG = 0x%x\n", JL_DEBUG->PRP_MMU_MSG);
    otp_printf("LSB MMU MSG = 0x%x\n", JL_DEBUG->LSB_MMU_MSG_CH);
    otp_printf("PRP WR LIMIT MSG = 0x%x\n", JL_DEBUG->PRP_WR_LIMIT_MSG);
    otp_printf("LSB WR LIMIT MSG = 0x%x\n", JL_DEBUG->LSB_WR_LIMIT_CH);

    asm("mov %0, RETI " : "=r"(tmp));

    otp_printf("\nRETI=0x%x\n", tmp);
    otp_printf("\nSP=0x%x\n", sp);

    while (1);
    /* JL_POWER->CON |= BIT(4); //异常后软复位 */
}



void app_irq_init()
{
    irq_init();
    irq_index_tab_reg((void *)irq_index_2_prio_tab, sizeof(irq_index_2_prio_tab));

    irq_handler_register(IRQ_EXCEPTION_IDX, exception_isr, 0);
}


