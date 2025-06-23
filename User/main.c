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

#include "dap_offline.h"
#include "user_fatfs.h"

#include "debug.h"
#include "dap_main.h"
#include "usb2uart.h"

char FilePath[13];

/*********************************************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 *********************************************************************************************/
int main (void) {
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_2);  // 设置中断
    SystemCoreClockUpdate();                          // 设置时钟

    Delay_Init();
    USART_Printf_Init (115200);  // printf串口初始化

    printf ("SystemClk:%dMHz,ChipID:%08X\r\n\r\n", SystemCoreClock/1000000, DBGMCU_GetCHIPID());//打印系统信息

    fatfs_file_init();

    /**********************************dap_offline_download***************************/
    // rx_buff[1]:0-F103,1-F4xx
    // rx_buff[2]:NO. of bin from FatFs
    printf ("************dap_offline_download program begin!***************\r\n");
    FatReadDirTest (2, FilePath);
    printf ("FilePath:%s\r\n", FilePath);
    uint8_t error = dap_offline_download (fs, fnew, FilePath, F103x);  // McuType = 0,F103
    printf ("OffDAP!(%d)\r\n", error);
    printf ("************dap_offline_download program end!*****************\r\n");
    /********************************************************************************/

    // RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    // GPIO_PinRemapConfig (GPIO_Remap_SWJ_Disable, ENABLE);

    uartx_preinit();  // 串口GPIO中断等设置
    chry_dap_init();  // 初始化DAP,完成USB Device设备描述符，端点等的配置
    while (!usb_device_is_configured())
        ;             // 等待USB完成枚举配置完成
    printf ("usb_device_is_configured\r\n");

    while (1)  // 进入DAP数据处理和串口数据处理
    {
        chry_dap_handle();
        chry_dap_usb2uart_handle();
    }
}

void USBHS_RCC_Init (void) {
    RCC_USBCLK48MConfig (RCC_USBCLK48MCLKSource_USBPHY);
    RCC_USBHSPLLCLKConfig (RCC_HSBHSPLLCLKSource_HSE);
    RCC_USBHSConfig (RCC_USBPLL_Div2);  // V305:RCC_USBPLL_Div3
    RCC_USBHSPLLCKREFCLKConfig (RCC_USBHSPLLCKREFCLK_4M);
    RCC_USBHSPHYPLLALIVEcmd (ENABLE);
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_USBHS, ENABLE);
}

void usb_dc_low_level_init (void) {  // chry_dap_init()调用了
    USBHS_RCC_Init();
    NVIC_EnableIRQ (USBHS_IRQn);
}