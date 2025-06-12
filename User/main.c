/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/******************************************************************************
* 用户自行实现内容：
*
* 1.dap_main.c/h
* 2.DAP_config.h
* 3.usb_config.h
* 4.usb2uart.c/h
* 5.main.c
*******************************************************************************/

#include "debug.h"
#include "dap_main.h"
#include "usb2uart.h"

void USBHS_RCC_Init( void )
{
    RCC_USBCLK48MConfig( RCC_USBCLK48MCLKSource_USBPHY );
    RCC_USBHSPLLCLKConfig( RCC_HSBHSPLLCLKSource_HSE );
    RCC_USBHSConfig( RCC_USBPLL_Div3 );
    RCC_USBHSPLLCKREFCLKConfig( RCC_USBHSPLLCKREFCLK_4M );
    RCC_USBHSPHYPLLALIVEcmd( ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_USBHS, ENABLE );
}

void usb_dc_low_level_init(void)
{
    USBHS_RCC_Init( );
    NVIC_EnableIRQ( USBHS_IRQn );
}

void check_iap_status(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    volatile uint8_t io_status = 0;
    for(uint8_t i = 0;i < 10;i++)
    {
        io_status += GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7);
    }
    if (io_status == 0) {
        while(1)
        {
        }
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断
	SystemCoreClockUpdate();//设置时钟

	Delay_Init();
	check_iap_status();//IAP固件升级用的

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

	uartx_preinit();//串口GPIO中断等设置
    chry_dap_init();//初始化DAP,完成USB Device设备描述符，端点等的配置
    while (!usb_device_is_configured()) {
    }//等待USB完成枚举配置完成

	while(1)//进入DAP数据处理和串口数据处理
    {
        chry_dap_handle();
        chry_dap_usb2uart_handle();
	}
}
