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
* �û�����ʵ�����ݣ�
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж�
	SystemCoreClockUpdate();//����ʱ��

	Delay_Init();
	check_iap_status();//IAP�̼������õ�

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

	uartx_preinit();//����GPIO�жϵ�����
    chry_dap_init();//��ʼ��DAP,���USB Device�豸���������˵�ȵ�����
    while (!usb_device_is_configured()) {
    }//�ȴ�USB���ö���������

	while(1)//����DAP���ݴ���ʹ������ݴ���
    {
        chry_dap_handle();
        chry_dap_usb2uart_handle();
	}
}
