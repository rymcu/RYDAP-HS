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
#include "ff.h"
#include "string.h"
#include "dap_offline.h"
#include "my_data_array.h"
#include "bsp_spi_flash.h"

#include "debug.h"
#include "dap_main.h"
#include "usb2uart.h"

void USBHS_RCC_Init (void) {
    RCC_USBCLK48MConfig (RCC_USBCLK48MCLKSource_USBPHY);
    RCC_USBHSPLLCLKConfig (RCC_HSBHSPLLCLKSource_HSE);
    RCC_USBHSConfig (RCC_USBPLL_Div2);  // V305:RCC_USBPLL_Div3
    RCC_USBHSPLLCKREFCLKConfig (RCC_USBHSPLLCKREFCLK_4M);
    RCC_USBHSPHYPLLALIVEcmd (ENABLE);
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_USBHS, ENABLE);
}

void usb_dc_low_level_init (void) {
    USBHS_RCC_Init();
    NVIC_EnableIRQ (USBHS_IRQn);
}

/*********************************************************************
 * @fn      show_sdcard_info
 *
 * @brief   SD Card information.
 *
 * @return  none
 */
/****************************************************************
 * Reads all files in the specified directory
 * f_readdir():read the files sequentially
 * call the function repeatedly to read all files in the directory
 ****************************************************************/
FATFS fs;
static FIL fnew;
UINT fnum;
char FilePath[13];
FRESULT res_sd;
static uint8_t work_buf[4096];// __attribute__((aligned(4)));  // 8KB静态缓冲区
u8 WriteBuf[] = {"Hello WCH!"};
u8 ReadSDBuf[11] = {0};
MKFS_PARM opt = {0};

void fatfs_file_init (void);

void FatReadDirTest (char *path, uint8_t flag) {
    DIR fnew;                // 修正：声明为DIR类型（目录对象）
    FILINFO FilInfo;
    uint8_t Tempflag;
    char* dir_path = (char*)path;  // 转换路径类型
    
    Tempflag = flag;

    // 修正：确保fnew是DIR类型，path作为第二个参数
    if (f_opendir(&fnew, dir_path) == FR_OK) {
        while (f_readdir(&fnew, &FilInfo) == FR_OK) {
            if (!FilInfo.fname[0])
                break;
            printf("fname:%s\r\n", FilInfo.fname);
            if (strstr(FilInfo.fname, "BIN") && Tempflag) {
                // 复制文件名（优化版）
                strncpy(FilePath, FilInfo.fname, 10);
                FilePath[12] = '\0';  // 确保字符串终止
                Tempflag--;
            }
        }
        f_closedir(&fnew);  // 关闭目录（避免资源泄漏）
    }
}    

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main (void) {
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_2);  // 设置中断
    SystemCoreClockUpdate();                          // 设置时钟

    Delay_Init();
    usb_dc_low_level_init();

    USART_Printf_Init (115200);
    printf ("SystemClk:%d\r\n", SystemCoreClock);
    printf ("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    fatfs_file_init();

    /**********************************dap_offline_download************************************/
    // rx_buff[1]:0-F103,1-F4xx
    // rx_buff[2]:NO. of bin from FatFs
    printf ("*********************dap_offline_download program begin!*********************\r\n");
    FatReadDirTest("0:",2);
    printf("FilePath:%s\r\n",FilePath);
    uint8_t error = dap_offline_download(fs, fnew, FilePath, F103x);  // McuType = 0,F103
    printf ("OffDAP!(%d)\r\n", error);
    printf ("*********************dap_offline_download program end!*********************\r\n");
    /********************************************************************************/

    // RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    // GPIO_PinRemapConfig (GPIO_Remap_SWJ_Disable, ENABLE);

    uartx_preinit();  // 串口GPIO中断等设置
    chry_dap_init();  // 初始化DAP,完成USB Device设备描述符，端点等的配置
    while (!usb_device_is_configured());// 等待USB完成枚举配置完成
    printf ("usb_device_is_configured\r\n");

    while (1)  // 进入DAP数据处理和串口数据处理
    {
        chry_dap_handle();
        chry_dap_usb2uart_handle();
    }
}

void fatfs_file_init (void) {
    res_sd = f_mount (&fs, "0:", 1);  // SD mount
    if (res_sd == FR_OK) {
        printf ("Disk mounted successfully\r\n");
    }
    if (res_sd == FR_NO_FILESYSTEM) {
        printf ("Disk formatting\r\n");
        res_sd = f_mkfs ("0:", &opt, work_buf, sizeof (work_buf));
        if (res_sd == FR_OK) {
            printf ("Disk formatted successfully\r\n");
            res_sd = f_mount (&fs, "0:", 1);
            if (res_sd == FR_OK) {
                printf ("Disk mounted successfully\r\n");
            } else {
                printf ("Disk mounting failed\r\n");
                printf ("error code%x\r\n", res_sd);
            }
        } else {
            printf ("Disk formatting failed!(%d)\r\n", res_sd);
        }
    }
    res_sd = f_open (&fnew, (const char *)"0:f103.bin", FA_CREATE_ALWAYS | FA_WRITE);
    if (res_sd != FR_OK) {
        printf ("Create file error\r\n");
    } else {
        printf ("Create file successfully\r\n");
    }
    printf ("Writing......................\r\n");
    res_sd = f_write (&fnew, my_data_array, sizeof (my_data_array), &fnum);
    for (int j = 0; j < sizeof (my_data_array); j++) {
        printf ("%02X ", my_data_array[j]);
    }
    if (res_sd != FR_OK) {
        printf ("Write error\r\n");
    }
    f_close (&fnew);
    res_sd = f_open (&fnew, (const char *)"0:f103.bin", FA_OPEN_EXISTING | FA_READ);
    if (res_sd != FR_OK) {
        printf ("Open file error\r\n");
    }
    printf ("Reading......................\r\n");
    res_sd = f_read (&fnew, ReadSDBuf, 11, (UINT *)&fnum);
    if (res_sd != FR_OK) {
        printf ("Read error\r\n");
    }
    for (int i = 0; i < sizeof (ReadSDBuf); i++) {
        printf ("ReadBuf[%d]=%02X\r\n", i, ReadSDBuf[i]);
    }
    f_close (&fnew);
}
